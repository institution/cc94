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

	


	struct Console{
		vector<string> output;
		string buffer;
		std::deque<string> history;
		std::deque<string>::iterator chi;

		logic::Memory mem;

		Env & env;

		uint32 mod;

		Auth auth{0};

		long time{0};  // miliseconds

		/*struct SelectProductionOnBuild {
			
			build_id
			selected_id
			
		};*/
		
		
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

		

		Runner & runner;
		
		
		unordered_set<char16_t> charset;

		
		// currently selected popup entry
		int selected_id{0};
		
		
		
		Build *selprod_build{nullptr};
		Makeable const* selprod_makeable{nullptr};
		

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



		// is console active - keyboard focus
		int active{false};

		int sel_colony_slot_id{-1};

		Mode mode;
		
		Nation::Id nation_id;

		string memtag;
		
		int verbose{0};


		Env *server{nullptr};   // wtf?

		


		Console(Env & env, Runner & runner, bool verbose):
			env(env), server(&env), runner(runner), verbose(verbose)
		{

			for (auto c: CHARSET) {
				charset.insert(c);
			}

			chi = history.begin();
			mod = 0;
			mode = Mode::AMERICA;
			clear();

		}

		void move_view(v2s delta) {
			view_pos += delta;
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

		void select_next_unit() {
			select_unit(
				mem.get_next_to_order(
					env,
					env.get_current_nation(),
					get_sel_unit()
				)
			);
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


		void order_unit(Unit::Id unit_id, char letter, int8 dx=0, int8 dy=0) {
			// R,P,O,B - road,plow,clear-forest,build-colony
			// 1,2,3,4,6,7,8,9 - move dir

			inter::Any a = get_inter(unit_id, letter, dx, dy);

			if (exec(a)) {
				mem.set_order(unit_id, letter, dx, dy);
				select_next_unit();
			}


		}





		void select_unit(Unit *unit) {
			sel_unit = unit;
			if (unit) {
				mem.set_order(unit->id, '-');
			}
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






		char get_letter(Unit const& u) {
			return mem.get_order(env.get_id(u)).code;
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



		void skip_unit() {
			mem.set_order(get_sel_unit()->id, ' ');
			select_next_unit();
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

		void repeat_all() {
			// any user input except hover -> break
			//on([&stop](){ stop = true; })

			auto& n = env.get_current_nation();

			while (Unit *unit = mem.get_next_to_repeat(env, n))
			{
				// exec action
				print("exec action %||\n", get_letter(*unit));

				auto unit_id = unit->id;

				auto o = mem.get_order(unit_id);

				try {
					get_server().apply_inter(
						get_inter(unit_id, o.code, o.dx, o.dy), auth
					);
				}
				catch (Error & e) {
					mem.set_order(unit_id);
					put(e.what());
					print("%||\n", e.what());
				}

			}
			select_next_unit();



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
					case front::EventKeyDown:
						if (ev.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
							app.stop();
						}
						break;
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
