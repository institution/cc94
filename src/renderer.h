#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "col.hpp"
#include "env.h"
#include "console.h"
#include "pixfont.h"
#include "layout.h"

namespace col {

	void render(sf::RenderWindow &app, col::Env & env, col::Console & con);

	void preload_terrain();

}


#endif
