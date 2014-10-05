#ifndef EXPERT_AI_H
#define EXPERT_AI_H

#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>

#include "env.h"
#include "player.h"
#include "inter.h"

namespace expert_ai{
	// AI operating under coded "expert" rules

	using col::Player;
	using col::Env;
	using col::Nation;

	struct ExpertAi: Player {

		void play(Env & env, Nation::Id nation_id, Nation::Auth nation_auth) {
			print("expert_ai: ready\n");
			env.apply_inter(col::inter::ready(nation_id), nation_auth);
		}


	};

}


#endif
