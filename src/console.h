#ifndef CONSOLE_H
#define CONSOLE_H

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


#include "col.h"
#include "envgame.h"

/*
 * place biome plains|tundra|grassland|...
 * place phys mountain
 * place unit eng colonist
 * clear unit
 * clear phys
 * move q|w|e|a|d|z|x|c
 * 
 * 
 */










namespace col {

	struct Console{
		vector<string> output;
		string buffer;
		
		EnvGame &envgame;
		
		uint32 mod;
		
		// selected square
		Coords sel;
		
		enum class Mode{
			AMERICA, COLONY, EUROPE, REPORT
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
