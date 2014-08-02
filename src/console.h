#ifndef CONSOLE_H
#define CONSOLE_H

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <deque>
#include <functional>

#include "col.hpp"
#include "envgame.h"
#include "layout.h"
#include "action.h"
#include "ai.h"
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


	int const ActiveNone = 0;
	int const ActiveMsg = 1;
	int const ActiveCmd = 2;



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


	struct Console{
		vector<string> output;
		string buffer;
		std::deque<string> history;
		std::deque<string>::iterator chi;

		EnvGame &envgame;

		uint32 mod;

		// ai
		using Ais = unordered_map<Player::Id, Ai>;

		Ais ais;


		int equip_to_unit_id{0};
		int equip_to_unit_type_id{0};

		int select_unit_popup{0};



		void select_terr(int x, int y) {
			sel = Coords(x,y);
		}

		void unselect_terr(int x, int y) {
			// ???
		}


		// selected square
		Terr::Id sel;

		// selected unit
		Unit::Id sel_unit_id{0};


		unordered_set<char16_t> charset;

		// active screen
		enum class Mode{
			AMERICA, COLONY, EUROPE, REPORT
		};

		void reset_hotspots() {
			hts.clear();
		}

		void click_colony_field(Terr const& terr) {
			cout << "click_colony_field: " << envgame.get_coords(terr) << endl;
		}

		//void click_colony_build(Colony const& col) {
		//	cout << "click_colony_build: " << envgame.get_coords(terr) << endl;
		//}

		void click_map_tile(Terr const& terr) {
			cout << "click_colony_field: " << envgame.get_coords(terr) << endl;
		}







		Item get_next_workitem_field(Env const& env, Field const& f) const;


		// EVENTS --------------

		enum struct Event {
			Press, Release, Hover
		};

		enum struct Button{
			None, Left, Right
		};

		struct HotSpot{
			Event t{Event::Press};
			Button b{Button::None};
			Box2 box{0,0,0,0};
			bool anywhere{0};
			std::function<void()> callback;

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

		};

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



		bool handle_event(v2i const& pos, Event type, Button button) {
			for (int i = hts.size(); 0 < i; --i) {
				auto& p = hts[i-1];
				if (p.t == type
					and (p.anywhere or overlap(p.box, pos))
					and (p.b == Button::None or p.b == button)
				) {
					p.callback();
					return true;
				}
			}
			return false;
		}





		// is console active - keyboard focus
		int active{ActiveMsg};

		int sel_colony_slot_id{-1};

		Mode mode;

		Console(EnvGame &envgame_): envgame(envgame_) {
			for (auto c: CHARSET) {
				charset.insert(c);
			}

			chi = history.begin();
			sel = Coords(-1,-1);
			mod = 0;
			mode = Mode::AMERICA;
			clear();


		}

		Console() = delete;
		Console(Console const&) = delete;
		Console const& operator=(Console const&) = delete;


		void create_ai(Player::Id const& pid) {
			ais.emplace(pid, pid);
			auto it = ais.find(pid);
			(*it).second.action_count = envgame.action_count;
		}

		void exec(Action const& action) {
			envgame.exec(action);

			for (auto &p: ais) {
				auto& ai = p.second;
				ai.apply(action);
			}

		}



		void clear() {
			output.clear();
			buffer = "";
			sel_unit_id = 0;
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


	};



}
#endif
