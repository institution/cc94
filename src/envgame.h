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


		bool check(int L, int M) const {
			return (roll::roll2(0,M) < L);
		}

		bool get_assign_action(Assign & a, Player::Id const& pid, Unit const& unit) {
			// [0-14] building
			// [16-24] field square

			auto & t = get_terr(unit);
			auto & c = t.get_colony();

			// try 4 times
			for (int i=0; i<4; ++i) {

				// factory or field
				if (check(1,2)) {

					// which manufacture
					auto const& slot_id = roll::roll2(0,16);

					auto & b = c.get_build(slot_id);

					if (b.assign(false)) {
						a = Assign(pid, slot_id, unit.id, b.get_proditem());
						return true;
					}


				}
				else {
					// which field
					auto field_id = roll::roll2(0,9);

					auto& f = get_colony_field(t, field_id);

					if (f.assign(false)) {
						// random workitem
						auto const& xs = f.get_proditems();

						a = Assign(pid, 16+field_id, unit.id,
							xs.at(roll::roll2(0, xs.size()))
						);
						return true;
					}
				}
			}

			return false;
		}







		unique_ptr<Action> create_random_action() {
			// OrderMove or Ready

			if (in_progress()) {
				auto pid = get_current_player().id;

				for (auto& it: units) {
					auto& u = it.second;


					if (u.order != Order::Space and u.get_player().id == pid) {

						if (u.assigned() and roll::roll2(0, 16) >= 2) {
							u.order = Order::Space;
							continue;
						}

						// assign or move

						auto const& t = get_terr(u);
						if (t.has_colony() and check(4,5)) {
							// assign work
							Assign tmp(pid,0,0,0);
							if (get_assign_action(tmp, pid, u)) {
								auto r = unique_ptr<Action>(new Assign(tmp));
								if ((*r).is_allowed(*this)) return r;
							}
						}
						else {
							// move
							auto dir = roll::roll2(0, 9);

							{
								auto r = unique_ptr<Action>(new OrderAttack(pid, u.id, dir));
								if ((*r).is_allowed(*this)) return r;
							}

							{
								auto r = unique_ptr<Action>(new OrderMove(pid, u.id, dir));
								if ((*r).is_allowed(*this)) return r;
							}
						} // if

					} // if

				} // for

				return unique_ptr<Action>(new Ready(pid));

			}
			else {
				return unique_ptr<Action>(nullptr);
			} // if

		} // func

	};

	/*
	void add_player() {
		auto& id = next_id<Player>();
		auto& p = create<Player>(id, );
	}*/

	void copy_det(EnvGame &trg, EnvGame const& src, Player::Id const& pid);







}

#endif
