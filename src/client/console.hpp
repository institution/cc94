#pragma once

#include <deque>
#include <functional>
#include <chrono>
#include <set>

#include "col.hpp"
#include "env.hpp"
#include "layout.hpp"
#include "player.hpp"
#include "logic.hpp"
#include "halo.hpp"
#include "player.hpp"
#include "runner.hpp"
#include "random_module.hpp"
#include "../front/front.hpp"


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


	
	
	using Ins = int8_t;
	Ins const
		InsNone{0},
		InsWait{1},       // Space for N turns
		InsMove{2},
		InsAttack{3},
		InsBuild{4},
		InsClear{5},
		InsRoad{6},
		InsPlow{7},
		InsWork{9};        // automatically assigned to units working inside city
		
	inline string get_ins_name(Ins ins) {
		switch (ins) {
			case InsNone: return "InsNone";
			case InsWait: return "InsWait";
			case InsMove: return "InsMove";
			case InsAttack: return "InsAttack";
			case InsBuild: return "InsBuild";
			case InsClear: return "InsClear";
			case InsRoad: return "InsRoad";
			case InsPlow: return "InsPlow";
			case InsWork: return "InsWork";
			default: 
				print(std::cerr, "WARINING: unknown ins code: %||\n", ins);
				return "InsError";			
		}	
	}
		
	using Arg = int8_t;
		
	struct Cmd{
		Ins ins;
		Arg arg;
		Cmd() = default;
		Cmd(Ins ins, Arg arg): ins(ins), arg(arg) {}
	};
	
	inline bool operator==(Cmd a, Cmd b) {
		return a.ins == b.ins and a.arg == b.arg;
	}
	
	inline Cmd make_cmd(Ins a, Arg b = 0) {
		return Cmd(a,b);
	}
	
	inline ostream & operator<<(ostream & o, Cmd const& cmd) {
		o << format("(%|| %||)", get_ins_name(cmd.ins), int(cmd.arg));
		return o;
	}
	

	struct Chain {
		vector<Cmd> cmds;
		
		size_t size() const { return cmds.size(); }
		Cmd const& at(size_t i) const { return cmds.at(i); }
		Cmd & at(size_t i) { return cmds.at(i); }
		
		
		void pop_cmd() { cmds.pop_back(); }
		bool has_cmd() const { return cmds.size() > 0;}
		void clear_cmds() { cmds.clear(); }
		
		Cmd get_curr_cmd() const { 
			if (has_cmd()) {
				return cmds.back(); 
			}
			else {
				return make_cmd(InsNone);
			}				
		}
					
		void add_cmd(Cmd cmd) {	
			cmds.push_back(make_cmd(InsNone));
			for (auto i = cmds.size(); i >= 2; --i)
			{
				cmds[i-1] = cmds[i-2];
			}				
			cmds[0] = cmd;
		}
		
		void add_cmds(Chain other) {				
			for (auto i = other.cmds.size(); i > 0; --i)
			{
				add_cmd(other.cmds[i-1]);
			}			
		}
		
		void set_cmd(Cmd cmd) {				
			clear_cmds();
			add_cmd(cmd);
		}
	};
	
	
	

	/*char order2letter(Order o) {
		switch (o) {
			case OrderNone: return '-';
			case OrderWait: return 'W';
			case OrderMove: return 'G';
			case OrderAttack: return 'A';
			case OrderBuild: return 'B';
			case OrderClear: return 'O';
			case OrderRoad: return 'R';
			case OrderPlow: return 'P';
			case OrderWork: return 'w';
			default: return '?';			
		}
	}
	
	Order letter2order(char l) {
		switch (o) {
			case '-': return OrderNone;
			case 'W': return OrderWait;
			case 'G': return OrderMove;
			case 'A': return OrderAttack;
			case 'B': return OrderBuild;
			case 'O': return OrderClear;
			case 'R': return OrderRoad;
			case 'P': return OrderPlow;
			case '=': return OrderWaitOne;
			//case 'w': return OrderWaitInf;   // no need - this order is only used internally
			default: return OrderNone;			
		}
	}*/

	// mouse events: normal, drag&drop

	/*
	struct DragDrop {
		enum class Obj {
			None, Item, Unit
		};

		Obj what;
		union Arg{
			Item * item;
			Unit * unit;
		} arg;
	}*/



	inline v2f get_logical_pos(Front const& front, v2i p) {
		auto l = front.get_logical_dim();
		auto o = front.get_physical_dim();
		
		auto rx = float(l[0])/float(o[0]);
		auto ry = float(l[1])/float(o[1]);
		
		auto ret = v2f(
			float(p[0]) * rx,
			float(p[1]) * ry
		);
		
		return ret;
	}


	struct Console;
	
	struct Widget{
		virtual void render(Front & win, Console & con) {
			print(std::cerr, "default widget render");
		}
	};


	struct Console : Agent {
		vector<string> output;
		string buffer;
		std::deque<string> history;
		std::deque<string>::iterator chi;

		Env & env;

		uint32_t mod;

		Auth auth{0};

		long time{0};  // miliseconds

		bool editing{true};
		
		
		int equip_to_unit_id{0};
		int equip_to_unit_type_id{0};

		Workplace *prod_to_workplace{nullptr};

		int select_unit_popup{0};

		// selected area (affects edit mode)   TODO:: set
		std::set<Terr*> sel_terrs;

		// selected square
		//Terr * sel_terr{nullptr};

		// selected unit
		Unit* sel_unit{nullptr};

		v2s view_pos{0,0};		
		
		b2c get_view_box() const { return b2c(view_pos, view_dim); }
		v2c get_view_pos() const { return view_pos; }		
		v2c get_view_dim() const { return view_dim; }

		Runner * runner{nullptr};
		
		
		template <class AI>
		void add_ai(Nation::Id nation_id);
		
		
		
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
			select_path();
			
			this->input_mode = input_mode;			
		}
		
		
		Dir part_dir{DirS};
		
		
		RandomModule rm;

		
		// last modification time
		front::Tick last_mod_env;
		front::Tick last_mod_con;
		
		front::Tick last_tick;
		
		front::Front win;
		
		int verbose{0};
		
		void init();
		
		
		
		void init_GUI();
		bool step_GUI();
		
		string get_name() const override {
			return "GUI `Console`";
		}
		
		bool step() override {
			sync();
			return step_GUI();
		}
		
		
	
		
		
		
		//Build *selprod_build{nullptr};
		//Makeable const* selprod_makeable{nullptr};
		

		using Event = halo::Event;
		using Button = halo::Button;
		using Key = halo::Key;
		using Pattern = halo::Pattern;
		using Dev = halo::Dev;


		halo::Patterns hts;

		// active screen
		enum class Mode{
			AMERICA, COLONY, EUROPE, REPORT
		};

		Widget * widget{nullptr};
		//Widget * widget_next{nullptr};
		
		template <class T, class ... Args>
		T & replace_widget(Args && ... args) {
			if (widget) {
				delete widget;
			}		
			T * t = new T(std::forward<Args>(args)...);	
			widget = t; 
			return *t;
		}

		void clear_widget() {
			if (widget) {
				delete widget;
			}			
			widget = nullptr;			
		}

		// call this at the begining of render function
		void sync_widget() {
			
		}
		

		// is console active - keyboard focus
		int active{false};

		int sel_colony_slot_id{-1};

		Mode mode;
		
		Nation::Id nation_id{1};
		
		
		string memtag;
		
		Env *server{nullptr};   // wtf?


		/// map view position
		v2s view_dim{15,12};


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

		
		
		

		void set_nation_id(Nation::Id id) {
			nation_id = id;
		}
		
		bool has_control(Unit const& unit) const {
			return editing or env.has_control(env.get_nation(nation_id), unit);
		}


		
		
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

		void apply_inter(inter::Any const& a, Agent & s);

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

		struct UnitExt;

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
					not get_unitext(u.id).has_cmd();
			});
		}

		
		
		
		
		Unit * get_next_to_repeat(Unit * cur) 
		{
			return find_unit(cur, [this](Unit & u) {
				return 
					has_control(u) and 
					u.get_time_left() and 
					get_unitext(u.id).has_cmd() and
					get_unitext(u.id).can_cont;					
			});
		}

		Unit * get_next_to_repeat() {
			return get_next_to_repeat(get_sel_unit());
		}
		
		void center_on(Unit const* u) {
			if (u) {
				auto unit_pos = env.get_coords(*u);
				auto view_box = b2c(view_pos, view_dim);
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


		inter::Any get_inter(Unit::Id unit_id, char letter, uint8 dx, uint8 dy) {
			inter::Any a;

			switch(letter) {
				case 'R':
					a = inter::improve(unit_id, PhysRoad);
					break;
				case 'P':
					a = inter::improve(unit_id, PhysPlow);
					break;
				case 'O':
					a = inter::destroy(unit_id, PhysForest);
					break;
				case 'B':
					a = inter::build_colony(unit_id);
					break;
				case 'G':
					assert(dx != 0 or dy != 0);
					a = inter::move_unit(dx, dy, unit_id);
					break;
				default:
					throw Error("unknown order code: %||", letter);
			}
			return a;
		}


		bool exec(inter::Any const& a) {
			try {
				get_server().apply_inter(a, auth);
				return 1;
			}
			catch(InvalidAction const& e) {
				put(e.what());
				print("%||\n", e.what());
				return 0;
			}
		}




		void clear_unit_cmds(Unit::Id unit_id) {
			get_unitext(unit_id).clear_cmds();
		}

		void select_unit(Unit *unit) {
			sel_unit = unit;
			select_path();
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




		bool has_vision(Terr const& terr) const {
			return this->editing or terr.get_vision(this->nation_id);
		}
		
		bool is_discovered(Terr const& terr) const {
			return this->editing or terr.get_discovered(this->nation_id);
		}

		Time get_move_cost(v2c p1, v2c p0, Unit const& u) const;


		char get_letter(Unit const& u) {
			auto & ue = get_unitext(u.id);			
			auto cmd = ue.get_curr_cmd();
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





		void reset_hotspots() {
			hts.clear();
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

		Nation & get_nation() {
			return env.get_nation(nation_id);
		}


		v2c get_coords(Unit const& u) const {
			return env.get_coords(u);
		}




		struct UnitExt {
			Unit::Id id;
			
			Chain chain;
			
			// true means unit is doing nothing productive right now
			bool can_cont{true};
			
			void pop_cmd() { chain.pop_cmd(); }			
			bool has_cmd() const { return chain.has_cmd();}
			void clear_cmds() { chain.clear_cmds(); }			
			Cmd get_curr_cmd() const { return chain.get_curr_cmd(); }										
			void add_cmd(Cmd cmd) { chain.add_cmd(cmd); }			
			void set_cmd(Cmd cmd) { chain.set_cmd(cmd); }
			
			void add_cmds(Chain other) { chain.add_cmds(other); }
			
			Chain const& get_cmds() const {
				return chain;
			}
									
			UnitExt() = default;
			UnitExt(Unit::Id id): id(id) {}
		};
		
		unordered_map<Unit::Id, UnitExt> unitexts;
		
		
		
		
		Chain path;
		
		void select_path(Chain p) {
			path = p;
		}
		void select_path() {
			path.clear_cmds();
		}
		
		Chain get_sel_path() {
			return path;
		}
		
		std::array<Dir, 8> const neighs = {DirQ,DirW,DirE,DirA,DirD,DirZ,DirX,DirC};
		
		Chain find_path(v2c src, v2c trg, Unit const& u);
	
		
		
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
		bool exec_unit_cmd(UnitExt & ext) 
		{	
			print("exec_unit_cmd: id=%||, cmd=%||\n", ext.id, ext.get_curr_cmd());
		
			Unit & unit = env.get_unit(ext.id);
			
			auto cmd = ext.get_curr_cmd();
			
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
					auto r = env.move_unit(dir, unit);
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
		
		
		UnitExt & get_unit_ext(Unit::Id id) {
			auto p = unitexts.find(id);
			if (p != unitexts.end()) {
				return p->second;
			}
			auto r = unitexts.insert({id, UnitExt(id)});
			return unitexts.at(id);			
		}
		
		
		UnitExt & get_unitext(Unit::Id id) {
			auto p = unitexts.find(id);
			if (p != unitexts.end()) {
				return p->second;
			}
			auto r = unitexts.insert({id, UnitExt(id)});
			return unitexts.at(id);
			//return r.first->second;
			//return unitexts[id];
		}
		
		/// Move all units with active command
		void repeat_all() {
			// any user input except hover -> break
			//on([&stop](){ stop = true; })

			auto & nation = get_nation();

			
			while (auto * up = get_next_to_repeat()) {
				auto & unit = *up;
				
				auto & ext = get_unitext(unit.id);
				
				while (ext.has_cmd() and exec_unit_cmd(ext)) {
					// sleep ?
					// show ?
					// center on unit?
				}	
			
			}
			
			select_next_unit();
		}
		
		
		


		Chain const& get_unit_cmds(Unit const& unit) {
			return get_unit_ext(unit.id).get_cmds();
		}

		
		// R,P,O,B - road,plow,clear-forest,build-colony
		// 1,2,3,4,6,7,8,9 - move dir

		void cmd_unit(Unit::Id unit_id, Cmd cmd) 
		{
			auto & unit_ext = get_unitext(unit_id);
			unit_ext.set_cmd(cmd);			
			exec_unit_cmd(unit_ext);
			select_next_unit();
		}

		void cmd_unit(Unit::Id unit_id, Chain cmds) 
		{
			auto & unit_ext = get_unitext(unit_id);
			
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
			hts.clear();
		}

		template<typename... T>
		void on(T&&... t) {
			hts.push_back(halo::Pattern(std::forward<T>(t)...));
		}

		// mouse button over area - string command
		void on2(Event t, Button b, v2s pos, v2s dim, std::string const& cmd) {
			on(t, b, pos, dim, [this,cmd](){
				command(cmd);
			});
		}

		// mouse button anywhere - string command
		void on2(Event t, Button b, std::string const& cmd) {
			on(t, b, [this,cmd](){
				command(cmd);
			});
		}

		// mouse hover over area - string command
		void on2(Event t, v2s pos, v2s dim, std::string const& cmd) {
			on(t, pos, dim, [this,cmd](){
				command(cmd);
			});
		}

		// mouse hover anywhere - string command
		void on2(Event t, std::string const& cmd) {
			on(t, [this,cmd](){
				command(cmd);
			});
		}





		Env & get_server() { return *server; }
		void set_server(Env & server) { this->server = &server; }



		Console() = delete;
		Console(Console const&) = delete;
		Console const& operator=(Console const&) = delete;


		void handle_events(Front & app) {
			if (verbose >= 2) print("Console.handle_events: {\n");
			
			Console &con = *this;
			front::Event ev;
			while (app.pool_event(ev)) {
				if (verbose >= 2) print("Console.handle_events: some event;\n");
				
				switch (ev.type) {
					/*case front::EventKeyDown:
						if (ev.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
							app.stop();
						}
						break;*/
					case front::EventQuit:
						if (verbose >= 1) {
							print("EventQuit\n");
						}						
						app.stop();
						break;
					case front::EventWindowEvent:
						if (verbose >= 1) {
							front::print_window_event(ev);
						}
						break;					
					default:
						break;
						
				}
					
				con.handle(app, ev);
			}
			
			if (verbose >= 2) print("Console.handle_events: }\n");
		}


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

		void handle(Front const&, front::Event const&);

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
