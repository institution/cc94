#ifndef CONSOLE_H
#define CONSOLE_H

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <deque>
#include <functional>
#include <thread>

#include "col.hpp"
#include "envgame.h"
#include "layout.h"
#include "action.h"
#include "ai.h"
#include "user.h"
#include "ai-env-helpers.h"

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


	//Memory mem;


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

	inline
	bool overlap(Box2 const& b, v2i const& pos) {
		return
			b.pos[0] <= pos[0] and
			b.pos[1] <= pos[1] and
			pos[0] <= b.end[0] and
			pos[1] <= b.end[1];
	}



	struct Console: User{
		vector<string> output;
		string buffer;
		std::deque<string> history;
		std::deque<string>::iterator chi;

		EnvGame & env;

		uint32 mod;

		float time{0.0f};

		int equip_to_unit_id{0};
		int equip_to_unit_type_id{0};

		int select_unit_popup{0};

		// selected square
		Terr * sel_terr{nullptr};

		// selected unit
		Unit* sel_unit{nullptr};


		void select_unit(Unit *unit) {
			sel_unit = unit;
		}

		void select_unit(Unit &unit) {
			sel_unit = &unit;
		}

		void select_unit(Unit::Id id) {
			if (id) {
				sel_unit = &env.get<Unit>(id);
			}
			else {
				sel_unit = nullptr;
			}
		}

		void unselect_unit() {
			select_unit(nullptr);
		}
		

		void select_terr(Terr * terr) {
			sel_terr = terr;
			sel_unit = nullptr;
		}

		void select_terr(Terr & terr) {
			sel_terr = &terr;
			sel_unit = nullptr;
		}

		void select_terr(Terr::Id const& c) {
			if (env.in_bounds(c)) {
				sel_terr = &env.get_terr(c);
			}
			else {
				sel_terr = nullptr;
			}
			sel_unit = nullptr;
		}

		void unselect_terr() {
			sel_terr = nullptr;
			sel_unit = nullptr;
		}




		Unit* get_sel_unit() const {
			return sel_unit;
		}

		Terr* get_sel_terr() const {
			if (sel_unit) {
				return &env.get_terr(*sel_unit);
			}
			return sel_terr;
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



		void activate() {
			put("Activate!");
		}



		Item get_next_workitem_field(Env const& env, Field const& f) const;


		// EVENTS --------------

		enum struct Dev {
			None, Mouse, Keyboard
		};

		enum struct Mod {
			None, Alt, Ctrl, Shift
		};

		enum struct Event {
			Press, Release, Hover, Char
		};

		enum struct Button{
			None, Left, Right,
		};

		using Key = sf::Keyboard::Key;



		struct HotSpot{
			Dev d{Dev::Mouse};
			Event t{Event::Press};
			Button b{Button::None};
			Key k{Key::Unknown};
			Box2 box{0,0,0,0};
			bool anywhere{0};
			std::function<void()> callback;
			uint32 u{0};

			HotSpot() {}

			HotSpot(
				Box2 const& box,
				std::function<void()> callback,
				Event t,
				Button b,
				bool anywhere
			):
				box(box), callback(callback),
				t(t), b(b), anywhere(anywhere) {}

			HotSpot(
				std::function<void()> callback,
				Event t,
				Key k
			):
				callback(callback),
				d(Dev::Keyboard), t(t), k(k), anywhere(1) {}

			HotSpot(
				std::function<void()> callback,
				Event t
			):
				callback(callback),
				d(Dev::Keyboard), t(t), anywhere(1) {}

			HotSpot& set_unicode(uint32 uni) { u = uni; return *this; }

		};



		void on(Event t, uint32 uni, std::function<void()> cl) {
			hts.push_back(HotSpot(cl, t).set_unicode(uni));
		}


		//
		void on(Event t, Key k, std::function<void()> cl) {
			hts.push_back({cl, t, k});
		}


		// ----------- callback

		// mouse button over area
		void on(Event t, Button b, v2i const& pos, v2i const& dim, std::function<void()> cl) {
			hts.push_back({Box2(pos, dim), cl, t, b, false});
		}

		// mouse button anywhere
		void on(Event t, Button b, std::function<void()> cl) {
			hts.push_back({Box2(0,0,0,0), cl, t, b, true});
		}

		// mouse hover over area
		void on(Event t, v2i const& pos, v2i const& dim, std::function<void()> cl) {
			hts.push_back({Box2(pos, dim), cl, t, Button::None, false});
		}

		// mouse hover anywhere
		void on(Event t, std::function<void()> cl) {
			hts.push_back({Box2(0,0,0,0), cl, t, Button::None, true});
		}



		// ----------- autocommand

		// mouse button over area - string command
		void on(Event t, Button b, v2i const& pos, v2i const& dim, string const& cmd) {
			on(t, b, pos, dim, [this,cmd](){
				command(cmd);
			});
		}

		// mouse button anywhere - string command
		void on(Event t, Button b, string const& cmd) {
			on(t, b, [this,cmd](){
				command(cmd);
			});
		}

		// mouse hover over area - string command
		void on(Event t, v2i const& pos, v2i const& dim, string const& cmd) {
			on(t, pos, dim, [this,cmd](){
				command(cmd);
			});
		}

		// mouse hover anywhere - string command
		void on(Event t, string const& cmd) {
			on(t, [this,cmd](){
				command(cmd);
			});
		}








		BuildType::Id select_build{0};
		int sel_slot_num{-1};

		using HotSpots = std::vector<HotSpot>;

		HotSpots hts;

		void trapall() {
			hts.clear();
		}



		bool handle_event(Dev dev, v2i const& pos, Event type, Button button, Key key, uint32 uni=0) {
			for (int i = hts.size(); 0 < i; --i) {
				auto& p = hts[i-1];
				if (p.d == dev and p.t == type
					and (p.anywhere or overlap(p.box, pos))
					and (p.b == Button::None or p.b == button)
					and (p.k == Key::Unknown or p.k == key)
					and (p.u == 0 or p.u == uni)
				) {
					p.callback();
					return true;
				}
			}
			return false;
		}




		// is console active - keyboard focus
		int active{false};

		int sel_colony_slot_id{-1};

		Mode mode;
		vector<std::thread> & ths;
		bool &running;

		Console(EnvGame & env, vector<std::thread> & ths, bool & running): env(env), ths(ths), running(running) {
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



		void exec(Action const& action) {
			env.exec(action);
		}



		void clear() {
			output.clear();
			buffer = "";
			unselect_unit();
			unselect_terr();
		}

		void put(string const& s) {
			output.push_back(s);
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


	void run_console(EnvGame *ee, vector<std::thread> * tt, bool *running);

}
#endif
