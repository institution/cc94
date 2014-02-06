#include "env.h"

namespace col{
	




	UnitTypes load_unit_types() {
		UnitTypes itypes;

		auto vss = read_conf("./col94/unit-types.csv");
		auto p = vss.begin();
		auto e = vss.end();
		
		++p; // skip header
		while (p != e) {
			cout << "LOAD UNIT TYPES: " << (*p).size() << endl;
			if ((*p).size() > 1) {
				UnitType ict = UnitType(*p);

				itypes[ict.id] = ict;

				//for (auto &v: vs) {
				//	cout << v << '|';
				//}
				//cout << endl;
			}
			
			++p;
		}

		return itypes;
	}
	
	namespace io {

		template <>
		Icon read_obj<Icon>(Env &env, istream &f) {
			Icon u;
			read(u.id, f);
			u.type = read_ptr(env.uts, f);
			read(u.x, f);
			read(u.y, f);
			u.player = read_ptr(env.players, f);
			return u;
		}
		
		template <>
		size_t write_obj<Icon>(ostream &f, Icon const &icon) {
			size_t l = 0;
			l += write(f, icon.id);
			l += write(f, icon.type->id);
			l += write(f, icon.x);
			l += write(f, icon.y);
			l += write(f, icon.player->id);
			return l;
		}
		
		template <>
		Player read_obj<Player>(Env &env, istream &f) {
			Player player;
			read(player.id, f);
			read(player.name, f);
			read(player.color, f);
			return player;
		}

		template <>
		size_t write_obj<Player>(ostream &f, const Player &player) {
			size_t l = 0;
			l += write(f, player.id);
			l += write(f, player.name);
			l += write(f, player.color);
			return l;
		}


		template <>
		Env read<Env>(istream &f) {
			Env env;
			auto &terrain = env.terr;

			string fid = "COLMAPB";
			string ind = read_chars(f, 7);
			assert(ind == fid);
			auto nsect = read<uint8>(f);

			if (nsect < 1) return env;
			// terrain
			read(env.w, f);
			read(env.h, f);
			auto w = env.w;
			auto h = env.h;			
			
			cout << format("map size: %1%, %2%\n") % w % h;
			terrain.resize(boost::extents[w][h]);

			for(uint j = 0; j<h; ++j) {
				for(uint i = 0; i<w; ++i) {
					read(terrain[i][j], f);
					cout << to_string(terrain[i][j]) << ',';
				}
				cout << endl;
			}
			
			if (nsect < 2) return env;
			// players
			auto nplayers = read<Player::Id>(f);
			cout << format("Players(%||):\n") % nplayers;
			for (uint i = 0; i < nplayers; ++i) {
				Player p(read_obj<Player>(env, f));
				env.add_player(p);
				cout << p << endl;
			}
			
			if (nsect < 3) return env;
			// icons	
			auto nunits = read<Icon::Id>(f); // num of units on map
			cout << format("Icons(%||)\n") % nunits;
			for (uint i = 0; i < nunits; ++i) {
				Icon icon(read_obj<Icon>(env, f));
				env.icons[icon.id] = icon;
				cout << icon << endl;
			}

			if (nsect < 4) return env;
			// turn info
			read(env.turn_no, f);
			read(env.curr_player, f);
			cout << format("turn_no = %||\n") % env.turn_no;
			cout << format("curr_player = %||\n") % env.curr_player;


			return env;
		}




		template <>
		size_t write<Env>(ostream &f, const Env &env) {
			size_t l = 0;
			auto &terrain = env.terr;
			auto w = env.w;
			auto h = env.h;

			uint8 nsect = 4;
			string fid = "COLMAPB";

			l += write_chars(f, fid);
			l += write(f, nsect);

			// terrain
			l += write(f, env.w);
			l += write(f, env.h);

			for(uint j = 0; j < h; ++j) {
				for(uint i = 0; i < w; ++i) {
					l += write(f, terrain[i][j]);
				}
			}
			
			// players			
			l += write<Player::Id>(f, env.players.size());
			for (auto &p: env.players) {
				l += write_obj<Player>(f, p.second);
			}
			
			// icons
			l += write<Icon::Id>(f, env.icons.size());
			for (auto &icon: env.icons) {
				l += write_obj<Icon>(f, icon.second);
			}

			// turn info
			l += write(f, env.turn_no);
			l += write(f, env.curr_player);


			//res(ICON, 100)
			return l;
		}
	}
}