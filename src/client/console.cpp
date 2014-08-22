#include "console.h"

#include <SFML/Window.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "serialize.hpp"
#include "view_base.h"
#include "renderer.h"
#include "expert_ai.h"
#include "null_ai.h"

namespace col {
	using boost::str;

	Layout const ly(SCREEN_W, SCREEN_H);


	Color make_color_by_name(const string &s) {
		if (s == "red") {
			return Color(255,0,0);
		}
		else
		if (s == "green") {
			return Color(0,255,0);
		}
		else
		if (s == "blue") {
			return Color(0,0,255);
		}
		else {
			return Color(0,0,0);
		}
	}

	uint8 flag_id4color_name(const string &s) {
		if (s == "red") {
			return 119;
		}
		else
		if (s == "blue") {
			return 120;
		}
		else
		if (s == "yellow") {
			return 120;
		}
		else
		if (s == "orange") {
			return 120;
		}
		else
		if (s == "usa") {
			return 131;
		}
		else {
			return 65;
		}
	}

	halo::Mod get_mod() {
		halo::Mod mod = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) {
			mod = mod | halo::ModShift;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
			mod = mod | halo::ModCtrl;
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
			mod = mod | halo::ModButton;
		}
		return mod;	
	}
	
	void Console::handle(sf::RenderWindow const& app, sf::Event const& event) {
		
		
		auto type = event.type;

		// prepare pattern
		Pattern p;
		
		if (type == sf::Event::KeyPressed) {
			p.dev = Dev::Keyboard;
			p.event = Event::Press;
			p.key = event.key.code;		
		}
		else if (type == sf::Event::TextEntered) {
			p.dev = Dev::Keyboard;
			p.event = Event::Char;
			p.unicode = event.text.unicode;
		}
		else if (type == sf::Event::MouseMoved)			
		{			
			sf::Vector2f mp = app.mapPixelToCoords(
				sf::Vector2i(
					event.mouseMove.x,
					event.mouseMove.y
				)
			);
			
			p.dev = Dev::Mouse;
			p.event = Event::Hover;
			p.mod = get_mod();
			p.area = Box2(v2i(mp.x, mp.y), {0,0});
		}		
		else if (type == sf::Event::MouseButtonPressed)
		{			
			sf::Vector2f mp = app.mapPixelToCoords(
				sf::Vector2i(
					event.mouseButton.x,
					event.mouseButton.y
				)
			);
			
			p.dev = Dev::Mouse;
			p.event = Event::Press;
			p.mod = get_mod();
			p.area = Box2(v2i(mp.x, mp.y), {0,0});
			
			
			if (event.mouseButton.button == sf::Mouse::Left) {
				p.button = Button::Left;
			}			
			else if (event.mouseButton.button == sf::Mouse::Right) {
				p.button = Button::Right;
			}			
		}
		else if (type == sf::Event::MouseButtonReleased)
		{	
			sf::Vector2f mp = app.mapPixelToCoords(
				sf::Vector2i(
					event.mouseButton.x,
					event.mouseButton.y
				)
			);
			
			p.dev = Dev::Mouse;
			p.event = Event::Release;
			p.mod = get_mod();
			p.area = Box2(v2i(mp.x, mp.y), {0,0});
			
			
			if (event.mouseButton.button == sf::Mouse::Left) {
				p.button = Button::Left;
			}			
			else if (event.mouseButton.button == sf::Mouse::Right) {
				p.button = Button::Right;
			}			
		}
		
		
		
		// handle
		if (halo::handle_event(hts, p)) {
			modified();
		}
	}

	/*
	struct NotEnoughArgs: std::runtime_error {}
	
	string const& arg(vector<string> const& es, int n) {
		if (n >= es.size()) {
			throw NotEnoughArgs();
		}
		return es[n];
	}*/
	
	
	
	void Console::handle_char(uint16 code) {
		
		if (!is_active()) {
			throw Critical("console not active");
		}
		
		if (charset.find(code) == charset.end()) {
			return;
		}

		if (code == u'\b') {
			if (buffer.size()) {
				buffer.pop_back();
			}
		}
		else if (code == u'\r') {

			command(buffer);
			
			history.push_front(buffer);
			buffer = "";
			chi = history.begin();

		}
		else {
			//cout << char(code);
			buffer.push_back(char(code));
		}

		//cout.flush();
		++mod;
	}
	
	Item Console::get_next_workitem_field(Env const& env, Field const& f) const {
		// return next item availbe for production in this workplace
		
		// ble :P
		auto lst = vector<Item>{ItemNone,
			ItemFood,ItemFish,ItemSugar,ItemTobacco,ItemCotton,ItemFurs,
			ItemLumber,ItemOre,ItemSilver,ItemHorses,
			ItemRum,ItemCigars,ItemCloth,ItemCoats,
			ItemTradeGoods,
			ItemTools,ItemMuskets,
			ItemHammers,ItemCross,
			ItemBell
		};
		
		size_t curr_pos = find(lst.begin(), lst.end(), f.get_proditem()) - lst.begin();
		
		size_t i = curr_pos + 1;
		while (i < lst.size()) {
			if (f.get_terr().get_yield(lst.at(i), false)) {
				return lst.at(i);
			}
			++i;
		}
		
		i = 0;
		while (i < curr_pos) {
			if (f.get_terr().get_yield(lst.at(i), false)) {
				return lst.at(i);
			}
			++i;
		}
		
		return ItemNone;
	}
	
	
	void Console::command(string const& line) {
		try {
			do_command(line);
		}
		catch (Error const& e) {
			put(string(e.what()));
		}
		
	}
	
	void Console::do_command(string const& line) {
		
		vector<string> es;
		split(es, line, is_any_of(" "));
		
		Unit::Id sel_unit_id = get_sel_unit_id();
		Terr::Id sel = get_sel_terr_id();
		
		auto& cmd = es[0];

		put(line);
		auto& con = *this;
		
		if (cmd == "list-nations") {
			for (auto &item: env.nations) {
				put(format("%||", item.second));
			}
		}
		else if (cmd == "comment") {

		}
		else if (cmd == "print") {
			put(buffer.substr(6));
		}
		else if (cmd == "list-orders") {
			put("build-colony");
			put("plow-fields");
			put("clear-forest");
			put("build-road");
			put("move");
		}
		else if (cmd == "help" or cmd == "?") {
			// save/load
			put("save");
			put("load");
			// editor
			put("list-nations");
			put("list-units");
			put("list-unit-types");
			put("list-orders");
			put("create-nation");
			put("create-unit");
			put("create-colony");
			put("delete-colony");
			put("set-biome");
			put("add-phys");
			put("sub-phys");
			put("set-alt");
			// orders
			put("build-colony");
			put("plow-fields");
			put("clear-forest");
			put("build-road");
			put("move");
			put("attack");
			put("work-build");
			put("work-field");
			put("work-none");
			put("prod-build");
			put("prod-field");			
			put("construct");
			// game turn sequence
			put("start");
			put("ready");
			put("turn");
			put("info");
			// ai
			put("ai");
			//put("create-ai");				
			//put("exec-ai");
			//put("list-ais");
			// misc
			put("score");
			put("cls");
			put("print");
			put("comment");
			put("enter");
			put("exit");
		}
		else if (cmd == "add-phys") {
			switch (es.size()) {
				default:
					put("Usage: add-phys <phys-name>");
					break;
				case 2:
					Phys p = get_phys_by_name(es.at(1));
					for (auto tp: get_sel_terrs()) {
						tp->add_phys(p);						
					}
					break;
			}
		}
		else if (cmd == "sub-phys") {
			switch (es.size()) {
				default:
					put("Usage: sub-phys <phys-name>");
					break;
				case 2:
					Phys p = get_phys_by_name(es.at(1));
					for (auto tp: get_sel_terrs()) {
						tp->sub_phys(p);						
					}
					break;
			}
		}		
		else if (cmd == "cls") {
			output.clear();
		}
		else if (cmd == "info") {
			switch (es.size()) {
				default:
					put("Usage: info");
					break;
				case 1:
					put("Turn: " + to_string(env.get_turn_no()) + "; " +
						"Nation: " + env.get_current_nation().name  + "; " +
						"State: " + to_string(env.state)
					);
					break;
			}
		}		
		else if (cmd == "connect") {
			switch (es.size()) {
				default:
					put("Usage: connect <nation-id>");
					break;
				case 2:
					env.connect(stoi(es.at(1)), *this);					
					break;
			}
		}
		else if (cmd == "score") {
			switch (es.size()) {
				default:
					put("Usage: score <nation_id>");
					break;
				case 2:
					auto f = env.get_result(stoi(es.at(1)));
					put(to_string(f));
					break;
			}
		}
		else if (cmd == "set-biome") {
			switch (es.size()) {
				default:
					put("Usage: set-biome <biome-name>");
					break;
				case 2:
					Biome b = get_biome_by_name(es.at(1));					
					for (auto tp: con.get_sel_terrs()) {
						tp->set_biome(b);	
					}
					break;
			}
		}
		else if (cmd == "set-alt") {
			switch (es.size()) {
				default:
					put("Usage: set-alt <0,1,2,3>");
					break;
				case 2:
					for (auto tp: get_sel_terrs()) {
						tp->set_alt(stoi(es.at(1)));						
					}
					break;
			}
		}
		else if (cmd == "turn") {
			env.turn();
			put(format("Turn %||", env.turn_no));
		}
		else if (cmd == "expert") {
			switch (es.size()) {
				default: {
					output.push_back("Usage: expert <pid>\n");
					break;
				}
				case 2: {
					auto pid = std::stoi(es.at(1));
					auto & p = env.get<Nation>(pid);
					if (p.player == nullptr) {
						ths.emplace_back(expert_ai::run, pid, &env, &running);
						put("expert ai connected to nation");
					}
					break;
				}
			}
		}
		else if (cmd == "null-ai") {
			switch (es.size()) {
				default: {
					output.push_back("Usage: null-ai <pid>\n");
					break;
				}
				case 2: {
					auto pid = std::stoi(es.at(1));
					auto & p = env.get<Nation>(pid);
					if (p.player == nullptr) {
						ths.emplace_back(null_ai::run, pid, &env, &running);
						put("null ai connected to nation");
					}
					break;
				}
			}
		}
		else if (cmd == "ai") {				
			// calculate and show preffered move for current nation
			switch (es.size()) {
				default: {
					output.push_back("Usage: ai <num> <exec>\n");
					break;
				}
				case 3: {
					if (env.in_progress()) {
						auto & p = env.get_current_nation();
						if (p.player == nullptr) {
							put("creating ai for this nation");
							
							ths.emplace_back(Ai::run, p.id, &env);
						}

						auto run_mode = stoi(es.at(2));  // 0 no, 1 step, 2 full turn
						auto power = stoi(es.at(1));
						
						/*
						auto& ai = ais.at(pid);
						
						if (run_mode == 1) {
							auto const& a = ai.calc(env, power, 0);
							put(string("action: ") + to_string(a));
							exec(a);
						}
						else {
							while (1) {
								auto const& a = ai.calc(env, power, 0);
								put(string("action: ") + to_string(a));
								exec(a);
								if (a == Ready(pid)) {
									break;
								}
							}

						}
						*/
						

					}
					else {
						put("game in regress; start game first");
					}
					break;
				}
			}
		}
		else if (cmd == "start") {
			switch (es.size()) {
				default: {
					output.push_back("Usage: start\n");
					break;
				}
				case 1: {
					env.start();
					put("Game started!");
					select_next_unit();
					break;
				}
			}
		}
		else if (cmd == "ready") {
			switch (es.size()) {
				default: {
					output.push_back("Usage: ready [nation_id]\n");
					break;
				}
				case 2: {
					exec(Ready(std::stoi(es.at(1))));
					select_next_unit();
					break;
				}
				case 1: {
					//exec(Ready(env.get_current_nation().id));
					env.ready(env.get_current_nation());
					select_next_unit();
					break;
				}
			}
		}
		else if (cmd == "list-units") {
			for (auto &item: env.units) {
				put(format("%||", item.second));
			}
		}
		else if (cmd == "list-unit-types") {
			for (auto &utp: *env.uts) {
				auto &ut = utp.second;
				put(format("%u: %s", uint16(ut.id), ut.name));
			}
		}
		else if (es[0] == "remove-unit") {
			env.remove<Unit>(std::stoi(es[1]));			    
		}
		else if (cmd == "create-unit") {
			switch (es.size()) {
				default: {
					put("Usage: create-unit <type_id> <nation_id>");
					break;
				}
				case 3: {
					auto& c = env.get<UnitType>(std::stoi(es.at(1))); // type_id
					auto& p = env.get<Nation>(std::stoi(es.at(2)));  // nation_id
					auto& t = *get_sel_terr(); 
					auto& u = env.create<Unit>(c, p);
					env.init(t, u);
					if (compatible(u.get_travel(), LAND) and
						!compatible(u.get_travel(), t.get_travel())) 
					{
						u.transported = true;
					}
					break;
				}
			}
		}
		else if (cmd == "create-nation") {
			switch (es.size()) {
				default: {
					put("Usage: create-nation <nation-name> <color-name>");
					break;
				}
				case 3: {
					env.create<Nation>(
						string(es[1]), // name
						make_color_by_name(string(es[2])), // color
						flag_id4color_name(string(es[2]))  // flag_id
					);
					break;
				}
			}
		}
		else if (cmd == "delete-colony") {
			switch (es.size()) {
				default:
					put("Usage: delete-colony");
					break;
				case 1:
					auto &t = *get_sel_terr();
					if (t.has_colony()) {
						env.burn_colony(t);
					}
					else {
						put("there is no colony at selected location");
					}
					break;
			}
		}
		else if (es[0] == "sel") {
			switch (es.size()) {
				default: {
					output.push_back("Usage: sel <x> <y> OR sel <unit_id>\n");
					break;
				}
				case 3: {
					select_terr(Coords(std::stoi(es.at(1)), std::stoi(es.at(2))));
					break;
				}
				case 2: {
					auto unit_id = std::stoi(es.at(1));
					try {
						env.get<Unit>(unit_id);
						select_unit(unit_id);
					}
					catch (Error const& e) {
						put(e.what());
					}					
					break;
				}
			}
		}
		else if (es[0] == "setturn") {
			switch (es.size()) {
				default: {
					output.push_back("Usage1: setturn num\n");
					break;
				}
				case 2: {
					env.turn_no = stoi(es.at(1));
					break;
				}
			}
		}
		else if (es[0] == "delp") {
			env.del_nation(std::stoi(es[1]));
		}
		else if (cmd == "save") {
			switch (es.size()) {
				default:
					put("Usage: save <filename>");
					break;
				case 2: {
						ofstream f(es.at(1), std::ios::binary);
						boost::archive::text_oarchive oa(f);
						oa << env;
					}
					break;
			}
		}
		else if (cmd == "load") {
			switch (es.size()) {
				default:
					put("Usage: load <filename>");
					break;
				case 2: {
						ifstream f(es.at(1), std::ios::binary);
						boost::archive::text_iarchive oa(f);
						oa >> env;
					}
					break;
			}
		}
		else if (es[0] == "set_owner") {
			env.set_owner(
				stoi(es[1]),
				stoi(es[2])
			);
		}
		else if (cmd == "build-road") {
			switch (es.size()) {
				default:
					put("Usage: build-road");
					break;
				case 1:
					if (auto up = get_sel_unit()) {
						auto ret = env.improve(*up, PhysRoad);
						if (ret) {
							mem.set_order(up->id, '-');							
						}
						else {
							mem.set_order(up->id, 'R');							
						}
						select_next_unit();
					}
					else {
						put("no unit selected");
					}
					break;
			}
			
		}
		else if (cmd == "add-item") {
			switch (es.size()) {
				default:
					put("Usage: add-item <item-name> <amount>");
					break;
				case 3:
					auto number = stoi(es.at(2));
					auto item = get_item_by_name(es.at(1));

					auto &terr = *get_sel_terr();
					terr.get_colony().add(item, number);
					break;
			}
		}
		else if (cmd == "construct") {			
			switch (es.size()) {
				default:
					put("Usage: construct <place-number> <building-id>");
					break;
				case 3: {
					auto place_id = stoi(es.at(1));
					auto buildtype_id = stoi(es.at(2));
					
					auto& c = get_sel_terr()->get_colony();
					env.colony_construct(c, buildtype_id, place_id);
					
					}
					break;
			}
		}
		else if (cmd == "work-build") {
			
			switch (es.size()) {
				default:
					put("Usage: work-build <build-num>");
					break;
				case 2: {
					auto number = stoi(es.at(1));
					auto unit_id = get_sel_unit_id();
					auto& unit = env.get<Unit>(unit_id);
					env.work_build(number, unit);
					
					break;
				}
				
			}
		}
		else if (cmd == "equip") {
			
			switch (es.size()) {
				default:
					put("Usage: equip <unit-type-id>");
					break;
				case 2: {
					auto id = stoi(es.at(1));
					env.equip(
						*get_sel_unit(), 
						env.get<UnitType>(id)
					);
					break;
				}
				
			}
		}
		else if (cmd == "work-field") {
			
			switch (es.size()) {
				default:
					put("Usage: work-field <field-num>");
					break;
				case 2: {
					auto number = stoi(es.at(1));
					auto unit_id = get_sel_unit_id();
					auto& unit = env.get<Unit>(unit_id);
					env.work_field(number, unit);
					
					break;
				}
				
			}
		}
		else if (cmd == "work-none") {
			switch (es.size()) {
				default:
					put("Usage: work-none");
					break;
				case 1: {
					auto unit_id = get_sel_unit_id();
					auto& unit = env.get<Unit>(unit_id);
					env.work_none(unit);
					break;
				}
				
			}
		}
		else if (cmd == "prodnext-field") {			
			switch (es.size()) {
				default:
					put("Usage: prodnext-field <field-num>");
					break;
				case 2: {
					auto number = stoi(es.at(1));
					auto const& item = get_next_workitem_field(
						env, 
						env.get_field(env.get_terr(sel), number)
					);
					env.set_proditem_field(env.get_terr(sel), number, item);
					break;
				}
				
			}
		}		
		else if (cmd == "sel-place") {
			switch (es.size()) {
				default:
					put("Usage: sel-place <number>");
					break;
				case 2:
					auto number = stoi(es.at(1));
					
					if (-1 <= number and number <= 16) {
						sel_colony_slot_id = number;
					}
					else {
						put("number must be in [-1,16] range");
					}
					break;
			}
		}
		else if (cmd == "build-colony") {
			switch (es.size()) {
				default:
					put("Usage: build-colony <name>");
					break;
				case 2:
					if (env.has_defender(sel)) {
						env.colonize(
							env.get_defender(env.get_terr(sel)),
							es.at(1)
						);
						//select_next_unit();
					}
					else {
						put("no unit selected");
					}
					break;
			}

		}
		else if (cmd == "move") {
			switch (es.size()) {
				default:
					put("Usage: move <dx> <dy>");
					break;
				case 3:
					if (Unit* u = get_sel_unit()) {
						env.move_board(
							stoi(es.at(1)), // dx
							stoi(es.at(2)), //dy
							*get_sel_unit()
						);					
						select_next_unit();						 
					}
					else {
						put("no selected unit");
					}
					break;
			}
		}
		else if (cmd == "attack") {
			switch (es.size()) {
				default:
					put("Usage: attack <dx> <dy>");
					break;
				case 3:
					if (env.has_defender(sel)) {
						exec(OrderAttack(
							env.get_current_nation().id,
							env.get_defender(env.get_terr(sel)).id,
							dir4vec(
								Coords(
									stoi(es.at(1)), // dx
									stoi(es.at(2))  // dy
								)
							)
						));
					}
					else {
						put("no unit selected");
					}
					break;
			}
		}
		else if (es.at(0) == "enter") {
			switch (es.size()) {
				default: {
					output.push_back("Usage: enter [x y]");
					break;
				}
				case 1: {
					mode = Mode::COLONY;
					mod++;
					break;
				}
				case 3: {
					sel[0] = stoi(es[1]);
					sel[1] = stoi(es[2]);
					mode = Mode::COLONY;
					mod++;
					break;
				}
			}
		}
		else if (es.at(0) == "exit") {
			switch (es.size()) {
				default: {
					output.push_back("Usage: exit");
					break;
				}
				case 1: {
					// when exiting city view select next unit if current unit is working
					if (auto u = get_sel_unit()) {
						if (u->is_working()) {
							select_next_unit();
						}
					}
					mode = Mode::AMERICA;
					mod++;
					break;
				}
			}
		}
		else {
			put("ERROR: no such command");

		}


	}
	
	

	
	
	void run_console(EnvGame *ee, vector<std::thread> * tt, bool *running) {
		// ee -- server

		Console con(*ee, *tt, *running);

		preload_terrain();

		sf::RenderWindow app(
			sf::VideoMode(SCREEN_W * GLOBAL_SCALE, SCREEN_H * GLOBAL_SCALE, 32),
			"cc14"
		);

		sf::View view(sf::FloatRect(0, 0, SCREEN_W, SCREEN_H));
		app.setView(view);

		auto last_env = con.env.mod - 1;
		auto last_con = con.mod - 1;

		sf::Clock clock;
		auto last_t = clock.getElapsedTime().asSeconds();
		while (app.isOpen())
		{
			if ((con.env.mod != last_env) || (con.mod != last_con) || (last_t + 0.1 > clock.getElapsedTime().asSeconds())) {
				//cout << "RENDER:" << con.mod << ',' << env.mod << endl;
				con.time = clock.getElapsedTime().asSeconds();
				last_t = con.time;

				render(app, con.env, con);

				last_env = con.env.mod;
				last_con = con.mod;
			}

			con.handle_events(app);

		}
		(*running) = false;
		std::cout << "run_console -- clean exit\n";
	}


	
}