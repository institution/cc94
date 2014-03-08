#include "env.h"

namespace col{
	


	TerrTypes load_terr_types() {
		TerrTypes tts;

		auto vss = read_conf("./col94/terrs.csv");
		auto p = vss.begin();
		auto e = vss.end();
		
		++p; // skip header
		while (p != e) {
			// cout << "LOAD TERR TYPES: " << (*p).size() << endl;
			if ((*p).size() > 1) {
				TerrType tt = TerrType(*p);

				tts[tt.id] = tt;

				//for (auto &v: *p) {
				//	cout << v << '|';
				//}
				//cout << endl;
			}			
			++p;
		}

		return tts;
	}

	UnitTypes load_unit_types() {
		UnitTypes itypes;

		auto vss = read_conf("./col94/icons.csv");
		auto p = vss.begin();
		auto e = vss.end();
		
		++p; // skip header
		while (p != e) {
			//cout << "LOAD UNIT TYPES: " << (*p).size() << endl;
			if ((*p).size() > 1) {
				UnitType ut = UnitType(*p);

				itypes[ut.id] = ut;

				//for (auto &v: *p) {
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
		void read<Coords>(Coords &cs, istream &f) {
			read(cs[0], f);
			read(cs[1], f);			
		}
			
		
		template <>
		size_t write<Coords>(ostream &f, Coords const& cs) {
			size_t l = 0;
			l += write(f, cs[0]);
			l += write(f, cs[1]);
			return l;			
		}
		
		
		
		template <>
		Icon read_obj<Icon>(Env &env, istream &f) {
			Icon u;
			read(u.id, f);
			u.type = read_ptr(env.uts, f);
			read(u.pos, f);
			u.player = read_ptr(env.players, f);
			return u;
		}
		
		
		
		
		template <>
		size_t write_obj<Icon>(ostream &f, Icon const &icon) {
			size_t l = 0;
			l += write(f, icon.id);
			l += write(f, icon.type->id);
			l += write(f, icon.pos);
			l += write(f, icon.player->id);
			return l;
		}
		
		template <>
		Player read_obj<Player>(Env &env, istream &f) {
			Player player;
			read(player.id, f);
			read(player.name, f);
			read(player.color, f);
			read(player.flag_id, f);
			return player;
		}

		
		
		template <>
		size_t write_obj<Player>(ostream &f, const Player &player) {
			size_t l = 0;
			l += write(f, player.id);
			l += write(f, player.name);
			l += write(f, player.color);
			l += write(f, player.flag_id);
			return l;
		}


		template <>
		Env read<Env>(istream &f) {
			Env env;
			auto &terrain = env.terrs;

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
			
			cout << format("Map size: %1%, %2%\n") % w % h;
			terrain.resize(boost::extents[h][w]);

			for(Coord j = 0; j < h; ++j) {
				for(Coord i = 0; i < w; ++i) {
					read(terrain[j][i], f);
					//cout << to_string(terrain[j][i]) << ',';
				}
				//cout << endl;
			}
			
			if (nsect < 2) return env;
			// players
			auto nplayers = read<Player::Id>(f);
			cout << format("Players(%||):\n") % nplayers;
			for (uint i = 0; i < nplayers; ++i) {
				Player p(read_obj<Player>(env, f));
				cout << "Player_read" << endl;
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
			if (env.players.size()) {
				env.curr_player = read_ptr(env.players, f);
			}
			else {
				Player::Id zero;
				read(zero, f);
				env.curr_player = nullptr;
			}
			
			cout << format("turn_no = %||\n") % env.turn_no;


			return env;
		}




		template <>
		size_t write<Env>(ostream &f, const Env &env) {
			size_t l = 0;
			auto &terrain = env.terrs;
			auto w = env.w;
			auto h = env.h;

			uint8 nsect = 4;
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
			
			// icons
			l += write<Icon::Id>(f, env.icons.size());
			for (auto &icon: env.icons) {
				l += write_obj<Icon>(f, icon.second);
			}

			// turn info
			l += write(f, env.turn_no);
			if (env.curr_player != nullptr) {
				l += write(f, env.curr_player->id);				
			}
			else {
				l += write(f, Player::Id(0));
			}


			//res(ICON, 100)
			return l;
		}
	}
}