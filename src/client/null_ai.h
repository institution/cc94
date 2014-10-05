#ifndef NULL_AI_H
#define NULL_AI_H

#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>

#include "env.h"
#include "player.h"
#include "format.hpp"
#include "inter.h"
#include "player.h"

namespace null_ai{
	// AI operating under coded "expert" rules

	using col::Player;
	using col::Env;
	using col::Nation;
	namespace inter = col::inter;


	struct NullAi: Player {

		void play(Env & env, Nation::Id nation_id, Nation::Auth nation_auth) {
			print("null_ai: ready\n");
			env.apply_inter(inter::ready(nation_id), nation_auth);
		}


	};

}


#endif
