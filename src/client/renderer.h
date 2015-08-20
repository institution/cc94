#ifndef RENDERER_H
#define RENDERER_H

#include "col.hpp"
#include "env.h"
#include "console.h"
#include "backend/pixfont.h"
#include "layout.h"

namespace col {

	void render(backend::Back &app, col::Env & env, col::Console & con, int verbose);

	void preload_fonts(backend::Back & back);
	void preload_terrain(backend::Back &app);

}


#endif
