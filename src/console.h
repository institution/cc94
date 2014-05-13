#ifndef CONSOLE_H
#define CONSOLE_H

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <deque>


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
		unordered_set<char16_t> charset;

		// active screen
		enum class Mode{
			AMERICA, COLONY, EUROPE, REPORT
		};

		// is console active - keyboard focus
		bool active;

		Mode mode;

		Console(EnvGame &envgame_): envgame(envgame_) {
			active = 0;

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
		}

		void put(string const& s) {
			output.push_back(s);
		}


		void modified() {
			++mod;
		}

		void handle_char(uint16 code);
		void handle(sf::RenderWindow const&, sf::Event const&);

	};



}
#endif
