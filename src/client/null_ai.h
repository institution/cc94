#ifndef NULL_AI_H
#define NULL_AI_H

#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>

#include "envgame.h"
#include "player.h"
#include "format.hpp"
#include "inter.h"

namespace null_ai{
	// AI operating under coded "expert" rules

	using col::Player;
	using col::EnvGame;
	using col::Nation;
	using col::Ready;
	namespace inter = col::inter;

	struct Apply: boost::static_visitor<void>{
		template <class T>
		void operator()(T const& t) const {
			std::cout << format("Null Ai: inter received: %||\n", inter::type_name<T>::get());
		}
	};

	struct NullAi: Player {
		std::mutex mtx;

		NullAi() {

		}

		void activate() {
			std::cout << "Null Ai: activate\n";
			mtx.unlock();
		}

		void apply_inter(col::inter::Any const& a) {
			boost::apply_visitor(Apply(), a);
		}


	};


	void run(Nation::Id pid, EnvGame * env, bool *running);

}


#endif
