#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>
#include <vector>
#include <map>
#include <boost/cstdint.hpp>
#include <boost/multi_array.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "col.hpp"
#include "iomm.h"
#include "text.h"
#include "env.h"
#include "csv.h"
#include "ai.h"
#include "envgame.h"
#include "console.h"
#include "renderer.h"

using std::unique_ptr;

using std::ifstream;
using std::ofstream;
using std::string;
using std::add_pointer;
using std::to_string;

using std::cout;
using std::endl;

using boost::format;

using std::pair;
using std::map;
using std::vector;


/*
 * render colony on map
 * render map road
 * order units: arrows, R, B, P
 * render disabled unit shield
 * click to enter colony
 * button to exit colony
 * space for menu
 * 
 * 
 */

namespace col{

	enum PlayerType{
		HUMAN,
		AI
	};
	
	PlayerType get_player_type(const Player &p) {
		if (p.id == 0) {
			return HUMAN;
		}
		else {
			return AI;
		}
	}
	
	
	// ctrl - console
	// model - env
	// view - renderer

	// console: INPUT -> model
	// renderer: model,console -> OUTPUT
	// 
	
	void handle_events(sf::RenderWindow &app, Console &con) {
		sf::Event ev;
		while (app.pollEvent(ev)) {
			if (ev.type == sf::Event::KeyPressed) {
				if (ev.key.code == sf::Keyboard::Escape) {
					app.close();
				}				
			}
			else
			if (ev.type == sf::Event::Closed) {
				app.close();
			}

			con.handle(app, ev);
		}
	}

}






int main()
{
	using namespace col;
	
	string fname("./aaa.mp");
	
	const uint SCL = 3;
	sf::RenderWindow app(sf::VideoMode(320 * SCL, 200 * SCL, 32), "SFML Window");
	sf::View view(sf::FloatRect(0, 0, 320, 200));
	app.setView(view);
	
	
	
	//cout << "Loading terr types...";
	//tts = load_terr_types();
	//cout << " " << tts.size() << " loaded." << endl;

		
	EnvGame env;

	env.loads<TerrType>("./col94/terrs.csv");
	env.loads<BuildType>("./col94/builds.csv");
	env.loads<UnitType>("./col94/units.csv");
	
	//ifstream f(fname, std::ios::binary);
	// 
	//f.close();	
    
	env.resize({8,8});
	env.fill(Terr{Biome::Plains, Phys::None});
	
	Console con(env);
		
	auto last_env = env.mod - 1;
	auto last_con = con.mod - 1;

	while (app.isOpen())
	{
		
		
		if ((env.mod != last_env) || (con.mod != last_con)) {
			//cout << "RENDER:" << con.mod << ',' << env.mod << endl;
			
			render(app, env, con);		
			
			last_env = env.mod;
			last_con = con.mod;		
		}
		
		handle_events(app, con);
		
	}

	/*{
		ofstream fo(fname, std::ios::binary);
		io::write<Env>(fo, env);
	}*/
		
	return EXIT_SUCCESS;
}

