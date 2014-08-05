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

		bool can_move(Env const& env, Player const& p, Unit const& u);

		Unit const* get_next_to_move(Env const& env, Player const& p);

	}

}


#endif
