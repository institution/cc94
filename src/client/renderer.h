#ifndef RENDERER_H_875487584
#define RENDERER_H_875487584

#include "col.hpp"
#include "env.h"
#include "console.h"
#include "backend/pixfont.h"
#include "layout.h"

namespace col {

	void render(backend::Back &app, col::Env & env, col::Console & con, int verbose);

	void preload_renderer(backend::Back &app);

}


#endif
