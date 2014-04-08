#ifndef CONSOLE_H
#define CONSOLE_H

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


#include "col.h"
#include "envgame.h"

namespace col {

	struct Console{
		vector<string> output;
		string buffer;
		
		EnvGame &envgame;
		
		uint32 mod;
		
		// selected square
		Coords sel;
		
		enum class Mode{
			AMERICA, COLONY, EUROPE
		};
		
		Mode mode;

		Console(EnvGame &envgame_): envgame(envgame_) {
			
			sel = Coords(-1,-1);
			mod = 0;
			mode = Mode::AMERICA;
			clear();
		}

		void clear() {
			output.clear();
			buffer = "";
		}

		void modified() {
			++mod;
		}
		
		void handle_char(uint16 code);
		void handle(sf::RenderWindow const&, sf::Event const&);

	};
	


}
#endif
