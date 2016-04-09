#pragma once

#include "../front/front.hpp"
#include "agent.hpp"
#include "console.hpp"

namespace col{

	struct Gui: Agent {
		Console con;
		Env & env;
		
		
		
		// last modification time
		front::Tick last_mod_env;
		front::Tick last_mod_con;
		
		front::Tick last_tick;
		
		front::Front app;
		
		int verbose{0};
		
		void init();
		
		bool step();
		
		Gui(Env & env, Runner & runner): env(env), con(env, runner, 0), verbose(0) {
			init();
		}
		
	};

	
}

