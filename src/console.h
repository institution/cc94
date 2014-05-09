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

	struct Console{
		vector<string> output;
		string buffer;
		std::deque<string> history;
		std::deque<string>::iterator chi;

		EnvGame &envgame;

		uint32 mod;


		// experimental ai section

		EnvGame game_copy;

		// mcts tree node type
		using NodeType = tree::Node<Action>;

		NodeType *root;


		void init_ai() {

			auto tmp = unique_ptr<Action>(new Start(1));
			root = new NodeType(tmp);


			// = new NodeType(tmp);
			//roll::seed();
			//make_unique(Start())
			//unique_ptr<Action> tmp(new OXSim::Start('X'));
		}

		string run_ai() {
			cerr << "<<< AI --------------- >>>" << endl;


			for (int i=0; i<20; ++i) {

				//cout << "dumping tree before step: " << endl;
				//dump(root, 8);
				//cout << endl;

				copy_det(game_copy, envgame, 1);
				game_copy.verbose = 1;
				game_copy.turn_limit = game_copy.turn_no + 10; // look ahead 10 turns
				//cout << "after reset:" << endl;
				//dump(gw.game);

				//cout << "step" << endl;
				mcts::step(root, game_copy, 1);

				//cout << "game state after play: " << endl;
				//dump(gw.game);
				//cout << endl;


			}

			cout << "dumping tree: " << endl;
			dump(root, 4);

			// select preferred move
			NodeType *node = mcts::preferred_node(root);
			cout << "preferred move: " << *node->action << endl;

			return to_string(*node->action);

		}





		// selected square
		Coords sel;
		unordered_set<char16_t> charset;

		enum class Mode{
			AMERICA, COLONY, EUROPE, REPORT
		};

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

			// ai
			root = nullptr;

			init_ai();
		}

		~Console() {
			if (root) {
				delete root;
			}
		}

		Console() = delete;
		Console(Console const&) = delete;
		Console const& operator=(Console const&) = delete;

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
