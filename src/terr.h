#ifndef TERR_H
#define TERR_H

#include "objs.h"

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
			movement_cost = stoi(xs[2]) * UNIT_OF_MOVEMENT;
			defensive = stoi(xs[3]);			
			movement_type = stoi(xs[15]);
		}
				
	};
	
	struct Terr: Place {
		using Id = Coords;
		
		uint8 biome;
		uint8 phys;
		
		
		bool has(Phys const& p) const {
			return phys & p;
		}
		
		MovementType get_movement_type();
		int8 get_movement_cost(MovementType const& movement_type);
		
		int8 get_defensive_value(MovementType const& movement_type);
		
		// max speed
		
		int8 get_land_movement_cost();		
		int8 get_naval_movement_cost();		
		int8 get_land_defensive_value();		
		int8 get_naval_defensive_value();		
	};
	
	

	
	
}
#endif
