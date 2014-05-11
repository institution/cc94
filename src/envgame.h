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


		unique_ptr<Action> create_random_action(Player::Id const& pid) {
			// OrderMove or Ready

			if (!in_progress()) {
				return unique_ptr<Action>(nullptr);
			}

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
					/*
					if (dir != Dir::S) {
						auto dest_pos = get_coords(u) + vec4dir(dir);
						if (in_bounds(dest_pos)) {
							auto& dest = get_terr(dest_pos);

							if (has_defender(dest) and get_defender(dest).get_player().id != pid) {
								return unique_ptr<Action>(new OrderAttack(pid, u.id, dir));
							}
							else {
								return unique_ptr<Action>(new OrderMove(pid, u.id, dir));
							}
						}
						else {
							u.order = Order::Space;
						}
					}
					else {
						u.order = Order::Space;
					}
					*/
				}
			}
			return unique_ptr<Action>(new Ready(pid));

		}

		unique_ptr<Action> create_random_action() {
			return create_random_action(get_current_player().id);
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
