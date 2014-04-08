#ifndef ENVWRAP_H
#define ENVWRAP_H

#include "env.h"
#include "action.h"

namespace col{
	
	struct EnvGame: Env {
		// add concept of "current player"
		// add player score
		// extract actions and create_random_action function
		
		
		Player::Id current_player_id;
		
		EnvGame(): Env() {
			current_player_id = -1;
		}
				
		int32 get_result(Player::Id pid) {
			
		}
		
		void exec(Action const& a) {
			a.exec(*this);
		}
		
		
	};
	
	void copy(EnvGame &trg, EnvGame const& src);

	
}

#endif
