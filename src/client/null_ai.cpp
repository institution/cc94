#include "null_ai.h"

#include <exception>
#include <mutex>

namespace null_ai{
		
	void run(Nation::Id pid, EnvGame * env, bool * running) {
		
		NullAi ai;
		env->connect(pid, ai);
		ai.mtx.lock();
		
		while (1) {
			ai.mtx.lock();
			
			if (*running) {
				std::cout << "Null AI active" << std::endl;
				
				// just ready
				env->exec(Ready(pid));
			}
			else {
				break;
			}

			//std::this_thread::sleep_for(std::chrono::seconds(2));
		}
		std::cout << "null ai - clean exit\n";
	}

	
	
}
