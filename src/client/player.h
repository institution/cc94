#ifndef PLAYER_H_54754745
#define PLAYER_H_54754745

#include "env.h"


namespace col{

	struct Player {
		
		
		
		virtual bool step(Env &, Nation::Id, Nation::Auth) = 0;
		
		virtual void start() {}
		virtual void stop() {}

		virtual ~Player() {}
	};
}


#endif
