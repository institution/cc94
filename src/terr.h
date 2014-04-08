#ifndef TERR_H
#define TERR_H

#include "objs.h"

namespace col{
	
	struct Terr {
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
