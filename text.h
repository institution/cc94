#ifndef TEXT_H2
#define TEXT_H2

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <string>
#include <boost/format.hpp>
#include "uint.h"

using boost::format;
using std::cout;

struct Text{
	std::string text;
	uint width;
	sf::Vector2i pos;
	
	const sf::Font *f;
	
	 	
};

void render_text(sf::RenderWindow &win, const Text &text); 


//IntRect 	Rectangle
// 	Bounding rectangle of the glyph, in relative coordinates.

#endif