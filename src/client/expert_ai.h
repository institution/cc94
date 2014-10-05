#ifndef EXPERT_AI_H
#define EXPERT_AI_H

#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>

#include "envgame.h"
#include "player.h"
#include "inter.h"

namespace expert_ai{
	// AI operating under coded "expert" rules

	using col::Player;
	using col::EnvGame;
	using col::Nation;
	using col::Ready;

	struct ExpertAi: Player {
		std::mutex mtx;

		ExpertAi() {

		}

		void activate() {
			mtx.unlock();
		}

		void apply_inter(col::inter::Any const& a, Player & s) {

		}


	};


	void run(Nation::Id pid, EnvGame * env, bool *running);

}


#endif
