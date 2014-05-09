#include "terr.h"

namespace col{

	int8 Terr::get_land_movement_cost() {
		int8 base;

		if (has(Phys::Road) or has(Phys::MinorRiver) or has(Phys::MajorRiver)) {
			base = SPACE_UNIT / 3;
		}
		else {
			switch (biome) {
				case Biome::Tundra:
				case Biome::Prairie:
				case Biome::Plains:
				case Biome::Desert:
				case Biome::Savannah:
				case Biome::Grassland:
					base = SPACE_UNIT;
					break;
				case Biome::Marsh:
				case Biome::Swamp:
				case Biome::Arctic:
					base = SPACE_UNIT * 3 / 2;
					break;
				default:
					cout << "invalid land biome: " << uint16(biome) << endl;
					throw runtime_error("invalid land biome");
			}

			if (has(Phys::Forest)) {
				base *= 2;
			}
		}

		if (has(Phys::Hill)) {
			base += SPACE_UNIT / 2;
		}
		else
		if (has(Phys::Mountain)) {
			base += SPACE_UNIT;
		}

		return base;
	}

	int8 Terr::get_naval_movement_cost() {
		if (colony != nullptr) {
			return SPACE_UNIT;
		}
		else
		if (has(Phys::MajorRiver) and !has(Phys::Hill) and !has(Phys::Mountain)) {
			return SPACE_UNIT * 2;
		}
		else {
			switch (biome) {
				case Biome::Ocean:
				case Biome::SeaLane:
					return SPACE_UNIT;
				default:
					throw runtime_error("invalid sea biome");
			}
		}

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
		if (biome == Biome::Ocean or biome == Biome::SeaLane) {
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