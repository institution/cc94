#include "console.h"

#include "serialize.h"
#include "view_base.h"
#include "renderer.h"
#include "null_ai.h"
#include "backend/backend.h"

namespace col {
	using boost::str;

	Layout const ly(conf.screen_w, conf.screen_h);


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
		
		auto keys = backend::get_keyboard_state();
		
		if (keys[SDL_SCANCODE_LSHIFT] or keys[SDL_SCANCODE_RSHIFT]) {
			mod = mod | halo::ModShift;
		}
		if (keys[SDL_SCANCODE_LCTRL] or keys[SDL_SCANCODE_RCTRL]) {
			mod = mod | halo::ModCtrl;
		}
		
		auto mouse = backend::get_mouse();
		
		if (mouse[backend::ButtonLeft] or mouse[backend::ButtonRight]) 
		{
			mod = mod | halo::ModButton;
		}
		return mod;	
	}
	
	void Console::handle(backend::Back const& app, backend::Event const& event) {
		
		if (verbose) print("Console.handle: {\n");
		
		auto type = event.type;

		// prepare pattern
		Pattern p;
		
		if (type == backend::EventKeyDown) {
			p.dev = Dev::Keyboard;
			p.event = Event::Press;
			p.key = event.key.keysym.sym;
			print("key = %||(%||)\n", event.key.keysym.sym, char16_t(event.key.keysym.sym));			
		}
		else if (type == backend::EventTextInput) {
			p.dev = Dev::Keyboard;
			p.event = Event::Char;
			p.unicode = event.text.text[0];
		}
		else if (type == backend::EventMouseMotion)			
		{	
			auto mp = get_logical_pos(app, 
				{event.motion.x, event.motion.y}
			);
			
			p.dev = Dev::Mouse;
			p.event = Event::Hover;
			p.mod = get_mod();
			p.area = Box2(v2i(mp), {0,0});
		}		
		else if (type == backend::EventMouseButtonDown)
		{			
			auto mp = get_logical_pos(app, 
				{event.button.x, event.button.y}
			);
			
			p.dev = Dev::Mouse;
			p.event = Event::Press;
			p.mod = get_mod();
			p.area = Box2(v2i(mp), {0,0});
			
			
			if (event.button.button == backend::ButtonLeft) {
				p.button = Button::Left;
			}			
			else if (event.button.button == backend::ButtonRight) {
				p.button = Button::Right;
			}
		}
		else if (type == backend::EventMouseButtonUp)
		{	
			auto mp = get_logical_pos(app, 
				{event.button.x, event.button.y}
			);
			
			p.dev = Dev::Mouse;
			p.event = Event::Release;
			p.mod = get_mod();
			p.area = Box2(v2i(mp), {0,0});
			
			if (event.button.button == backend::ButtonLeft) {
				p.button = Button::Left;
			}			
			else if (event.button.button == backend::ButtonRight) {
				p.button = Button::Right;
			}
		}
		
		
		
		if (verbose) print("Console.handle: halo\n");
		
		// handle
		if (halo::handle_event(hts, p)) {
			modified();
		}
		
		if (verbose) print("Console.handle: }\n");
	}

	
	
	/*
	struct NotEnoughArgs: std::runtime_error {}
	
	string const& arg(vector<string> const& es, int n) {
		if (n >= es.size()) {
			throw NotEnoughArgs();
		}
		return es[n];
	}*/
	
	void Console::load_cargo(Item const& item, Amount const& num) {
		if (auto unit_id = get_sel_unit_id()) {
			get_server().apply_inter(
				inter::load_cargo(unit_id, item, num), auth
			);
		}
	}
	
	void Console::handle_char(char16_t code) {
		
		if (!is_active()) {
			throw Critical("console not active");
		}
		
		if (charset.find(code) == charset.end()) {
			return;
		}

		if (code == u'\b') {
			// backspace
			if (buffer.size()) {
				buffer.pop_back();
			}
		}
		else if (code == u'\r') {
			// return
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
			put(e.what());
		}
		
	}
	
	void Console::do_command(string const& line) {
		
		auto & env = get_server();
		auto & r_env = get_server();
		
		vector<string> es;
		split(es, line, is_any_of(" "));
		
		Unit::Id sel_unit_id = get_sel_unit_id();
		Terr::Id sel = get_sel_terr_id();
		
		auto& cmd = es[0];

		put(line);
		auto& con = *this;
		
		if (cmd == "list-nations") {
			for (auto &item: env.nations) {
				put("%||", item.second);
			}
		}
		else if (cmd == "print") {
			put(buffer.substr(6));
		}
		else if (cmd == "help" or cmd == "?") {
			// save/load
			put("save");
			put("load");
			
			// editor
			put("list-nations");
			put("list-units");
			put("list-unit-types");
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
			//put("create-ai");				
			//put("exec-ai");
			//put("list-ais");
			
			// gui
			put("enter");
			put("exit");
			
			// misc
			put("score");
			put("cls");
			put("print");
		}
		else if (cmd == "add-phys") {
			switch (es.size()) {
				default:
					put("Usage: add-phys <sealane|road|forest|minorriver|majorriver|plow>");
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
					put("Usage: sub-phys <sealane|road|forest|minorriver|majorriver|plow>");
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
					if (b == BiomeNone)
						put("selected unknown biome");
					
					for (auto tp: con.get_sel_terrs()) {
						tp->set_biome(b);	
					}					
					break;
			}
		}
		else if (cmd == "set-alt") {
			switch (es.size()) {
				default:
					put("Usage: set-alt <0,1,2,3,4>");
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
		else if (cmd == "null") {
			switch (es.size()) {
				default: {
					output.push_back("Usage: null <nation_id>\n");
					break;
				}
				case 2: {
					auto nation_id = std::stoi(es.at(1));
					
					runner.add_agent<null_ai::NullAi>(env, nation_id);
										
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
		else if (es[0] == "del-nation") {
			env.del_nation(std::stoi(es[1]));
		}
		else if (cmd == "save") {
			switch (es.size()) {
				default:
					put("Usage: save <filename>");
					break;
				case 2: {						
						write_file(es.at(1), env);
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
						read_file(es.at(1), env);
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
		else if (cmd == "add-item") {
			switch (es.size()) {
				default:
					put("Usage: add-item <item-name> <amount>");
					break;
				case 3:
					auto number = stoi(es.at(2));
					auto item = get_item_by_name(es.at(1));

					auto &terr = *get_sel_terr();
					env.get_store(terr).add(item, number);
					break;
			}
		}
		else if (cmd == "load-cargo") {
			switch (es.size()) {
				default:
					put("Usage: load-cargo <item-id> <amount>");
					break;
				case 3:
					if (auto unit_id = get_sel_unit_id()) {
						auto item = stoi(es.at(1));
						auto num = stoi(es.at(2));

						r_env.apply_inter(inter::load_cargo(unit_id, item, num), auth);

						break;
					}
					else {
						put("no selected unit");
					}
			}
		}
		else if (cmd == "construct") {			
			switch (es.size()) {
				default:
					put("Usage: construct <place-number> <building-id>");
					break;
				case 3: {
					if (auto tp = get_sel_terr()) {
						Build::Id place_id = stoi(es.at(1));
						BuildType::Id buildtype_id = stoi(es.at(2));					
						env.apply(inter::construct(env.get_id(*tp), place_id, buildtype_id));
					}
					break;
				}
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
					if (auto unit_id = get_sel_unit_id()) {
						auto& unit = env.get<Unit>(unit_id);
						env.work_field(number, unit);
					}
					else {
						print("WARNING: no selected unit on work-field command\n");
					}
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
					if (auto unit_id = get_sel_unit_id()) {
						auto& unit = env.get<Unit>(unit_id);
						env.work_none(unit);
					}
					else {
						print("WARNING: no selected unit on work-none command\n");
					}					
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
						env.apply_inter(
							inter::move_unit(
								stoi(es.at(1)), // dx
								stoi(es.at(2)), //dy
								env.get_id(*u)  // unit_id							
							),
							auth
						);
						
						select_next_unit();						 
					}
					else {
						put("no selected unit");
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
		else if (es.at(0) == "move-all") {
			switch (es.size()) {
				default: {
					output.push_back("Usage: move-all");
					break;
				}
				case 1: {
					repeat_all();
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
		else if (es.at(0) == "get-state") {
			switch (es.size()) {
				default: {
					output.push_back("Usage: get-state <nation-id>");
					break;
				}
				case 1: {
					//env.get_state(stoi(es[1]))
					
					break;
				}
			}
		}
		else {
			put("ERROR: no such command");

		}


	}
	
	
	


	
}
