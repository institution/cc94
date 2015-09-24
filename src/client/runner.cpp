#include "runner.h"

#include "col.hpp"
#include "env.h"
#include "csv.h"
#include "console.h"
#include "server/serialize.h"
#include "format.hpp"
#include "gui.h"

namespace col{
	
	Runner::Agents::size_type const Runner::max_size = 16;
	
	void Runner::init(std::string const& fname, int ver) {
		this->verbose = ver;
		
		
		
		if (verbose >= 1) {
			print("csv_path=%||\n", conf.csv_path);
			print("tile_path=%||\n", conf.tile_path);			
		}

		env.loads<col::BuildType>((conf.csv_path/"builds.csv").c_str());
		env.loads<col::UnitType>((conf.csv_path/"units.csv").c_str());

		// load state from file
		if (fname == "") {
			read_file("res/default.mp", env);
		}
		else if (fname == "-") {
			env.resize({15,12});
			//env.fill(Terr{AltSea, BiomeDesert});
			env.fill(col::Terr{col::AltSea, col::BiomeDesert});
		}
		else {
			read_file(fname, env);
		}
		
		(*this).add_agent<col::Gui>(env, *this);	
				
	}
	

	bool Runner::step() {
		
		if (verbose >= 2) {
			print("Runner.step: size=%||\n", agents.size());
		}
		
		bool cont = true;
			
		for (Agents::size_type i = 0; i < agents.size(); ++i) {
			Agent *agent = agents[i];
			
			auto r = agent->step();			
			if (!r and i == 0) {				
				cont = false;
			}			
		} 
			
		cont = (cont and (env.get_state() != Env::StateExit));
		
		if (!cont and verbose >= 1) {
			print("Runner.step: cont=%||\n", cont);
		}
		
		return cont;
	
	}
	
	
}
