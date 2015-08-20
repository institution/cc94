#include "game.h"

#include "col.hpp"
#include "env.h"
#include "csv.h"
#include "game.h"
#include "console.h"
#include "server/serialize.h"
#include "format.hpp"


namespace col{
	
	void Game::init(std::string const& fname, int ver) {
		this->verbose = ver;
		
		Path csv_path = "res/csv/";	
		conf.res_path = "res/tile48";
		conf.font_path = "res/font";
		
		if (verbose >= 1) {
			print("csv_path=%||\n", csv_path);
			print("res_path=%||\n", conf.res_path);
			print("font_path=%||\n", conf.font_path);
		}

		env.loads<col::BuildType>((csv_path/"builds.csv").c_str());
		env.loads<col::UnitType>((csv_path/"units.csv").c_str());

		// load state from file
		if (fname != "") {
			read_file(fname, env);
		}
		else {
			env.resize({15,12});
			//env.fill(Terr{AltSea, BiomeDesert});
			env.fill(col::Terr{col::AltSea, col::BiomeDesert});
		}
		
		(*this).add_player<col::Console>(0, env, *this);	
				
	}
		
	
	void Game::connect(Player * p, Nation::Id nation_id) {
		if (verbose >= 1) {
			print("Game.connect: nation_id=%||; player_ptr=%||\n", nation_id, p);
		}
			
		for (auto& np: who) {
			if (np.first == nation_id) {
				np.second = p;
		
				if (verbose >= 2) {
					print("Game.connect: adding at existing location\n");
				}
		
				return;
			}
		}			
		// else
		
		if (verbose >= 2) {
			print("Game.connect: adding new\n");
		}
		
		who.push_back(std::pair<Nation::Id, Player*>(nation_id, p));
		
		p->start();
	}

	bool Game::step() {
		
		if (verbose >= 2) {
			print("Game.step: for who; size=%||\n", who.size());
		}
			
		// Who::size_type i = 0; i < who.size(); ++i
		for (auto const& np: who) {
			
			if (verbose >= 2) {
				print("Game.step: for inner\n");
			}
		
			//auto const& np = who.at(i);
		
			// what about connect during iteration?
			
			Nation::Id nation_id = np.first;
			Player * player = np.second;
			
			if (verbose >= 2) {
				print("Game.step: player.step; nation_id=%||;\n", nation_id);
			}
				
			if (nation_id != 0) {
				
				if (verbose >= 2) {
					print("Game.step: nation_id != 0;\n");
				}
				
				//player->step(env, nation_id, env.get<Nation>(nation_id).auth);
				player->step(env, 0, 0);
			}
			else {
				
				if (verbose >= 2) {
					print("Game.step: nation_id == 0;\n");
				}
										
				player->step(env, nation_id, 0);
			}
			
			
			
		} // for
		
			
		bool cont = (env.get_state() != Env::StateExit);
		
		if (verbose >= 2) {
			print("Game.step: cont=%||\n", cont);
		}
		
		return cont;
	
	}
	
	
}
