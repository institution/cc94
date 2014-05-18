#include "terr.h"

#include "item.h"


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
		if (alt == AltSea) {
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
	
	
	
	uint16 Terr::get_yield(Item const& item, bool const& is_expert) const {
		/* 		 
		    Why this is so complicated; games are supposed to have easy to understand rules
		  ballance should be achieved otherwise (negative feedback possibly)
		 TODO: simplify in AC spirit; yields should be devised from climate parameters; 			
		*/
		int ret = 0;
		int forest = has(PhysForest);
		switch(item) {
			case ItemFish:
				if (alt == SEA_LEVEL) {
					ret = 4;
				}
				break;
			case ItemLumber:
				if (alt == FLATLAND_LEVEL) {
					if (forest) {
						switch (biome) {
							case BiomePlains:
							case BiomeSavannah:
								ret = 6;
								break;
							case BiomeTundra:
							case BiomePrairie:
							case BiomeGrassland:
							case BiomeSwamp:
							case BiomeMarsh:
								ret = 4;
								break;
							case BiomeDesert:
								ret = 2;
								break;
						}
					}
					break;
				}
			case ItemOre:
				if (alt > FLATLAND_LEVEL) {
					ret = 4;
				}
				else if (alt == FLATLAND_LEVEL) {
					switch (biome) {
						case BiomeTundra:
						case BiomeSwamp:
						case BiomeMarsh:
						case BiomeDesert:
							ret = 2 - forest;
							break;
					}
				}
				break;
			case ItemFurs:
				if (alt == FLATLAND_LEVEL) {
					if (forest) {
						switch (biome) {
							case BiomeTundra:
							case BiomePlains:
								ret = 3;
								break;
							case BiomePrairie:
							case BiomeGrassland:
							case BiomeMarsh:
							case BiomeSavannah:
							case BiomeDesert:
							case BiomeArctic:
								ret = 2;
								break;
							case BiomeSwamp:
								ret = 1;
								break;
						}
					}
				}
				break;
			case ItemFood:
				if (alt == SEA_LEVEL) {
					ret = 4;
				}
				else if (alt == FLATLAND_LEVEL) {
					switch (biome) {
						case BiomePlains:
							ret = 5 - forest;
							break;
						case BiomeSavannah:
							ret = 4 - forest;
							break;
						case BiomeTundra:
						case BiomePrairie:
						case BiomeGrassland:
						case BiomeMarsh:
						case BiomeSwamp:
							ret = 3 - forest;
							break;
						case BiomeDesert:
							ret = 2 - forest;
							break;
					}
				}
				else if (alt == HILL_LEVEL) {
					ret = 2;
				}
				break;
			case ItemCotton:
				if (alt == FLATLAND_LEVEL) {
					switch (biome) {
						case BiomePrairie:
							ret = 3 - forest;
							break;
						case BiomePlains:
							ret = 2 - forest;
							break;
						case BiomeDesert:
							ret = 1 - forest;
							break;
					}
				}
				break;
			case ItemTobacco:
				if (alt == FLATLAND_LEVEL) {
					switch (biome) {
						case BiomeGrassland:
							ret = 3 - forest;
							break;
						case BiomeMarsh:
							ret = 2 - forest;
							break;
					}
				}
				break;
			case ItemSugar:
				if (alt == FLATLAND_LEVEL) {
					switch (biome) {
						case BiomeSavannah:
							ret = 3 - forest;
							break;
						case BiomeSwamp:
							ret = 2 - forest;
							break;
					}
				}
				break;
			case ItemSilver:
				if (alt == MOUNTAIN_LEVEL) {
					ret = 1;
				}
				break;
		}
		return ret * (is_expert + 1);
	}



}