#include "terr.h"

namespace col{
	
	int8 Terr::get_land_movement_cost() {
		int8 base;

		if (has(PHYS_ROAD) or has(PHYS_MINOR_RIVER) or has(PHYS_MAJOR_RIVER)) {
			base = 4;
		}
		else {
			switch (biome) {
				case BIOME_TUNDRA:
				case BIOME_PRAIRIE:
				case BIOME_PLAINS:
				case BIOME_DESERT:
				case BIOME_SAVANNAH:
				case BIOME_GRASSLAND:
					base = 12;
					break;
				case BIOME_MARSH:
				case BIOME_SWAMP:
				case BIOME_ARCTIC:
					base = 18;
					break;
				default:
					cout << "invalid land biome: " << uint16(biome) << endl;
					throw runtime_error("invalid land biome");
			}

			if (has(PHYS_FOREST)) {
				base *= 2;
			}
		}			

		if (has(PHYS_HILL)) {
			base += 6.0;
		}
		else
		if (has(PHYS_MOUNTAIN)) {
			base += 12.0;
		}

		return base;
	}

	int8 Terr::get_naval_movement_cost() {			
		if (has(PHYS_DOCKS)) {
			return 12;
		}
		else 
		if (has(PHYS_MAJOR_RIVER) and !has(PHYS_HILL) and !has(PHYS_MOUNTAIN)) {
			return 24;
		}
		else {
			switch (biome) {
				case BIOME_OCEAN:
				case BIOME_SEA_LANE:
					return 12;
				default:
					throw runtime_error("invalid sea biome");
			}
		}

	}

	int8 Terr::get_movement_cost(MovementType const& movement_type) {
		if (movement_type == MOVEMENT_TYPE_LAND) {
			return get_land_movement_cost();
		}
		else
		if (movement_type == MOVEMENT_TYPE_NAVAL) {
			return get_naval_movement_cost();
		}
		else {
			throw runtime_error("aaa");
		}
	}

	uint8 Terr::get_movement_type() {
		if (biome == BIOME_OCEAN or biome == BIOME_SEA_LANE) {
			return MOVEMENT_TYPE_NAVAL;
		}
		else {
			if (has(PHYS_DOCKS)) {
				return MOVEMENT_TYPE_NAVAL | MOVEMENT_TYPE_LAND;
			}
			else {
				return MOVEMENT_TYPE_LAND;
			}				
		}					
	}

	int8 Terr::get_land_defensive_value() {
		return 1;

	}

	int8 Terr::get_naval_defensive_value() {
		return 1;			
	}

	int8 Terr::get_defensive_value(MovementType const& movement_type) {
		if (movement_type == MOVEMENT_TYPE_LAND) {
			return get_land_defensive_value();
		}
		else
		if (movement_type == MOVEMENT_TYPE_NAVAL) {
			return get_naval_defensive_value();
		}
		else {
			throw runtime_error("aaa");
		}
	}	

	
}