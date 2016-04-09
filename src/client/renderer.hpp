#pragma once

#include "col.hpp"
#include "env.hpp"
#include "console.hpp"
#include "layout.hpp"

namespace col {

	void render(Front &app, col::Env & env, col::Console & con, int verbose);

	void preload_renderer(Front &app);

}

