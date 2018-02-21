#pragma once

#include "rcparser.hpp"
#include "env.hpp"

namespace col {

	void read_defs(RCParser & p, Env & env);
	void read_defs(Path const& path, Env & env);
	
}
