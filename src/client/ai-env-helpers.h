#ifndef AI_ENV_HELPERS_H
#define AI_ENV_HELPERS_H

#include "env.h"

namespace col{

	namespace misc{

		using UnitTypes = vector<UnitType const*>;
		using Items = vector<Item>;

		UnitTypes equip_to_types(Env const& env, Unit const& u);

		Items const& get_all_items(Env const& env);
		Items get_proditems(Env const& env, Field const& f);

		Unit const* get_unassigned_unit(Env const& env, Terr const& terr);

		int get_yield(Env const& env, Build const& b);

		inline
		bool to_move(Unit const& u) {
			return u.get_time_left() and !u.is_working();
		}

		inline
		bool owned(Unit const& u, Nation const& p) {
			return u.get_nation() == p;
		}

		Unit const* get_next_to_move(Env const& env, Nation const& p, Unit const* cur=nullptr);
		Unit::Id get_next_to_move_id(Env const& env, Nation const& pl, Unit::Id cur_id=0);

		Unit * find_unit(Env & env, Unit * cur, function<bool(Unit const&)> func);

		inline
		float get_moves(Unit const& u) {
			int unit_cost = TIME_UNIT/u.get_speed();
			return u.get_time_left() / unit_cost;
		}

		struct Memory{
			// remember unfinished orders

			struct Order{
				char code;
				Order(char c): code(c) {}
				Order() = default;
			};

			Unit * get_next_unit(Env & env, Nation const& p, Unit * cur) const;
			bool has_next_unit(Env const& env, Nation const& p) const;

			using Unit2Order = map<Unit::Id, Order>;
			Unit2Order uos;

			Order get_order(Unit::Id id) const {
				auto p = uos.find(id);
				if (p != uos.end()) {
					return (*p).second;
				}
				return Order('-');
			}

			Memory & set_order(Unit::Id id, char c) {
				uos[id] = Order(c);
				return *this;
			}


		};


	}

}


#endif
