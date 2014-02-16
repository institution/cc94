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
#include "col.h"



namespace col {


	struct Text{
		std::string text;
		uint width;
		Vector2<int16> pos;

		const sf::Font *f;

		Text() {
			width = 100;
			pos = Vector2<int16>(0,0);
			text = "";
		}

		/*Text(text, pos, width) {
			width = width;
			pos = Vector2<int16>(0,0);
			text = "";
		}*/

	};	

	int16 render_text(
		sf::RenderWindow &win, 
		Vector2<int16> const& pos,
		sf::Font const& font,
		string const& s,
		int16 width = -1
	);


}

//IntRect 	Rectangle
// 	Bounding rectangle of the glyph, in relative coordinates.

#endif