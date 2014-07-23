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


		bool check(int L, int M) {
			return (roll::roll2(0,M) < L);
		}

		bool assign(int num, Unit & unit, Item const& item, bool const& exec=1) {
			// Assign unit to work on colony slot
			// num -- slot number
			//    [0-14] building
			//    [16-24] field square
			// item -- item to produce
			// exec -- if 0 then only check arguments

		// [0-14] building
		// [16-24] field square

			t = env.get_terr(unit);
			t.get_colony()

			if (check(1,2)) {
				// work build
				slot_id = roll::roll2(0,16);

				b = c.get_build(slot_id);

				if (b.assign(false)) {
					// can assign

				}
				else {
					// build full
				}


			}
			else {
				// work field

			}

			nchoices = 15+9;

			rand_i = roll::roll2(0,nchoices+1)

			i = 0;
			nchoices = 0;

			for (int n=0; n<15; ++n) {



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


					if (u.order != Order::Space and u.get_player().id == pid)

						if (u.assigned() and roll::roll2(0, 16) >= 2) {
							u.order = Order::Space;
							continue;
						}

						// assign or move
						t = env.get_terr(unit);
						t.has_colony()
						get_random_work


						auto dir = roll::roll2(0, 9);

						{
							auto r = unique_ptr<Action>(new OrderAttack(pid, u.id, dir));
							if ((*r).is_allowed(*this)) return r;
						}

						{
							auto r = unique_ptr<Action>(new OrderMove(pid, u.id, dir));
							if ((*r).is_allowed(*this)) return r;
						}

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
