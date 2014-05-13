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




		EnvGame(int verbose = 0): Env(verbose) {

		}


		void exec(Action const& a) {
			if (verbose) {
				cerr << "Exec " << a << endl;
			}
			a.exec(*this);
		}


		// Action create_random_action

		float get_result(Player::Id pid) {

			// % of units
			float score = 0, total = 0;
			for (auto& p: units) {
				if (p.second.get_player().id == pid) {
					score += 1;
				}
				total += 1;
			}

			if (total) {
				return score/total;
			}
			else {
				return score;
			}

		}



		unique_ptr<Action> create_random_action() {
			// OrderMove or Ready


			if (!in_progress()) {
				return unique_ptr<Action>(nullptr);
			}
			else {
				auto pid = get_current_player().id;

				for (auto& it: units) {
					auto& u = it.second;
					if (u.get_player().id == pid and u.time_left and u.order != Order::Space) {
						auto dir = roll::roll2(0, 9);

						{
							auto r = unique_ptr<Action>(new OrderAttack(pid, u.id, dir));
							if ((*r).is_allowed(*this)) return r;
						}

						{
							auto r = unique_ptr<Action>(new OrderMove(pid, u.id, dir));
							if ((*r).is_allowed(*this)) return r;
						}

						u.order = Order::Space;
					}
				}
				return unique_ptr<Action>(new Ready(pid));
			}

		}

	};

	/*
	void add_player() {
		auto& id = next_id<Player>();
		auto& p = create<Player>(id, );
	}*/

	void copy_det(EnvGame &trg, EnvGame const& src, Player::Id const& pid);







}

#endif
