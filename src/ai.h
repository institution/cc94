#ifndef AI3_H
#define AI3_H

#include "mcts.hpp"
#include "envgame.h"

namespace col {

	using GameType = EnvGame;
	using PlayerIdType = Player::Id;

	// template G(ameType) P(layerIdType)
	struct Ai {
		// mcts tree node type
		using NodeType = tree::Node<Action>;
		// for playing random games
		GameType game_copy;
		PlayerIdType pid;
		NodeType *root {nullptr};
		int action_count{0};


		void create(PlayerIdType const& pid) {
			this->pid = pid;
			auto tmp = unique_ptr<Action>(nullptr);
			root = new NodeType(tmp);
		}

		void destroy() {
			if (root) {
				delete root;
				root = nullptr;
			}
		}



		Ai(PlayerIdType const& pid) {
			create(pid);
		}

		~Ai() {
			destroy();
		}

		Ai(Ai const&) { throw Critical("disabled"); }
		Ai(Ai &&) { throw Critical("disabled"); };
		Ai const& operator=(Ai const&) { throw Critical("disabled"); };


		void reset(PlayerIdType const& pid) {
			// clear tree
			//auto tmp = unique_ptr<Action>(new Start(1));
			destroy();
			create(pid);
		}


		void apply(Action const& a) {
			/* Choose tree branch */
			mcts::apply(root, a);
			action_count += 1;
		}

		Action const& preferred() {
			/* Return preferred action */

			// select preferred move
			NodeType *node = mcts::preferred_node(root);

			return *(node->action);

			/*
			if (node != nullptr) {
				cout << "preferred move: " << *node->action << endl;
				return to_string(*node->action);
			}
			else {
				cout << "no possible moves (game ended)" << endl;
				return "no possible moves (game ended)";
			}
			*/


		}

		Action const& calc(GameType const& game, int limit, int verbose=0) {
			/*
				Action& run_ai(const& game_state, time_limit) -> tree must be empty or correspond to game_state
				limit -- number of steps (random games played)
				return -- preffered action
			*/

			if (action_count != game.action_count) {
				throw Critical("unsynchronized");
			}

			for (int i=0; i<limit; ++i) {

				//cout << "dumping tree before step: " << endl;
				//dump(root, 8);
				//cout << endl;

				// copy game state visible for player
				copy_det(game_copy, game, pid);

				// determinization
				// ...

				//
				game_copy.verbose = verbose;
				game_copy.turn_limit = game_copy.turn_no + 100; // look ahead 20 turns

				//cout << "after reset:" << endl;
				//dump(gw.game);

				//cout << "step" << endl;
				mcts::step(root, game_copy, verbose);

				//cout << "game state after play: " << endl;
				//dump(gw.game);
				//cout << endl;

			}

			//if (verbose)
				dump(root, 4);




			// select preferred move
			NodeType *node = mcts::preferred_node(root);

			if (verbose and node == nullptr) {
				cerr << "ai/run: no possible moves (game ended)" << endl;
			}

			return *(node->action);

			/*
			if (node != nullptr) {
				cout << "preferred move: " << *node->action << endl;
				return to_string(*node->action);
			}
			else {
				cout << "no possible moves (game ended)" << endl;
				return "no possible moves (game ended)";
			}
			*/
		}




	}; // struct





} // ns


#endif
