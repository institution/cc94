#ifndef LOGIC_H_39406754
#define LOGIC_H_39406754

#include "env.h"

namespace col{

	namespace logic{


		using UnitTypes = vector<UnitType const*>;
		using Items = vector<Item>;

		UnitTypes equip_to_types(Env const& env, Unit const& u);

		Items const& get_all_items(Env const& env);
		Items get_proditems(Env const& env, Field const& f);

		Unit const* get_unassigned_unit(Env const& env, Terr const& terr);

		inline
		bool to_move(Unit const& u) {
			return u.get_time_left() and !u.is_working();
		}

		inline
		bool owned(Unit const& u, Nation const& p) {
			return u.get_nation() == p;
		}








		Amount get_nominal_prod(Workplace const& fact, Item const& item);
		
		// Fill registers describing colony resources ballance
		// lim -- limited delta (by supply)
		// nom -- nominal delta
		void fill_colony_regs(Terr const& terr, Register & lim, Register & nom);






		Unit const* get_next_to_move(Env const& env, Nation const& p, Unit const* cur=nullptr);
		Unit::Id get_next_to_move_id(Env const& env, Nation const& pl, Unit::Id cur_id=0);

		Unit const* find_unit(Env const& env, Unit const* cur, function<bool(Unit const&)> func);
		Unit * find_unit(Env & env, Unit * cur, function<bool(Unit const&)> func);

		inline
		float get_moves(Unit const& u) {
			int unit_cost = TIME_UNIT/u.get_speed();
			return u.get_time_left() / unit_cost;
		}

		struct Memory{
			// remember and repeat orders


			struct Order{
				char code;
				int8 dx, dy;
				Order(char c, int8 dx=0, int8 dy=0): code(c), dx(dx), dy(dy) {}
				Order() = default;
			};

			// next active unit without order
			Unit * get_next_to_order(Env & env, Nation const& p, Unit * cur = nullptr) const;
			Unit const* get_next_to_order(Env const& env, Nation const& p, Unit const* cur = nullptr) const;

			// next active unit with order
			Unit * get_next_to_repeat(Env & env, Nation const& p, Unit * cur = nullptr) const;
			Unit const* get_next_to_repeat(Env const& env, Nation const& p, Unit const* cur = nullptr) const;

			using Unit2Order = map<Unit::Id, Order>;
			Unit2Order uos;



			Order get_order(Unit::Id id) const {
				auto p = uos.find(id);
				if (p != uos.end()) {
					return (*p).second;
				}
				return Order('-');
			}

			Memory & set_order(Unit::Id id, char c='-', int8 dx=0, int8 dy=0) {
				uos[id] = Order(c, dx, dy);
				return *this;
			}


		};


	}

}


#endif
