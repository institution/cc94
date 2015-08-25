#ifndef GUI_H_857325634238
#define GUI_H_857325634238

#include "backend/backend.h"
#include "agent.h"
#include "console.h"

namespace col{

	struct Gui: Agent {
		Console con;
		Env & env;
		
		
		
		// last modification time
		backend::Tick last_mod_env;
		backend::Tick last_mod_con;
		
		backend::Tick last_tick;
		
		backend::Back app;
		
		int verbose{0};
		
		void init();
		
		bool step();
		
		Gui(Env & env, Runner & runner): env(env), con(env, runner, 0), verbose(0) {
			init();
		}
		
	};

	
}


#endif
