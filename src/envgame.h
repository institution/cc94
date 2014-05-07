#ifndef ENVWRAP2_H
#define ENVWRAP2_H

#include <memory>

#include "env.h"
#include "action.h"

namespace col{

	using std::unique_ptr;

	struct EnvGame: Env {
		// add player score
		// extract actions
		// create_random_action function




		bool verbose;


		EnvGame(bool verbose = false): Env(), verbose(verbose) {

		}


		void exec(Action const& a) {
			assert(a.pid == get_current_player().id);
			if (verbose) {
				cerr << "Exec " << a << endl;
			}
			a.exec(*this);
		}


		// Action create_random_action

		uint32 get_result(Player::Id pid) {

			// num of units
			uint32 score = 0;
			for (auto& p: units) {
				if (p.second.get_player().id == pid) {
					score += 1;
				}
			}


		}


		unique_ptr<Action> create_random_action() {
			// OrderMove or Ready

			for (auto& it: units) {
				auto& u = it.second;
				if (u.time_left) {

					return unique_ptr<Action>(new OrderMove(get_current_player().id, u.id, roll::roll2(0, 8)));

				}
			}
			return unique_ptr<Action>(new Ready(get_current_player().id));

		}
	};

	/*
	void add_player() {
		auto& id = next_id<Player>();
		auto& p = create<Player>(id, );
	}*/

	void copy(EnvGame &trg, EnvGame const& src);







}

#endif
