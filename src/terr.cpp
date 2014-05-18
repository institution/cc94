#include "terr.h"

namespace col{

	int8 Terr::get_land_movement_cost() {
		int8 base;

		if (has(PhysRoad) or has(PhysMinorRiver) or has(PhysMajorRiver)) {
			base = SPACE_UNIT / 3;
		}
		else {
			switch (biome) {
				case BiomeTundra:
				case BiomePrairie:
				case BiomePlains:
				case BiomeDesert:
				case BiomeSavannah:
				case BiomeGrassland:
					base = SPACE_UNIT;
					break;
				case BiomeMarsh:
				case BiomeSwamp:
				case BiomeArctic:
					base = SPACE_UNIT * 3 / 2;
					break;
				default:
					cout << "invalid land biome: " << uint16(biome) << endl;
					throw runtime_error("invalid land biome");
			}

			if (has(PhysForest)) {
				base *= 2;
			}
		}

		if (alt == HILL_LEVEL) {
			base += SPACE_UNIT / 2;
		}
		else
		if (alt == MOUNTAIN_LEVEL) {
			base += SPACE_UNIT;
		}

		return base;
	}

	int8 Terr::get_naval_movement_cost() {
		if (colony != nullptr) {
			return SPACE_UNIT;
		}
		else if (has(PhysMajorRiver) and alt == FLATLAND_LEVEL) {
			return SPACE_UNIT * 2;
		}
		else if (alt == SEA_LEVEL) {
			return SPACE_UNIT;				
		}
		throw runtime_error("incompatible travel type");

	}

	int8 Terr::get_movement_cost(Travel const& movement_type) {
		if (movement_type == LAND) {
			return get_land_movement_cost();
		}
		else
		if (movement_type == SEA) {
			return get_naval_movement_cost();
		}
		else {
			throw runtime_error("aaa");
		}
	}

	Travel Terr::get_travel() {
		if (alt == SEA_LEVEL) {
			return SEA;
		}
		else {
			if (colony != nullptr) {
				return SEA | LAND;
			}
			else {
				return LAND;
			}
		}
	}



}