#ifndef EXPERT_AI_H
#define EXPERT_AI_H

#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>

#include "envgame.h"
#include "user.h"

namespace expert_ai{
	// AI operating under coded "expert" rules

	using col::User;
	using col::EnvGame;
	using col::Player;
	using col::Ready;

	struct ExpertAi: User {
		std::mutex mtx;

		ExpertAi() {

		}

		void activate() {
			mtx.unlock();
		}




	};


	void run(Player::Id pid, EnvGame * env, bool *running);

}


#endif
