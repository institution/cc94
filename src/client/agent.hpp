#pragma once

#include "env.hpp"


namespace col{

	struct Agent {
		// not necessarily player, for ex: during map editing
		virtual bool step() = 0;
		virtual ~Agent() {}
	};
}




