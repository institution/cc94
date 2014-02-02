#include "env.h"

namespace col{
	Env read_env(istream &f) {
		Env env;
		auto &terrain = env.terr;

		string fid = "COLMAPB";
		string ind = read_chars(f, 7);
		assert(ind == fid);
		auto version = read_uint8(f);
		auto nsect = read_uint8(f);

		if (nsect < 1) return env;
		// terrain
		auto w = read_uint16(f);
		auto h = read_uint16(f);
		env.w = w;
		env.h = h;

		cout << format("%1% %2%\n") % w % h;
		terrain.resize(boost::extents[w][h]);

		for(uint j = 0; j<h; ++j) {
			for(uint i = 0; i<w; ++i) {
				terrain[i][j] = read_byte(f);
			}
		}

		if (nsect < 2) return env;
		// icons	
		uint nunits = read_uint16(f); // num of units on map
		cout << format("nunits = %1%\n") % nunits;
		for (uint i = 0; i < nunits; ++i) {
			env.add(read_icon(f));
		}

		if (nsect < 3) return env;
		// players
		uint nplayers = read_uint8(f);
		cout << format("nplayers = %1%\n") % nunits;
		for (uint i = 0; i < nplayers; ++i) {
			env.players.push_back(read_player(f));
		}

		if (nsect < 4) return env;
		// turn info
		env.turn_no = read_uint32(f);
		env.curr_player = read_uint16(f);
				
		
		return env;
	}





	void write_env(ostream &f, const Env &env) {
		auto &terrain = env.terr;
		uint w = env.w;
		uint h = env.h;


		uint8 version = 1;
		uint8 nsect = 3;
		string fid = "COLMAPB";

		write_chars(f, fid);
		write_uint8(f, version);
		write_uint8(f, nsect);

		// terrain
		write_uint16(f, w);
		write_uint16(f, h);

		for(uint j = 0; j < h; ++j) {
			for(uint i = 0; i < w; ++i) {
				write_uint8(f, terrain[i][j]);
			}
		}

		// icons
		uint nunits = env.icons.size();
		write_uint16(f, nunits); // num of units on map
		for (auto &icon: env.icons) {
			write_icon(f, icon.second);
		}

		// players
		uint np = env.players.size();
		write_uint8(f, np);
		for (auto &p: env.players) {
			write_player(f, p);
		}

		// turn info
		write_uint32(f, env.turn_no);
		write_uint16(f, env.curr_player);
		
		
		//res(ICON, 100)

	}
	
}