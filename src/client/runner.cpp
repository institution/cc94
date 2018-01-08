#include "runner.hpp"

#include "col.hpp"
#include "env.hpp"
#include "csv.hpp"
#include "console.hpp"
#include "client/serialize2.hpp"
#include "../ext/format.hpp"


namespace col{
	
	Runner::Agents::size_type const Runner::max_size = 32;



	void Runner::init(std::string const& file_map, std::string const& dir_tile, int ver) 
	{
		this->verbose = ver;

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

		env.loads<col::BuildType>((conf.csv_path/"builds.csv").c_str());
		env.loads<col::UnitType>((conf.csv_path/"units.csv").c_str());

		(*this).add_agent<col::Console>(env, this);

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
	
	}
	
	
	
	
	bool Runner::step() {
		
		if (verbose >= 2) {
			print("Runner.step: size=%||\n", agents.size());
		}
		
		bool cont = true;
		
		uint32_t t0 = 0, t1 = 0, dt = 0;
		
		// run all agents
		// one loop should take 1sec
		t0 = front::get_ticks();
		for (Agents::size_type i = 0; i < agents.size(); ++i) 
		{
			Agent *agent = agents[i];
					
			auto r = agent->step();			
			if (!r and i == 0) {
				cont = false;
				break;
			}
			
		}
		t1 = front::get_ticks();
		dt = t1 - t0;
	
		if (dt < 1000) 
		{
			// wait for events 
		}
			
		cont = (cont and (env.get_state() != Env::StateExit));
		
		if (!cont and verbose >= 1) {
			print("Runner.step: cont=%||\n", cont);
		}
		
		return cont;
	
	}
	
	
}
