#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "col.h"
#include "env.h"
#include "console.h"
#include "pixfont.h"

namespace col {
	
	void render(sf::RenderWindow &app, const col::Env &env, const col::Console &con);

}


#endif
