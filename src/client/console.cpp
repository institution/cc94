#include "console.hpp"

#include "serialize.hpp"
#include "props.hpp"
#include "renderer.hpp"
#include "null_ai.hpp"
#include "simple_ai.hpp"
#include "../front/front.hpp"
#include "serialize2.hpp"
#include "serialize3.hpp"
#include "timer.hpp"
//#include "widget.hpp"

namespace col {
	using boost::str;

	void clear_env(Env & env)
	{
		// buildtypes and unittypes
		// TODO: init to zeroes
		(*env.bts).resize(100);  
		(*env.uts).resize(100);

		// map
		env.resize({15,12});
		env.fill(Terr{AltSea, BiomeDesert});			
	}



	void read_rc(RCParser & p, Console & con)
	{	
		while (p.ok())
		{
			auto s = p.read_symbol();	
			if (s == "gui") {
				read_layout(p, ly);
			}
			else if (s == "defs") {
				read_defs(p, con.env);
			}			
			else if (s == "load") {
				read_file(p.read_string(), *con.runner);
			}
			else {
				p.set_error("unrecognized command: %||", s);
				break;
			}
		}
		p.end();		
	}


	uint8_t flag_id4color_name(const string &s) {
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

	/*void Console::set_root(IWidget * w) {
		if (root) {
			delete root;
		}
		root = w;
	}*/


	Event Console::SDLEvent_to_event(SDL_Event const& o, Front const& win)
	{
		Event p;

		keyboard.refresh();
		mouse.refresh();

		Mod kmod = keyboard.get_mod();
		Mod mmod = mouse.get_mod();
		Mod mod = kmod | mmod;

		switch(o.type)
		{
			case SDL_KEYDOWN:
				p.set_type(EventPress);
				p.set_key(SDLKeycode_to_key(o.key.keysym.sym));
				p.set_mod(mmod | SDLKeymod_to_mod(o.key.keysym.mod));
				break;

			case SDL_KEYUP:
				p.set_type(EventRelease);
				p.set_key(SDLKeycode_to_key(o.key.keysym.sym));
				p.set_mod(mmod | SDLKeymod_to_mod(o.key.keysym.mod));
				break;

			case SDL_TEXTINPUT:
				p.set_type(EventChar);
				p.set_chr(o.text.text[0]);
				break;

			case SDL_MOUSEMOTION:
				p.set_type(EventMotion);
				p.set_mod(mod);
				p.set_pos(v2s(
					get_logical_pos(win, {o.motion.x, o.motion.y})
				));
				break;

			case SDL_MOUSEBUTTONDOWN:
				p.set_type(EventPress);
				p.set_mod(mod);
				p.set_pos(v2s(
					get_logical_pos(win, {o.motion.x, o.motion.y})
				));
				p.set_key(SDLButton_to_key(o.button.button));
				break;

			case SDL_MOUSEBUTTONUP:
				p.set_type(EventRelease);
				p.set_mod(mod);
				p.set_pos(v2s(
					get_logical_pos(win, {o.motion.x, o.motion.y})
				));
				p.set_key(SDLButton_to_key(o.button.button));
				break;
		}

		return p;
	}


	void Console::handle_window_event(SDL_Event const& o)
	{
		// print_window_event(o);

		switch (o.window.event) {
			case SDL_WINDOWEVENT_SIZE_CHANGED:
			case SDL_WINDOWEVENT_RESIZED: {
				auto new_dim = v2s(o.window.data1, o.window.data2);
				if (win.win_dim != new_dim)
				{
					win.resize_view(new_dim, win.ctx_dim);
				}
				break;
			}
		}
	}


	void Console::handle_event(Event e)
	{
		//print("event = %||\n", p);
		if (e.has_type()) {

			for (auto i = widgets.size(); i-- > 0;)
			{

				auto * w = widgets.at(i);
				if (w->handle(*this, e))
				{
					//print("event handled by widget\n");
					return;
				}
			}


			reg.handle_event(e);
		}
	}

	void Console::handle_events()
	{				
		SDL_Event o;
		while (SDL_PollEvent(&o))
		{
			switch (o.type) {
				case SDL_QUIT:
					f_stop = true;
					break;
				case SDL_WINDOWEVENT:
					handle_window_event(o);
					break;
				default: {
					auto p = SDLEvent_to_event(o, win);
					handle_event(p);
					break;
				}
			}
		}
	}


	void Console::load_cargo(Item const& item, Amount const& num) {
		if (auto unit_id = get_sel_unit_id()) {
			auto & e = get_server();
			e.load_cargo(e.get<Unit>(unit_id), item, num);
		}
	}

	void Console::handle_char(char16_t code)
	{
		if (!is_active()) {
			throw Error("console not active");
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





	void Console::run() 
	{
		/*print("INFO: rendering intro screen\n");
		Texture bg = front::make_texture(front::load_png_RGBA8(conf.tile_path / "texture2.png"));
		front::bind_texture(2, bg);
		RenderCall rc;
		rc.win = &win;
		rc.mode = front::MODE_TEXTURE;
		rc.texu = 2;
		rc.dst = b2s({0,0}, bg.dim);
		rc.uv0 = aabb2f(0,0,1,1);
		rc();
		win.flip();
		*/
		
		auto main_dir = filesys::parent_path(main_rc);
		print("INFO: con.run; main_rc=\"%||\"\n", main_rc);
		print("INFO: con.run; main_dir=\"%||\"\n", main_dir);
		
		print("INFO: running \"%||\"\n", main_rc);
		clear_env(env);
		RCParser p(main_rc);
		read_rc(p, *this);
		p.close();

		print("INFO: layout racalculation\n");		
		ly.recalc();

		
		print("INFO: creating window; &win=%||\n", &win);
		win.create("ReCol", ly.scr.dim);
		win.clear();
		win.flip();

		print("INFO: loading resources from: \"%||\"\n", main_dir);
		load_resources(main_dir);
		ly.font_height = font_tiny().height;

		print("INFO: launching gui\n");
		keyboard.init();
		mouse.init();
		//front::input::print_mod_table();		
		start_gui(*this);


		print("INFO: entering event loop\n");
		Timer rt(33);
		while (not f_stop)
		{
			if (rt.left() <= 0)
			{
				render(env, *this, verbose);
				rt.wait();
			}

			sync();
			SDL_WaitEventTimeout(nullptr, 33);		
			handle_events();
		}

		print("INFO: destroy window; &win=%||\n", &win);
		win.destroy();
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
			if (env.get_prod(f.get_terr(), false, lst.at(i))) {
				return lst.at(i);
			}
			++i;
		}

		i = 0;
		while (i < curr_pos) {
			if (env.get_prod(f.get_terr(), false, lst.at(i))) {
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

	template <class AI>
	void Console::add_ai(Control control) {
		if (not runner) {
			fail("ERROR: cannot add AI: no runner\n");
		}
		else {
			runner->add_agent<AI>(env, control);
		}
	}


	/*MainFrame & Console::get_main_frame()
	{
		return (MainFrame)widgets.at(0);
	}*/

	void Console::do_command(string const& line) {

		auto & env = get_server();
		auto & r_env = get_server();

		vector<string> es;
		split(es, line, is_any_of(" "));

		Unit::Id sel_unit_id = get_sel_unit_id();
		Terr::Id sel = get_sel_terr_id();

		auto& cmd = es[0];

		// put(line);
		auto& con = *this;

		if (cmd == "list-nations") {
			for (auto & item: env.factions) {
				put("%|| %||", item.second.get_name(), int(item.second.id));
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
			put("reset");
			put("list-nations");
			put("list-units");
			put("list-unit-types");
			put("create-faction");
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
			put("attach-ai");
			//put("exec-ai");
			//put("list-ais");

			// gui
			put("enter");
			put("exit");

			// misc
			put("score");
			put("cls");
			put("print");
			put("init-demo");

		}
		else if (cmd == "list-agents")
		{
			if (runner)
			{
				for (auto * a: runner->agents)
				{
					put("%||", a->get_name());
				}
			}
		}
		else if (cmd == "reset") {
			switch (es.size()) {
				default:
					put("Usage: reset <width> <height>");
					break;
				case 3:
					env.reset(v2s(stoi(es.at(1)), stoi(es.at(2))));
					break;
			}
		}
		else if (cmd == "info") {
			switch (es.size()) {
				default:
					put("Usage: info");
					break;
				case 1:
					put("env.current_control %||", int(env.get_current_control()));
					put("env.in_progress %||", int(env.in_progress()));
					break;
			}
		}
		else if (cmd == "play") {

			switch (es.size()) {
				default:
					put("Usage: play");
					break;
				case 1:
					env.set_state(col::EnvStatePlay);
					break;
			}
		}
		else if (cmd == "init-demo") {
			switch (es.size()) {
				default:
					put("Usage: init-demo");
					break;
				case 1:
					init_demo();
					break;
			}
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


		else if (cmd == "attach-ai") {
			switch (es.size()) {
				default:
					put("Usage: attach-ai <nation_id>");
					break;
				case 2:
					add_ai<simple_ai::SimpleAi>(stoi(es.at(1)));
					put("Simple AI attached");
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
					add_ai<null_ai::NullAi>(nation_id);
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
			for (auto & ut: *env.uts) {
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
					auto & c = env.get_unittype(std::stoi(es.at(1))); // type_id
					auto cc = std::stoi(es.at(2));  // control
					auto & t = *get_sel_terr();

					auto& u = env.create<Unit>(c, cc);
					env.put(t, u);

					if (compatible(u.get_travel(), TravelLand) and
						!compatible(u.get_travel(), t.get_travel()))
					{
						//u.transported = true;
					}
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
		else if (cmd == "save") {
			switch (es.size()) {
				default:
					put("Usage: save <filename>");
					break;
				case 2: {
						if (runner) {
							write_file(es.at(1), *runner);
						}
						else {
							print("WARNING: no runner to save game\n");
						}
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
						if (runner) {
							read_file(es.at(1), *runner);
						}
						else {
							print("WARNING: no runner to load game\n");
						}
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
					env.get_store(terr).add(item, number);
					break;
			}
		}
		else if (cmd == "construct") {
			switch (es.size()) {
				default:
					put("Usage: construct <place-number> <building-id>");
					break;
				/*case 3: {
					if (auto tp = get_sel_terr()) {
						Build::Id place_id = stoi(es.at(1));
						BuildType::Id buildtype_id = stoi(es.at(2));

						env.apply(inter::construct_build(env.get_id(*tp), place_id, buildtype_id));
					}
					break;
				}*/
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
						env.get_unittype(id)
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
		else if (es.at(0) == "") {

		}
		else {
			put("No such command: %||", cmd);

		}


	}


	Time Console::get_move_cost(v2c p1, v2c p0, Unit const& u) const {
		auto const& t0 = env.get_terr(p0);
		auto const& t1 = env.get_terr(p1);

		if (is_discovered(t1) and is_discovered(t0)) {
			return env.get_move_cost(t1, t0, u);
		}
		else {
			return TimeInf;
		}
	}



	PathRes Console::find_path(v2c src, v2c trg, UnitExt const& ue)
	{
		auto const& u = get_unit(ue);

		return col::find_path(env, src, trg, [this,&u](v2c w1, v2c w0) {
			auto c = get_move_cost(w1, w0, u);
			if (c == TimeInf) {
				return std::numeric_limits<float>::infinity();
			}
			else {
				return c + rm.random_float(0.0f, 0.01f);
			}
		});
	}



	/*
		Esc -- cancel current orders
		Enter -- execute current orders




	*/



	void Console::init_demo()
	{
		if (env.factions.size() == 0)
		{
			new (&env.factions[ControlEnglandCol])
				Faction(ControlEnglandCol, "England (You)");

			set_control(ControlEnglandCol);

			new (&env.factions[ControlFranceCol])
				Faction(ControlFranceCol, "France (SimpleAI)");

			add_ai<simple_ai::SimpleAi>(ControlFranceCol);
		}
		else {
			print("Game already initialized\n");
		}

	}






	/// Make 'unit do current command.
	/// return true when unit is ready to execute next command
	/// return false when current command wasn't finished yet
	bool Console::exec_unit_cmd(UnitExt & ext)
	{
		print("exec_unit_cmd: id=%||, cmd=%||\n", ext.id, ext.get_curr_cmd());

		Unit & unit = env.get_unit(ext.id);

		// assert(ext.cmds.has());

		auto cmd = ext.cmds.get();

		switch (cmd.ins) {
			case InsNone: {
				ext.pop_cmd();
				ext.can_cont = 1;
				return 1;
			}
			case InsWait: {
				ext.can_cont = false;
				return 0;
			}
			case InsMove: {
				auto dir = (Dir)cmd.arg;
				auto r = env.move_unit(unit, dir);
				handle_error();
				if (r) {
					ext.pop_cmd();
				}
				ext.can_cont = bool(r);
				return r;
			}
			case InsAttack: {
				auto dir = (Dir)cmd.arg;
				// auto r = env.move_unit(dir, unit);
				auto r = 1;  // TODO ^
				handle_error();
				if (r) {
					ext.pop_cmd();
				}
				ext.can_cont = bool(r);
				return r;
			}
			case InsBuild: {
				auto r = env.colonize(unit);
				handle_error();
				if (r) { ext.pop_cmd(); }
				ext.can_cont = bool(r);
				return r;
			}
			case InsClear: {
				auto r = env.destroy(unit, PhysForest);
				handle_error();
				if (r) { ext.pop_cmd(); }
				ext.can_cont = bool(r);
				return r;
			}
			case InsRoad: {
				auto r = env.improve(unit, PhysRoad);
				handle_error();
				if (r) { ext.pop_cmd(); }
				ext.can_cont = bool(r);
				return r;
			}
			case InsPlow: {
				auto r = env.improve(unit, PhysPlow);
				handle_error();
				if (r) { ext.pop_cmd(); }
				ext.can_cont = bool(r);
				return r;
			}
			case InsWork: {
				ext.can_cont = 0;
				return 0;
			}
			default: {
				print(std::cerr, "WARNING: unknown unit command code: %||\n", cmd.ins);
				ext.pop_cmd();
				return 1;
			}
		}

	}



}
