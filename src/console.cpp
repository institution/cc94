#include "console.h"


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "serialize.hpp"
#include "view_base.h"

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



	
	void Console::handle(sf::RenderWindow const& app, sf::Event const& event) {
		auto type = event.type;


		
		if (type == sf::Event::KeyPressed) {
			//cerr << "key released!" << event.key.code << endl;
			
			auto code = event.key.code;
			
			if (handle_event(Dev::Keyboard, v2i(0,0), Event::Press, Button::None, code)) {
				modified();
			}
			
			if (event.key.code == sf::Keyboard::Tilde or event.key.code == sf::Keyboard::Unknown) {

			}
		}
		else
		if (type == sf::Event::TextEntered) {

			if (handle_event(Dev::Keyboard, v2i(0,0), Event::Char, Button::None, Key::Unknown, event.text.unicode)) {
				modified();
			}
			else {			
				if (is_active()) {
					handle_char(event.text.unicode);
				}
			}
			
		}
		else			
		if (type == sf::Event::MouseMoved)			
		{			
			sf::Vector2f mp = app.mapPixelToCoords(
				sf::Vector2i(
					event.mouseMove.x,
					event.mouseMove.y
				)
			);
			
			if (handle_event(Dev::Mouse, v2i(mp.x, mp.y), Event::Hover, Button::None, Key::Unknown)) {
				modified();
			}
				
		}		
		else if (type == sf::Event::MouseButtonPressed)
		{			
			sf::Vector2f mp = app.mapPixelToCoords(
				sf::Vector2i(
					event.mouseButton.x,
					event.mouseButton.y
				)
			);
			
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if (handle_event(Dev::Mouse, v2i(mp.x, mp.y), Event::Press, Button::Left, Key::Unknown)) {
					modified();
				}
			}
			
			if (event.mouseButton.button == sf::Mouse::Right)
			{
				if (handle_event(Dev::Mouse, v2i(mp.x, mp.y), Event::Press, Button::Right, Key::Unknown)) {
					modified();
				}				
			}
			
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
		//cerr << "--" << code << endl;

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

		auto& cmd = es[0];

		put(line);
		auto& con = *this;
		auto& env = envgame;
		
		if (cmd == "list-players") {
			for (auto &item: envgame.players) {
				put(str(format("%||") % item.second));
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
			put("list-players");
			put("list-units");
			put("list-unit-types");
			put("list-orders");
			put("create-player");
			put("create-unit");
			put("create-colony");
			put("delete-colony");
			put("set-biome");
			put("add-phys");
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
					if (envgame.in_bounds(sel)) {
						Phys p = get_phys_by_name(es.at(1));
						if (p != PhysNone) {
							envgame.get_terr(sel).add(p);
						}
						else {
							put("invalid phys name");
						}
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
					put("Turn: " + to_string(envgame.get_turn_no()) + "; " +
						"Player: " + envgame.get_current_player().name  + "; " +
						"State: " + to_string(envgame.state)
					);
					break;
			}
		}
		else if (cmd == "score") {
			switch (es.size()) {
				default:
					put("Usage: score <player_id>");
					break;
				case 2:
					auto f = envgame.get_result(stoi(es.at(1)));
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
					if (envgame.in_bounds(sel)) {
						Biome b = get_biome_by_name(es.at(1));
						if (b != BiomeNone) {
							envgame.get_terr(sel).set_biome(b);
						}
						else {
							put("invalid biome name");
						}
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
					if (envgame.in_bounds(sel)) {
						envgame.get_terr(sel).set_alt(stoi(es.at(1)));						
					}
					break;
			}
		}
		else if (cmd == "turn") {
			envgame.turn();
			put(str(format("Turn %||") % envgame.turn_no));
		}
		else if (cmd == "ai") {				
			// calculate and show preffered move for current player
			switch (es.size()) {
				default: {
					output.push_back("Usage: ai <num> <exec>\n");
					break;
				}
				case 3: {
					if (envgame.in_progress()) {
						auto const& pid = envgame.get_current_player().id;
						auto it = ais.find(pid);
						if (it == ais.end()) {
							put("creating ai for this player");
							create_ai(pid);
						}

						auto run_mode = stoi(es.at(2));  // 0 no, 1 step, 2 full turn
						auto power = stoi(es.at(1));
						
						
						auto& ai = ais.at(pid);
						
						if (run_mode == 1) {
							auto const& a = ai.calc(envgame, power, 0);
							put(string("action: ") + to_string(a));
							exec(a);
						}
						else {
							while (1) {
								auto const& a = ai.calc(envgame, power, 0);
								put(string("action: ") + to_string(a));
								exec(a);
								if (a == Ready(pid)) {
									break;
								}
							}

						}
						
						

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
					envgame.start();
					put("Game started!");
					break;
				}
			}
		}
		else if (cmd == "ready") {
			switch (es.size()) {
				default: {
					output.push_back("Usage: ready [player_id]\n");
					break;
				}
				case 2: {
					exec(Ready(std::stoi(es.at(1))));
					break;
				}
				case 1: {
					exec(Ready(envgame.get_current_player().id));
					break;
				}
			}
		}
		else if (cmd == "list-units") {
			for (auto &item: envgame.units) {
				put(str(format("%||") % item.second));
			}
		}
		else if (cmd == "list-unit-types") {
			for (auto &utp: *envgame.uts) {
				auto &ut = utp.second;
				put(str(format("%u: %s") % uint16(ut.id) % ut.name));
			}
		}
		else
		if (es[0] == "deli") {
			envgame.destroy<Unit>(std::stoi(es[1]));
		}
		else if (cmd == "create-unit") {
			switch (es.size()) {
				default: {
					put("Usage: create-unit <type_id> <player_id>");
					break;
				}
				case 3: {
					auto& c = env.get<UnitType>(std::stoi(es.at(1))); // type_id
					auto& p = env.get<Player>(std::stoi(es.at(2)));  // player_id
					auto& t = env.get_terr(Coords(sel[0], sel[1]));  // coords
					auto& u = env.create<Unit>(c, p);
					env.init(u, t);
					if (compatible(u.get_travel(), LAND) and
						!compatible(u.get_travel(), t.get_travel())) 
					{
						u.transported = true;
					}
					break;
				}
			}
		}
		else if (cmd == "create-player") {
			switch (es.size()) {
				default: {
					put("Usage: create-player <player-name> <color-name>");
					break;
				}
				case 3: {
					envgame.create<Player>(
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
					auto &t = envgame.get_terr(sel);
					if (t.has_colony()) {
						envgame.burn_colony(t);
					}
					else {
						put("there is no colony at selected location");
					}
					break;
			}
		}
		else
		if (es[0] == "sel") {
			switch (es.size()) {
				default: {
					output.push_back("Usage: sel <x> <y> OR sel <unit_id>\n");
					break;
				}
				case 3: {
					sel = Coords(std::stoi(es.at(1)), std::stoi(es.at(2)));
					break;
				}
				case 2: {
					auto unit_id = std::stoi(es.at(1));
					try {
						envgame.get<Unit>(unit_id);
						sel_unit_id = unit_id;
					}
					catch (Error const& e) {
						put(e.what());
					}					
					break;
				}
			}
		}
		else
		if (es[0] == "setturn") {
			switch (es.size()) {
				default: {
					output.push_back("Usage1: setturn num\n");
					break;
				}
				case 2: {
					envgame.turn_no = stoi(es.at(1));
					break;
				}
			}
		}
		else
		if (es[0] == "delp") {
			envgame.del_player(std::stoi(es[1]));
		}
		else if (cmd == "save") {
			switch (es.size()) {
				default:
					put("Usage: save <filename>");
					break;
				case 2: {
						ofstream f(es.at(1), std::ios::binary);
						boost::archive::text_oarchive oa(f);
						oa << envgame;
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
						oa >> envgame;
					}
					break;
			}
		}
		else
		if (es[0] == "set_owner") {
			envgame.set_owner(
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
					if (sel_unit_id) {
						auto ret = envgame.build_road(
							env.get<Unit>(con.sel_unit_id)
						);
						put(str(format("ret = %||") % ret));
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

					auto &terr = envgame.get_terr(sel);
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
					
					auto& c = envgame.get_terr(sel).get_colony();
					envgame.colony_construct(c, buildtype_id, place_id);
					
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
					auto unit_id = sel_unit_id;
					auto& unit = envgame.get<Unit>(unit_id);
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
						env.get<Unit>(sel_unit_id), 
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
					auto unit_id = sel_unit_id;
					auto& unit = envgame.get<Unit>(unit_id);
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
					auto unit_id = sel_unit_id;					
					auto& unit = envgame.get<Unit>(unit_id);
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
		
			
		else if (cmd == "work") {
			
		}
		else if (cmd == "worknext") {
			
		}
		
		else if (cmd == "build-colony") {
			switch (es.size()) {
				default:
					put("Usage: build-colony <name>");
					break;
				case 2:
					if (envgame.has_defender(sel)) {
						envgame.colonize(
							envgame.get_defender(envgame.get_terr(sel)),
							es.at(1)
						);
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
					auto& u = env.get<Unit>(sel_unit_id);
					exec(OrderMove(							
						env.get_current_player().id,
						u.id,
						dir4vec(
							Coords(
								stoi(es.at(1)), // dx
								stoi(es.at(2))  // dy
							)
						)
					));
					
					if (auto u = misc::get_next_to_move(env, env.get_current_player())) {
						sel_unit_id = u->id;
					}
					else {
						sel_unit_id = 0;
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
					if (envgame.has_defender(sel)) {
						exec(OrderAttack(
							envgame.get_current_player().id,
							envgame.get_defender(envgame.get_terr(sel)).id,
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
		else
		if (es.at(0) == "enter") {
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
		else
		if (es.at(0) == "exit") {
			switch (es.size()) {
				default: {
					output.push_back("Usage: exit");
					break;
				}
				case 1: {
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
	
}