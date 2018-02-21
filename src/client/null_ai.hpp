#pragma once

#include <chrono>
#include <iostream>

#include "../ext/format.hpp"
#include "env.hpp"
#include "agent.hpp"
#include "timer.hpp"

namespace null_ai{
	
	using col::Agent;
	using col::Env;
	using col::Faction;
	using col::Control;
	namespace inter = col::inter;

	struct NullAi: Agent {

		Env & env;
		Control cc;

		NullAi(Env & env, Control cc): env(env), cc(cc) {
			
		}		

		void run()
		{
			col::Timer t(100);
			while (1)
			{
				if (env.in_progress()) {
					if (env.get_current_control() == cc) {
						print("null_ai: ready\n");
						env.ready();
					}
				}
				
				if (env.get_state() == col::EnvStateExit) {
					break;
				}
							
				t.wait();
			}
		}


	};

}

