#include "env.h"


namespace col {

	
	Env::State const 
		Env::StateEdit = 0,
		Env::StatePlay = 1,
		Env::StateExit = 2;

	
	int Env::get_movement_cost(Terr const& dest, Terr const& orig, Travel const& travel) const {
		
		if (travel == TravelSea) {
			if (dest.alt == AltSea) {
				return TIME_UNIT;
			}
			else if (dest.has_colony() and !orig.has_colony()) {
				return TIME_UNIT;
			}
			else {
				return -1;
			}			
		}
		else if (travel == TravelLand) {
			if (dest.alt != AltSea) {
				if (dest.has_phys(PhysRoad) and orig.has_phys(PhysRoad)) {
					return TIME_UNIT / 3;
				}
				else if (dest.has_phys(PhysRiver) and orig.has_phys(PhysRiver)) {
					return TIME_UNIT / 3;
				}
				else {
					return TIME_UNIT * get_roughness(dest);
				}
			}
			else {
				return -1;
			}
		}
		else {
			throw Critical("Unknown travel type");
		}
	}
	
	int Env::get_roughness(Terr const& t) const {
		// return -- 1 or 2
		auto alt = t.get_alt();
		auto biome = t.get_biome();
		
		if (alt == AltHill or alt == AltMountain) {
			return 2;
		}
		else if (biome == BiomeSwamp or biome == BiomeMarsh or biome == BiomeArctic) {
			return 2;
		}
		else if (t.has_phys(PhysForest) or t.has_phys(PhysRiver)) {
			return 2;
		}		
		else {
			return 1;
		}
		
	}
	
	
}
	
	
