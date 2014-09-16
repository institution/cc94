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



#include "col.hpp"
#include "envgame.h"
#include "csv.h"
#include "console.h"



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

using Threads = vector<std::thread>;

bool running{true};

#include <X11/Xlib.h>  // XInitThreads


int main(int argc, char* argv[])
{
	XInitThreads();
	
	string const CSV_PATH = "../col94/";

	

	string fname("./aaa.mp");
	
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


	EnvGame env(1);
	env.loads<BuildType>(CSV_PATH + "builds.csv");
	env.loads<UnitType>(CSV_PATH + "units.csv");

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
	
	
	
	Threads ths;
	
	run_console(&env, &ths, &running);
	//std::thread(run_console, &env, &ths, &running);
	
	env.activate_all();
	
	for (auto& t: ths) {
		t.join();
	}

	
	/*{
		ofstream fo(fname, std::ios::binary);
		io::write<Env>(fo, env);
	}*/

	return EXIT_SUCCESS;
}

