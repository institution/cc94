#include "expert_ai.h"

#include <exception>
#include <mutex>

namespace expert_ai{
		
	void run(Player::Id pid, EnvGame * env, bool * running) {
		
		ExpertAi ai;
		env->connect(pid, ai);
		ai.mtx.lock();
		
		while (1) {
			ai.mtx.lock();
			
			if (*running) {
				std::cout << "AI active" << std::endl;
				// TODO ... send some commands
				env->exec(Ready(pid));
			}
			else {
				break;
			}

			//std::this_thread::sleep_for(std::chrono::seconds(2));
		}
		std::cout << "expert ai - clean exit\n";
	}

	
	
}
