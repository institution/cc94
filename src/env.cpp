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
		void read<Terr>(Terr& t, istream &f) {
			size_t l = 0;
			read(t.biome, f);
			read(t.phys, f);
		}
		
		template <>
		size_t write<Terr>(ostream &f, Terr const& t) {
			size_t l = 0;
			l += write(f, t.biome);
			l += write(f, t.phys);
			return l;			
		}
		
		template <>
		Icon read_obj<Icon>(Env &env, istream &f) {
			Icon u;
			read(u.id, f);
			u.type = (UnitType const*)read_ptr(*env.uts, f);
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

		


		
	}
	
	
	
}