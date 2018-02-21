#pragma once

#include "col.hpp"
#include "env.hpp"
#include "console.hpp"
#include "layout.hpp"
#include "res.hpp"
#include "render_func.hpp"

namespace col {

	void render(col::Env & env, col::Console & con, int verbose);

	
	void start_gui(Console & con);


}

