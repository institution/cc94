#ifndef CONSOLE_H
#define CONSOLE_H

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <deque>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>

#include "col.hpp"
#include "env.h"
#include "layout.h"
#include "player.h"
#include "ai-env-helpers.h"
#include "halo.h"
#include "format.hpp"
#include "player.h"
#include "game.h"


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
	void run_gui(Console * con, Env * env);


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





	struct Console: Player{
		vector<string> output;
		string buffer;
		std::deque<string> history;
		std::deque<string>::iterator chi;

		misc::Memory mem;

		Env & env;

		uint32 mod;

		Auth auth{0};

		float time{0.0f};

		int equip_to_unit_id{0};
		int equip_to_unit_type_id{0};

		int select_unit_popup{0};

		// selected area (affects edit mode)   TODO:: set
		std::set<Terr*> sel_terrs;

		// selected square
		//Terr * sel_terr{nullptr};

		// selected unit
		Unit* sel_unit{nullptr};

		Game & gm;

		void apply_inter(inter::Any const& a, Player & s);

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



		unordered_set<char16_t> charset;

		void skip_unit() {
			mem.set_order(get_sel_unit()->id, ' ');
			select_next_unit();
		}

		// active screen
		enum class Mode{
			AMERICA, COLONY, EUROPE, REPORT
		};

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

				// wait
				//std::this_thread::sleep_for(std::chrono::milliseconds(500));
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



		std::thread gui_th;

		std::mutex mtx;

		void start() {
			running = true;
			gui_th = std::move(std::thread(run_gui, this, &env));
		}

		void stop() {
			running = false;
			gui_th.join();
		}


		void play(Env & env, Nation::Id nation_id, Nation::Auth nation_auth) {
			//print("console: active\n");

			mtx.lock();
		}



		Item get_next_workitem_field(Env const& env, Field const& f) const;


		BuildType::Id select_build{0};
		int sel_slot_num{-1};



		using Event = halo::Event;
		using Button = halo::Button;
		using Key = halo::Key;
		using Pattern = halo::Pattern;
		using Dev = halo::Dev;


		halo::Patterns hts;

		void trapall() {
			hts.clear();
		}

		template<typename... T>
		void on(T&&... t) {
			hts.push_back(halo::Pattern(std::forward<T>(t)...));
		}

		// mouse button over area - string command
		void on2(Event t, Button b, v2i const& pos, v2i const& dim, std::string const& cmd) {
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
		void on2(Event t, v2i const& pos, v2i const& dim, std::string const& cmd) {
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



		// is console active - keyboard focus
		int active{false};

		int sel_colony_slot_id{-1};

		Mode mode;
		vector<std::thread> * ths;
		bool running{false};

		Nation::Id nation_id;

		string memtag;


		Env *server{nullptr};

		Env & get_server() { return *server; }
		void set_server(Env & server) { this->server = &server; }

		Console(Env & env, Game & gm):
			env(env), server(&env), gm(gm)
		{

			for (auto c: CHARSET) {
				charset.insert(c);
			}

			chi = history.begin();
			mod = 0;
			mode = Mode::AMERICA;
			clear();

		}



		Console() = delete;
		Console(Console const&) = delete;
		Console const& operator=(Console const&) = delete;


		void handle_events(sf::RenderWindow &app) {
			Console &con = *this;
			sf::Event ev;
			while (app.pollEvent(ev)) {
				if (ev.type == sf::Event::KeyPressed) {
					if (ev.key.code == sf::Keyboard::Escape) {
						app.close();
					}
				}
				else
				if (ev.type == sf::Event::Closed) {
					app.close();
				}

				con.handle(app, ev);
			}
		}


		void ready() {
			mtx.unlock();
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

		void handle_char(uint16 code);
		void handle(sf::RenderWindow const&, sf::Event const&);

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



	};



}
#endif
