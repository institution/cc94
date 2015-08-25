#ifndef NULL_AI_H
#define NULL_AI_H

#include <chrono>
#include <iostream>

#include "env.h"
#include "player.h"
#include "format.hpp"
#include "inter.h"
#include "agent.h"

namespace null_ai{
	
	using col::Agent;
	using col::Env;
	using col::Nation;
	namespace inter = col::inter;

	struct NullAi: Agent {

		Env & env;
		Nation::Id nation_id;

		NullAi(Env & env, Nation::Id nation_id): env(env), nation_id(nation_id) {
			
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


#endif
