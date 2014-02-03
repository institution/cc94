#include "env.h"

namespace col{
	
	namespace io {


		template <>
		Env read<Env>(istream &f) {
			Env env;
			auto &terrain = env.terr;

			string fid = "COLMAPB";
			string ind = read_chars(f, 7);
			assert(ind == fid);
			auto version = read<uint8>(f);
			auto nsect = read<uint8>(f);

			if (nsect < 1) return env;
			// terrain
			auto w = read<uint16>(f);
			auto h = read<uint16>(f);
			env.w = w;
			env.h = h;

			cout << format("%1% %2%\n") % w % h;
			terrain.resize(boost::extents[w][h]);

			for(uint j = 0; j<h; ++j) {
				for(uint i = 0; i<w; ++i) {
					terrain[i][j] = read<uint8>(f);
				}
			}

			if (nsect < 2) return env;
			// icons	
			uint nunits = read<uint16>(f); // num of units on map
			cout << format("nunits = %1%\n") % nunits;
			for (uint i = 0; i < nunits; ++i) {
				env.add(read<Icon>(f));
			}

			if (nsect < 3) return env;
			// players
			uint nplayers = read<uint8>(f);
			cout << format("nplayers = %1%\n") % nunits;
			for (uint i = 0; i < nplayers; ++i) {
				env.add_player(read<Player>(f));
			}

			if (nsect < 4) return env;
			// turn info
			env.turn_no = read<uint32>(f);
			env.curr_player = read<uint16>(f);


			return env;
		}




		template <>
		size_t write<Env>(ostream &f, const Env &env) {
			size_t l = 0;
			auto &terrain = env.terr;
			uint w = env.w;
			uint h = env.h;


			uint8 version = 1;
			uint8 nsect = 3;
			string fid = "COLMAPB";

			l += write_chars(f, fid);
			l += write<uint8>(f, version);
			l += write<uint8>(f, nsect);

			// terrain
			l += write<uint16>(f, w);
			l += write<uint16>(f, h);

			for(uint j = 0; j < h; ++j) {
				for(uint i = 0; i < w; ++i) {
					l += write<uint8>(f, terrain[i][j]);
				}
			}

			// icons
			uint nunits = env.icons.size();
			l += write<uint16>(f, nunits); // num of units on map
			for (auto &icon: env.icons) {
				l += write<Icon>(f, icon.second);
			}

			// players
			uint np = env.players.size();
			l += write<uint8>(f, np);
			for (auto &p: env.players) {
				l += write<Player>(f, p.second);
			}

			// turn info
			l += write<uint32>(f, env.turn_no);
			l += write<uint16>(f, env.curr_player);


			//res(ICON, 100)
			return l;
		}
	}
}