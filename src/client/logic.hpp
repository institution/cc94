#pragma once

#include <queue>
#include "env.hpp"

namespace col{

	namespace logic{


		using UnitTypes = vector<UnitType const*>;
		using Items = vector<Item>;

		UnitTypes equip_to_types(Env const& env, Unit const& u);

		Items const& get_all_items(Env const& env);
		

		Unit const* get_unassigned_unit(Env const& env, Terr const& terr);

		inline
		bool to_move(Unit const& u) {
			return u.get_time_left() and !u.is_working();
		}

		/*inline
		bool owned(Unit const& u, Nation const& p) {
			return u.get_nation() == p;
		}*/


		

		inline bool is_constructor(Build const& b) {
			return b.get_proditem() == ItemHammers;			
		}




		
		Amount get_nominal_prod(Env const& env, Workplace const& fact, Item const& item);

		
		// Fill registers describing colony resources ballance
		// lim -- limited delta (by supply)
		// nom -- nominal delta
		void fill_colony_regs(Env const& env, Terr const& terr, Register & lim, Register & nom);






		
		Unit const* find_unit(Env const& env, Unit const* cur, function<bool(Unit const&)> func);
		Unit * find_unit(Env & env, Unit * cur, function<bool(Unit const&)> func);

		inline
		float get_moves(Unit const& u) {
			int unit_cost = TIME_UNIT/u.get_speed();
			return u.get_time_left() / unit_cost;
		}

		


		/*struct Routes
		
		struct Supply*/


	}

}

