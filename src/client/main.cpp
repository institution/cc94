
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <execinfo.h>
#include <signal.h>
#include <string.h>

#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>
#include <vector>
#include <map>
#include <thread>

#include "pre.h"

#include <boost/cstdint.hpp>
#include <boost/multi_array.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/filesystem.hpp>



#include "col.hpp"
#include "env.h"
#include "csv.h"
#include "game.h"
#include "console.h"

#include "format.hpp"
#include "osdep.h"



namespace filesys = boost::filesystem;


using std::unique_ptr;

using std::ifstream;
using std::ofstream;
using std::string;
using std::add_pointer;
using std::to_string;

using std::cout;
using std::endl;

using std::pair;
using std::map;
using std::vector;

using Path = filesys::path;


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
/*
namespace col{

	enum NationType{
		HUMAN,
		AI
	};

	NationType get_nation_type(const Nation &p) {
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


}
*/








int main(int argc, char* argv[])
{
	
	
	//XInitThreads(); // os dep	

	
	Path argv0(argv[0]);
	Path bin_path;
	if (argv0.is_absolute()) {
		bin_path = filesys::canonical(argv0.parent_path(), "/");
	}
	else {
		bin_path = filesys::canonical(filesys::current_path() / argv0.parent_path(), "/"); 
	}
	
	Path csv_path = filesys::canonical(bin_path / "../../col94/", "/");;
	
	col::conf.res_path = filesys::canonical(os::get_home_dir() / Path(".cc94/COLONIZE"), "/");
	col::conf.font_path = filesys::canonical(bin_path / Path("../../font"), "/");;
		
	print("csv_path=%||\n", csv_path);
	print("res_path=%||\n", col::conf.res_path);
	print("font_path=%||\n", col::conf.font_path);

	// handle options
	/*{
	 	namespace po = boost::program_options;

		// declare options
		po::options_description generic("Allowed options");
		generic.add_options()
			("help,h", "produce help message")
			("version,v", "print version string")		
		;

		
		po::options_description positional;
		positional.add_options()
			("savefile,f", po::value< vector<string> >(), "load savefile")		
		;

		po::positional_options_description positional_mark;
		positional_mark.add("savefile", 1);

		// parse
		po::variables_map params;
		po::store(
			po::command_line_parser(argc, argv)
				.options(generic)
				.positional(positional_mark)
				.run(), 
			params
		);
		po::notify(params);

		// handle
		if (params.count("help")) {
			cout << generic << "\n";
			cout << positional << "\n";
			return 1;
		}

		if (params.count("savefile")) {
			fname = params["savefile"].as< vector<string> >().at(0);
		}
	}*/
	
	
	using namespace col;

	//cout << "Loading terr types...";
	//tts = load_terr_types();
	//cout << " " << tts.size() << " loaded." << endl;


	Env env(1);
	env.loads<BuildType>((csv_path/"builds.csv").c_str());
	env.loads<UnitType>((csv_path/"units.csv").c_str());

	// load state from file
	if (argc == 2) {
		ifstream f(argv[1], std::ios::binary);
		boost::archive::text_iarchive ar(f);
		ar >> env;
	}
	else {
		env.resize({15,12});
		//env.fill(Terr{AltSea, BiomeDesert});
		env.fill(Terr{AltSea, BiomeNone});
	}
	
	Game game(env);
	
	
	game.add_player<Console>(0, env, game);
	
	game.play();
	
	
	/*{
		ofstream fo(fname, std::ios::binary);
		io::write<Env>(fo, env);
	}*/

	return EXIT_SUCCESS;
}

