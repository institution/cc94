#pragma once

#include <deque>
#include <functional>
#include <chrono>
#include <set>

#include "col.hpp"
#include "env.hpp"
#include "layout.hpp"
#include "logic.hpp"
#include "runner.hpp"
#include "random_module.hpp"
#include "../front/front.hpp"
#include "command.hpp"
#include "exts.hpp"
#include "marks.hpp"
#include "path.hpp"
#include "event.hpp"
#include "widget.hpp"
#include "timer.hpp"

/*
 * place biome plains|tundra|grassland|...
 * place phys mountain
 * place unit eng colonist
 * clear unit
 * clear phys
 * move q|w|e|a|d|z|x|c
 *
 *
 */




namespace col {

	std::u16string const CHARSET = u" !\"#$%'()+,-./0123456789:;<=>?ABCDEFGHIJKLMNOPQRSTUWXYZ[\\]^_`vabcdefghijklmnopqrstuwxyz{|}@~\r\b";

	struct Console;
	struct MainFrame;

	


	inline v2f get_logical_pos(Front const& win, v2i p) {
		auto l = win.ctx_dim;
		auto o = win.win_dim;
		
		auto rx = float(l[0])/float(o[0]);
		auto ry = float(l[1])/float(o[1]);
		
		auto ret = v2f(
			float(p[0]) * rx,
			float(p[1]) * ry
		);
		
		return ret;
	}


	struct Console;
	
	struct Widget_0
	{
		virtual void render(Front & win, Console & con) {
			print(std::cerr, "default widget render");
		}
	};
	
	struct IWidget;



	struct UnitExt {
		Unit::Id id;
		
		Cmds cmds;
		
		// true means unit is doing nothing productive right now
		bool can_cont{true};
		
		void pop_cmd() { cmds.pop(); }			
		bool has_cmd() const { return cmds.has();}
		void clear_cmds() { cmds.clear(); }			
		Cmd get_curr_cmd() const { return cmds.get(); }										
		void add_cmd(Cmd cmd) { cmds.add(cmd); }			
		void set_cmd(Cmd cmd) { cmds.set(cmd); }
		
		void add_cmds(Cmds const& other) { cmds.adds(other); }
		
		Cmds const& get_cmds() const {
			return cmds;
		}
								
		UnitExt() = default;
		explicit UnitExt(Unit::Id id): id(id) {}
		UnitExt(UnitExt const&) = delete;
	};
	
	using UnitExts = Exts<Unit, UnitExt>;
	
	
	Class const ClassConsole = 101;


	/// provides:
	/// state for gui rendering
	/// state for cmd interface
	/// map editing mode
	/// human AI heplers (path finding, command queeing, extended info for game objects, has_vision, is_discovered)
	/// -simple autonomous AI	

	struct Console : Agent {
		vector<string> output;
		string buffer;
		std::deque<string> history;
		std::deque<string>::iterator chi;

		Env & env;

		Keyboard keyboard;
		Mouse mouse;

		uint32_t mod;

		Auth auth{0};

		long time{0};  // miliseconds

		bool editing{true};
		
		v2s old_dim{0,0};
		
		int equip_to_unit_id{0};
		int equip_to_unit_type_id{0};

		Workplace *prod_to_workplace{nullptr};

		int select_unit_popup{0};

		// selected area (affects edit mode)   TODO:: set
		std::set<Terr*> sel_terrs;

		// selected square
		//Terr * sel_terr{nullptr};

		// selected unit
		Unit * sel_unit{nullptr};

		Runner * runner{nullptr};
			
		template <class AI>
		void add_ai(Control cc);
		
		
		Marks marks;
		
		//int8_t show_unit_movement{1};		
		//stepmode_exec_sel_unit_cmds
		
		unordered_set<char16_t> charset;

		
		// currently selected popup entry
		int selected_id{0};
		
		using InputMode = int8_t;
		InputMode
			InputModeDefault = 0,
			InputModeMove = 1,
			InputModeTerr = 2,
			InputModeUnit = 3;
		
		InputMode input_mode{InputModeDefault};
		
		InputMode get_input_mode() const {
			return input_mode;
		}
		
		void set_input_mode(InputMode input_mode) {
			this->input_mode = input_mode;			
		}
		
		
		Dir part_dir{DirS};
		
		
		RandomModule rm;

			
		// last modification time
		front::Tick last_mod_env;
		front::Tick last_mod_con;
		
		front::Tick last_tick;
		
		//front::Front win;
		
		int verbose{0};
		
		void init();
		
		
		
		void init_GUI();
		bool step_GUI();
		
		string get_name() const override {
			return "GUI `Console`";
		}

		bool f_stop{false};
		
		void run() override;
		
		
		void handle_events();

		
	
		Class get_class() const override { return ClassConsole; }
		
		
		//Build *selprod_build{nullptr};
		//Makeable const* selprod_makeable{nullptr};
		

		
		Rules reg;
		

		// active screen
		enum class Mode{
			AMERICA, COLONY, EUROPE, REPORT
		};

		//Widget * widget{nullptr};
		//Widget * widget_next{nullptr};
		
		
		void handle_event(Event e);

		
		//std::vector<std::function<void(Console&)>> grs;
		
		
		// gui render stack
		std::vector<IWidget*> widgets;
		
		template <class T, class ...Args>
		T & push_widget(Args&&... args) {
			auto * w = new T(std::forward<Args>(args)...);
			widgets.push_back(w);
			return *w;
		}
		
		void pop_widget() {
			if (widgets.size()) {				
				IWidget * w = widgets.at(widgets.size() - 1);
				widgets.pop_back();
				delete w;
			}
		}
		

		void init_demo();
		
		

		// is console active - keyboard focus
		int active{false};

		int sel_colony_slot_id{-1};

		Mode mode;
		
		// my control code
		Control mcc{ControlNone};
		
		void set_control(Control cc) {
			mcc = cc;
		}
		
		
		string memtag;
		
		Env *server{nullptr};   // wtf?


		

		Console(Env & env, Runner * runner = nullptr):
			env(env), server(&env), runner(runner), verbose(0)
		{
			for (auto c: CHARSET) {
				charset.insert(c);
			}

			chi = history.begin();
			mod = 0;
			mode = Mode::AMERICA;
			clear();

			if (runner) {
				init_GUI();
			}
		}


		

		RGBA8 get_color() const 
		{
			switch (mcc) {
				case ControlEnglandCol: return {255,0,0,255};
				case ControlFranceCol: return {0,0,255,255};
				case ControlSpainCol: return {255,0,255,255};
				case ControlNetherlandsCol: return {255,255,0,255};
			}
			return {255,0,0,255};			
		}
		
		
		

		bool has_control(Unit const& unit) const {
			return editing or env.has_control(mcc, unit);
		}


		
		
		MainFrame & get_main_frame();
		
		bool is_selected(Terr * terr) {
			if (terr) {
				return sel_terrs.count(terr);
			}
			else {
				return true;  // nullptr == empty set -- always selected
			}
		}

		bool is_selected(Terr & terr) { return is_selected(&terr); }

		bool is_selected(Coords const& c) {
			return is_selected( env.in_bounds(c) ? (&env.get_terr(c)) : nullptr );
		}

		UnitExts unitexts;
		
		UnitExt & get_unit_ext(Unit const& u) { return unitexts.get(u); }		
		UnitExt & get_unit_ext(Unit::Id id) { return unitexts.get(id); }

		
		

		template <class F>
		Unit * find_unit(Unit * cur, F f) {
			if (cur and f(*cur)) {
				return cur;
			}
			for (auto & u: list_values(env.units)) {
				if (f(u)) {
					return &u;
				}
			}
			return nullptr;
		}
		
		template <class F>
		UnitExt * find_unitext(UnitExt * cur, F f) {
			if (cur and f(*cur)) {
				return cur;
			}
			for (auto & u: list_values(unitexts)) {
				if (f(u)) {
					return &u;
				}
			}
			return nullptr;
		}
		
		Unit * get_next_to_order(Unit * cur) {
			return find_unit(cur, [this](Unit & u) {
				return 
					has_control(u) and 
					u.get_time_left() and 
					not u.is_working() and 
					not get_unit_ext(u).has_cmd();
			});
		}

		
		
		
		
		Unit * get_next_to_repeat(Unit * cur) 
		{
			return find_unit(cur, [this](Unit & u) {
				return 
					has_control(u) and 
					u.get_time_left() and 
					get_unit_ext(u).has_cmd() and
					get_unit_ext(u).can_cont;					
			});
		}

		Unit * get_next_to_repeat() {
			return get_next_to_repeat(get_sel_unit());
		}
		
		Terr * get_idle_colony(Control control) {
			for (auto & p: env.get_cont<Colony>()) 
			{				
				auto & c = p.second;
				auto & terr = c.get_terr();
				
				if (env.get_control(terr) == control) 
				{
					for (auto & b: c.builds) {
											
						if (env.can_construct(b) and !b.task and b.has_units()) 
						{
							return &terr;
						}
					}
				}
			}
			return nullptr;
		}
		
		// view box
		v2s view_pos{0,0};		
		v2s view_dim{15,12};
		
		b2c get_view_box() const { return b2c(view_pos, view_dim); }
		v2c get_view_pos() const { return view_pos; }		
		v2c get_view_dim() const { return view_dim; }

		
		void limit_view() {
			auto max_pos = env.get_dim() - view_dim;
			auto min_pos = v2s{0,0};
			
			for (int i=0; i<2; ++i) {
				if (view_pos[i] > max_pos[i]) {
					view_pos[i] = max_pos[i];
				}
				
				if (view_pos[i] < min_pos[i]) {
					view_pos[i] = min_pos[i];
				}				
			}
		}
		
		void move_view(v2s delta) {			
			view_pos += delta;			
			limit_view();			
		}
		
		void center_view_on(v2c pos) {			
			view_pos = pos - v2c(view_dim[0]/2, view_dim[1]/2);			
			limit_view();			
		}
		
		
		void center_on(Unit const* u) {
			if (u) {
				auto unit_pos = env.get_coords(*u);
				auto view_box = get_view_box();
				//print("view_box = %||; unit_pos = %||\n", view_box, unit_pos);
				if (not overlap(view_box, unit_pos)) 
				{
					center_view_on(unit_pos);
				}
				else {
					//print("no overlap\n");
				}
			}
		}
		
		
		
		
		
		
		
		void select_next_unit() {
			select_unit(
				get_next_to_order(get_sel_unit())
			);
			
			center_on(get_sel_unit());			
		}
		
		void set_biome(Biome biome) {
			for (auto tp: get_sel_terrs()) {
				tp->set_biome(biome);
			}
		}
		
		void set_alt(Alt alt) {
			for (auto tp: get_sel_terrs()) {
				tp->set_alt(alt);
			}
		}
		
		Alt get_alt() {
			if (auto t = get_sel_terr()) {
				return t->get_alt();
			}
			return AltNone;
		}
		
		void set_phys(Phys mask, Phys value) {
			for (auto tp: get_sel_terrs()) {
				tp->set_phys(mask, value);
			}
		}
		
		void switch_phys(Phys mask) {
			for (auto tp: get_sel_terrs()) {
				tp->switch_phys(mask);
			}
		}


		




		void clear_unit_cmds(Unit::Id unit_id) {
			get_unit_ext(unit_id).clear_cmds();
		}

		void select_unit(Unit *unit) {
			sel_unit = unit;
			set_input_mode(InputModeDefault);
		}

		void unselect_unit() {
			select_unit(nullptr);
		}

		void select_unit(Unit &unit) {
			select_unit(&unit);
		}

		void select_unit(Unit::Id id) {
			select_unit( id ? (&env.get<Unit>(id)) : nullptr );
		}





		void select_terr(Terr * terr = nullptr, int8 v = 0) {
			// v -- mode: sub,set,add (-1,0,+1)
			sel_unit = nullptr;
			switch (v) {
				case -1:
					if (terr) {
						sel_terrs.erase(terr);
					}
					break;
				case 0:
					sel_terrs.clear();
					if (terr) {
						sel_terrs.insert(terr);
					}
					break;
				case 1:
					if (terr) {
						sel_terrs.insert(terr);
					}
					break;
				default:
					assert(0);
					break;
			}
		}

		void unselect_terr() {
			select_terr();
		}

		void select_terr(Terr & terr) {
			select_terr(&terr);
		}

		void select_terr(Terr::Id const& c, int8 v = 0) {
			select_terr(
				(env.in_bounds(c) ? (&env.get_terr(c)) : nullptr),
				v
			);
		}


		Control get_control() const {
			return mcc;
		}
		

		bool has_vision(Terr const& terr) const {
			return editing or terr.get_vision(mcc);
		}
		
		bool is_discovered(Terr const& terr) const {
			return editing or terr.get_discovered(mcc);
		}

		Time get_move_cost(v2c p1, v2c p0, Unit const& u) const;


		char get_letter(Unit const& u) {
			auto & ue = get_unit_ext(u);
			auto cmd = ue.cmds.get();
			switch (cmd.ins) {
				case InsNone: return '-';
				case InsWait: return 'W';
				case InsMove: return 'G';
				case InsAttack: return 'G';
				case InsBuild: return 'B';
				case InsClear: return 'O';
				case InsRoad: return 'R';
				case InsPlow: return 'P';
				case InsWork: return 'w';
				default: return '?';						
			}
		}

		void enter(Coords c) 
		{
			select_terr(c);
			mode = Mode::COLONY;
			mod++;
		}
		
		void exit() 
		{
			mode = Mode::AMERICA;
			mod++;
		}
		
		bool has_sel_unit() const {
			return sel_unit != nullptr;
		}

		Unit* get_sel_unit() const {
			return sel_unit;
		}

		Terr* get_sel_terr() const {
			if (sel_unit) {
				return &env.get_terr(*sel_unit);
			}
			if (sel_terrs.size()) {
				return *(sel_terrs.begin());
			}
			else {
				return nullptr;
			}
		}

		auto get_sel_terrs() const -> decltype(sel_terrs) const& {
			return sel_terrs;
		}


		Unit::Id get_sel_unit_id() const {
			if (sel_unit) {
				return sel_unit->id;
			}
			return 0;
		}

		Terr::Id get_sel_terr_id() const {
			if (auto tp = get_sel_terr()) {
				return env.get_coords(*tp);
			}
			return Terr::Id(-1,-1);
		}

		
		void click_colony_field(Terr const& terr) {
			cout << "click_colony_field: " << env.get_coords(terr) << endl;
		}

		//void click_colony_build(Colony const& col) {
		//	cout << "click_colony_build: " << env.get_coords(terr) << endl;
		//}

		void click_map_tile(Terr const& terr) {
			cout << "click_colony_field: " << env.get_coords(terr) << endl;
		}

		
		v2c get_coords(Unit const& u) const {
			return env.get_coords(u);
		}

		v2c get_coords(UnitExt const& u) const {
			return env.get_coords(env.get_unit(u.id));
		}



		
		
		
		
		Unit const& get_unit(UnitExt const& ext) const {
			return env.get_unit(ext.id);
		}
		
		Unit & get_unit(UnitExt & ext) {
			return env.get_unit(ext.id);
		}
		
		
		
		PathRes find_path(v2c src, v2c trg, UnitExt const& u);
		
		PathRes find_path(v2c src, v2c trg, Unit const& u) {
			return find_path(src, trg, get_unit_ext(u));
		}
		
		
		
		void show_error(string const& msg) {
			// TODO
			print("msg: %||\n", msg);
		}
		
		void show_error() {
			show_error(env.read_error());
		}
		
		void handle_error() {
			if (env.has_error()) {
				show_error(env.read_error());
			}
		}
		
		
		

		
		
		/// Make 'unit do current command.
		/// return true when unit is ready to execute next command
		/// return false when current command wasn't finished yet
		bool exec_unit_cmd(UnitExt & ext);
		
		
		
		UnitExt * get_sel_unit_ext() 
		{
			if (auto * u = get_sel_unit()) {
				return &get_unit_ext(u->id);
			}
			else {
				return nullptr;
			}			
		}
		


		void exec_unit_cmds(UnitExt & ext) 
		{
			while (ext.has_cmd() and exec_unit_cmd(ext)) {
				// sleep ?
				// show ?
				// center on unit?
			}	
		}
		
		/// Move all units with active command
		void repeat_all() {
			// any user input except hover -> break
			//on([&stop](){ stop = true; })

			while (auto * up = get_next_to_repeat()) {
				auto & unit = *up;
				
				auto & ext = get_unit_ext(unit);
				
				exec_unit_cmds(ext);
			
			}
			
			select_next_unit();
		}
		
		
		


		Cmds const& get_unit_cmds(Unit const& unit) {
			return get_unit_ext(unit.id).get_cmds();
		}

		
		// R,P,O,B - road,plow,clear-forest,build-colony
		// 1,2,3,4,6,7,8,9 - move dir

		void cmd_unit(Unit::Id unit_id, Cmd cmd) 
		{
			auto & unit_ext = get_unit_ext(unit_id);
			unit_ext.set_cmd(cmd);			
			exec_unit_cmd(unit_ext);
			select_next_unit();
		}

		void cmd_unit(Unit::Id unit_id, Cmds cmds) 
		{
			auto & unit_ext = get_unit_ext(unit_id);
			
			unit_ext.add_cmds(cmds);
			
			
			exec_unit_cmd(unit_ext);
			select_next_unit();
		}





		TurnNo last_sync{-1};

		void sync() {
			auto ct = env.get_turn_no();
			if (ct != last_sync) {
				print("con.SYNCING\n");
				for (auto & ue: list_values(unitexts)) {
					ue.can_cont = 1;
				}
				last_sync = ct;
			}
		}


		bool redraw{1};

		
		

		void load_cargo(Item const& item, Amount const& num);


		Item drag_item{ItemNone};
		Amount drag_num{0};
		// load = +1; unload = -1;
		signed drag_dir{0};

		void drag_cargo(Item const& item, Amount const& num, signed dir) {
			assert(num >= 0);
			drag_item = item;
			drag_num = num;
			drag_dir = dir;
		}

		void drop_cargo(signed dir) {
			print("drop cargo expected=%||, actual=%||, num=%||\n", dir, drag_dir, dir * drag_num);
			if (dir == drag_dir) {
				load_cargo(drag_item, dir * drag_num);
			}
			drag_item = ItemNone;
			drag_num = 0;
			drag_dir = 0;
		}


		Item get_next_workitem_field(Env const& env, Field const& f) const;

		void trapall() {
			assert(0);
		}



		void on(Event const& p) {
			reg.on(p);
		}

		template<typename F>
		void on(Event const& p, F f) {
			reg.on(p, f);
		}



		Env & get_server() { return *server; }
		void set_server(Env & server) { this->server = &server; }



		Console() = delete;
		Console(Console const&) = delete;
		Console const& operator=(Console const&) = delete;


		

		void clear() {
			output.clear();
			buffer = "";
			unselect_unit();
			unselect_terr();
		}

		template <class... Args>
		void put(string const& pat, Args const& ... args) {
			output.push_back(format(pat, args...));
		}


		void modified() {
			++mod;
		}

		void handle_char(char16_t code);

		void handle();
		void handle_window_event(SDL_Event const& o);
		Event SDLEvent_to_event(SDL_Event const& o, Front const& win);



		void command(string const& line);
		void do_command(string const& line);



		void history_up() {
			if (chi == history.end()) {
				chi = history.begin();
			}

			if (history.size()) {
				buffer = *chi;
				++chi;

				modified();
			}
		}

		void history_down() {
			if (chi == history.begin()) {
				chi = history.end();
			}

			if (history.size()) {
				--chi;
				buffer = *chi;

				modified();
			}
		}

		void set_active(bool a) {
			active = a;
		}

		bool is_active() const {
			return active;
		}

		bool blink() const {
			return ((this->time % 1000) >= 500);
		}

	};




	

}
