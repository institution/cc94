#include "runner.hpp"

#include "col.hpp"
#include "env.hpp"
#include "csv.hpp"
#include "console.hpp"
#include "client/serialize2.hpp"
#include "../ext/format.hpp"
#include "serialize3.hpp"



namespace col{
	




	void Runner::run(std::string const& file_map, std::string const& dir_tile, int ver) 
	{
		auto verbose = ver;

		// read client conf
		if (dir_tile == "") {
			conf.read("res/tile64");
		}
		else {
			conf.read(dir_tile);
		}

		// recalc layout
		ly.recalc(conf.screen_w, conf.screen_h);
		
		if (verbose >= 1) {
			print("csv_path=%||\n", conf.csv_path);
			print("tile_path=%||\n", conf.tile_path);			
		}


		// add master agent
		this->add_agent<col::Console>(env, this);
		

		// load
		RCParser p("res/csv/defs.rc");
		read_defs(p, env);
		p.close();
			

		// load state from file
		if (file_map == "-") {
			read_file("res/default.mp", *this);
		}
		else if (file_map == "") {
			env.resize({15,12});
			env.fill(col::Terr{col::AltSea, col::BiomeDesert});
		}
		else {
			read_file(file_map, *this);
		}

		// wait for agents
		threads.at(0).join();
		f_stop = true;		
	}
	
	
	
	
	
}
