#pragma once

#include <chrono>
#include <iostream>

#include "../ext/format.hpp"
#include "env.hpp"
#include "player.hpp"
#include "inter.hpp"
#include "agent.hpp"

namespace simple_ai{
	
	
	
	using col::Agent;
	using col::Env;
	using col::Nation;
	using col::Unit;
	namespace inter = col::inter;


	



	struct SimpleAi: Agent {

		Env & env;
		Nation & nation;
		//Nation::Auth auth{0};

		bool step();
		void sync();

		SimpleAi(Env & env, Nation & nation): env(env), nation(nation) {
			sync();
		}

	};

}

