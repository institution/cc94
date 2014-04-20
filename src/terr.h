#ifndef TERR_H
#define TERR_H

#include "objs.h"
#include "colony.h"

namespace col{
	
	
	struct TerrType{
		using Id = uint8;
		
		string name;
		Id id;
		uint8 movement_cost;
		uint8 movement_type;
		uint8 defensive;
		
		
		TerrType() {}
		
		TerrType(vector<string> const& xs) {
			assert(xs.size() >= 16);
			
			name = trim_copy(xs[0]);
			id = stoi(xs[1]);
			movement_cost = stoi(xs[2]) * SPACE_UNIT;
			defensive = stoi(xs[3]);			
			movement_type = stoi(xs[15]);
		}
				
	};
	
	struct Terr: Place, Workplace {
		using Id = Coords;
		
		Biome biome;
		Phys phys;
		
		vector<Unit*> units;
		Colony* colony;
		
		Terr(): colony(nullptr) {}
		
		explicit Terr(Biome const& biome, Phys const& phys = Phys::None): 
			biome(biome), phys(phys), colony(nullptr) 
		{}
		
		// Place
		
		PlaceType place_type() {
			return PlaceType::Terr;
		}
		
		// Workplace 
		
		uint16 get_yield(Item const& item, bool const& is_expert) const {
			return 3;
		}
		
		bool assign() {
			if (has(Phys::Worker)) {
				return false;
			}
			add(Phys::Worker);
			return true;
		}
			
		bool leave() {
			if (!has(Phys::Worker)) {
				return false;
			}
			sub(Phys::Worker);
			return true;
		}
		
		// Terr
		
		Colony& get_colony() {
			assert(colony != nullptr);
			return *colony;
		}
		
		Terr& set_colony(Colony &c) {
			colony = &c;
			return *this;
		}
		
		bool has(Phys const& p) const {
			return flag(phys) & flag(p);
		}
		
		void add(Phys const& p) {
			phys = static_cast<Phys>(flag(phys) | flag(p));
		}
		
		void sub(Phys const& p) {
			phys = static_cast<Phys>(flag(phys) & !flag(p));
		}
				
		Travel get_travel();
		
		int8 get_movement_cost(Travel const& movement_type);
		
		int8 get_defensive_value(Travel const& movement_type);
		
		// max speed
		
		int8 get_land_movement_cost();		
		int8 get_naval_movement_cost();		
		int8 get_land_defensive_value();		
		int8 get_naval_defensive_value();		
	};
	
	

	
	
}
#endif
