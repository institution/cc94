#pragma once

#include <chrono>
#include <iostream>

#include "../ext/format.hpp"
#include "env.hpp"
#include "player.hpp"
#include "inter.hpp"
#include "agent.hpp"

namespace null_ai{
	
	using col::Agent;
	using col::Env;
	using col::Nation;
	namespace inter = col::inter;

	struct NullAi: Agent {

		Env & env;
		Nation::Id nation_id;

		NullAi(Env & env, Nation & nation): env(env), nation_id(nation.id) {
			
		}

		bool step() {
			
			Nation::Auth nation_auth = 0;
			
			if (env.in_progress()) {
				if (env.get_current_nation().id == nation_id) {
					print("null_ai: ready\n");
					env.apply_inter(inter::ready(nation_id), nation_auth);
				}
			}
			
			return true;
		}


	};

}

