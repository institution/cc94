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
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "col.hpp"
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






int main(int argc, char* argv[])
{
	using namespace col;

	string fname("./aaa.mp");


	sf::RenderWindow app(sf::VideoMode(SCREEN_W * GLOBAL_SCALE, SCREEN_H * GLOBAL_SCALE, 32), "AI Col");
	sf::View view(sf::FloatRect(0, 0, SCREEN_W, SCREEN_H));
	app.setView(view);



	//cout << "Loading terr types...";
	//tts = load_terr_types();
	//cout << " " << tts.size() << " loaded." << endl;

	preload_terrain();

	EnvGame env;

	env.loads<TerrType>("./col94/terrs.csv");
	env.loads<BuildType>("./col94/builds.csv");
	env.loads<UnitType>("./col94/units.csv");

	//ifstream f(fname, std::ios::binary);
	//
	//f.close();

	if (argc == 2) {
		ifstream f(argv[1], std::ios::binary);
		boost::archive::text_iarchive ar(f);
		ar >> env;
	}
	else {
		env.resize({15,12});
		env.fill(Terr{Biome::Plains, Phys::None});
	}

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

