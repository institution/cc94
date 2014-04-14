#include "envgame.h"

namespace col {
	
	
	namespace io{
		
		template<>
		void read<EnvGame>(EnvGame &env, istream &f) {
			auto &terrain = env.terrs;

			string fid = "COLMAPB";
			string ind = read_chars(f, 7);
			assert(ind == fid);
			auto nsect = read<uint8>(f);

			if (nsect < 1) return;
			// terrain
			read(env.w, f);
			read(env.h, f);
			auto w = env.w;
			auto h = env.h;			
			
			cout << format("Map size: %1%, %2%\n") % w % h;
			terrain.resize(boost::extents[h][w]);

			for(Coord j = 0; j < h; ++j) {
				for(Coord i = 0; i < w; ++i) {
					read(terrain[j][i], f);
					
					//cout << "Biome: " << terrain[j][i].biome << endl;
					//cout << to_string(terrain[j][i]) << ',';
				}
				//cout << endl;
			}
			
			if (nsect < 2) return;
			// players
			auto nplayers = read<Player::Id>(f);
			cout << format("Players(%||):\n") % nplayers;
			for (uint i = 0; i < nplayers; ++i) {
				Player p(read_obj<Player>(env, f));
				cout << "Player_read" << endl;
				env.add_player(p);
				cout << p << endl;
			}
			
			
			if (nsect < 3) return;
			// units
			auto nunits = read<Unit::Id>(f);    // num of units
			cout << format("Units(%||)\n") % nunits;
			for (uint i = 0; i < nunits; ++i) {
				Unit unit(read_obj<Unit>(env, f));
				env.units[unit.id] = unit;
				cout << unit << endl;
			}
			
			if (nsect < 4) return;
			// turn info
			read(env.turn_no, f);
			cout << format("turn_no = %||\n") % env.turn_no;

			if (nsect < 5) return;
			// current player
			read(env.current_player_id, f);
			cout << format("current_player_id = %||\n") % env.current_player_id;
			
		}

		
		template<>
		size_t write<EnvGame>(ostream &f, const EnvGame &env) {
			size_t l = 0;
			auto &terrain = env.terrs;
			auto w = env.w;
			auto h = env.h;

			uint8 nsect = 5;
			string fid = "COLMAPB";

			l += write_chars(f, fid);
			l += write(f, nsect);

			// terrain
			l += write(f, env.w);
			l += write(f, env.h);

			for(Coord j = 0; j < h; ++j) {
				for(Coord i = 0; i < w; ++i) {
					l += write(f, terrain[j][i]);
				}
			}
			
			// players			
			l += write<Player::Id>(f, env.players.size());
			for (auto &p: env.players) {
				l += write_obj<Player>(f, p.second);
			}
			
			// units
			l += write<Unit::Id>(f, env.units.size());
			for (auto &unit: env.units) {
				l += write_obj<Unit>(f, unit.second);
			}

			// turn info
			l += write(f, env.turn_no);
						
			// current player
			l += write(f, env.current_player_id);
			
			//res(ICON, 100)
			return l;
		}
		
	}
	
	
	void copy(EnvGame &trg, EnvGame const& src)
	{
		trg.uts = src.uts;
		
		std::stringstream buf;
		io::write<EnvGame>(buf, src);		
		io::read<EnvGame>(trg, buf);		
	}
	
	
}