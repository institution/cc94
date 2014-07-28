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

	std::u16string const CHARSET = u" !\"#$%'()+,-./0123456789:;<=>?ABCDEFGHIJKLMNOPRSTUWXYZ[\\]^_`vabcdefghijklmnoprstuwxyz{|}@~\r\b";


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


		// selected square
		Coords sel;
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

		struct HotSpot{
			static int const Click = 1;
			static int const Hover = 2;
			static int const RightClick = 4;

			int type{1}; // 1 - left click, 2 - hover

			Box2 box;
			std::function<void()> cl;

			HotSpot() {}
			HotSpot(Box2 const& box, std::function<void()> cl, int type): box(box), cl(cl), type(type) {}
		};

		BuildType::Id select_build{0};
		int sel_slot_num{-1};

		using HotSpots = std::vector<HotSpot>;

		HotSpots hts;

		void trapall() {
			hts.clear();
		}

		void onclick(v2i const& pos, v2i const& dim, std::function<void()> cl) {
			hts.push_back({Box2(pos, dim), cl, HotSpot::Click});
		}

		void onclick(v2i const& pos, v2i const& dim, string const& cmd) {
			onclick(pos, dim,
				[this,cmd](){ command(cmd); }
			);
		}

		void onhover(v2i const& pos, v2i const& dim, std::function<void()> cl) {
			hts.push_back({Box2(pos, dim), cl, HotSpot::Hover});
		}


		void onhover(v2i const& pos, v2i const& dim, string const& cmd) {
			onhover(pos, dim,
				[this,cmd](){ command(cmd); }
			);
		}

		void on(int flag, v2i const& pos, v2i const& dim, std::function<void()> cl) {
			hts.push_back({Box2(pos, dim), cl, flag});
		}

		bool handle_event(v2i const& pos, int type) {
			for (int i = hts.size(); 0 < i; --i) {
				auto& p = hts[i-1];
				if (p.type & type and overlap(p.box, pos)) {
					p.cl();
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

		Item get_next_workitem(Workplace const& workplace, Item const& item = ItemNone);

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


	};



}
#endif
