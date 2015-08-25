#ifndef AGENT_H_54754745
#define AGENT_H_54754745

#include "env.h"


namespace col{

	struct Agent {
		// not necessarily player, for ex: during map editing
		virtual bool step() = 0;
		virtual ~Agent() {}
	};
}




#endif
