#ifndef HUMAN_AI_H
#define HUMAN_AI_H

#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>

#include "envgame.h"
#include "player.h"
#include "inter.h"
#include "console.h"

namespace human_ai{
	// AI operating under coded "expert" rules

	using col::Player;
	using col::EnvGame;
	using col::Nation;
	using col::Ready;

	struct HumanAi: Player {
		std::mutex mtx;

		HumanAi() {

		}

		void activate() {
			mtx.unlock();
		}

		void apply_inter(col::inter::Any const& a) {

		}


	};


	void run(Nation::Id pid, EnvGame * env, bool *running);

}


#endif
