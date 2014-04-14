#ifndef ENVWRAP2_H
#define ENVWRAP2_H

#include <memory>

#include "env.h"
#include "action.h"

namespace col{
	
	using std::unique_ptr;
	
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
	
	
		/*
	unique_ptr<Action> create_random_action() {		
		//AttackMove or EndTurn
			
		
		if (!game.end) {			
			int i = roll::roll(0, game.free);
						
			int j = 0;
			while (j < 9) {
				if (game.bs[j] == '.') {
					//cout << "i = "<< i << endl;
					if (i == 0) break;
					--i;
				}
				++j;
			}			
			return unique_ptr<Action>(new Move(game.player, j));
			
		}
		else {
			return unique_ptr<Action>(nullptr); 
		}
		
	}
		 */

	
}

#endif
