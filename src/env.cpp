#include "env.h"



	
	
namespace col {
	namespace io {
		
		template <>
		size_t write<Biome>(ostream &f, const Biome &t) {
			return write(f, enum_value(t));
		}

		template <>
		size_t write<Phys>(ostream &f, const Phys &t) {
			return write(f, enum_value(t));
		}

		template <>
		void read<Biome>(Biome& t, istream& f) {
			t = static_cast<Biome>(read<uint8>(f));
		}

		template <>
		void read<Phys>(Phys& t, istream& f) {
			t = static_cast<Phys>(read<uint8>(f));
		}
		
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
		Unit read_obj<Unit>(Env &env, istream &f) {
			Unit u;
			read(u.id, f);
			u.type = (UnitType const*)read_ptr(*env.uts, f);
			read(u.pos, f);
			u.player = read_ptr(env.players, f);
			return u;
		}
		
		
		
		
		template <>
		size_t write_obj<Unit>(ostream &f, Unit const &unit) {
			size_t l = 0;
			l += write(f, unit.id);
			l += write(f, unit.type->id);
			l += write(f, unit.pos);
			l += write(f, unit.player->id);
			return l;
		}
		/*
		template <>
		void read_obj<Player>(Env &env, Player &player, istream &f) {
			read(player.id, f);
			read(player.name, f);
			read(player.color, f);
			read(player.flag_id, f);			
		}
		*/
		
		
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
	
	
