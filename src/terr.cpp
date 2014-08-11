#include "terr.h"
#include "item.h"

namespace col{

	int Terr::get_roughness() const {
		
		int has_river = has(PhysMinorRiver) or has(PhysMajorRiver);
		int has_forest = has(PhysForest);
		int has_road = has(PhysRoad);
		
		int base = TIME_UNIT;
		switch (biome) {					
			case BiomePrairie:
			case BiomePlains:
			case BiomeSavannah:
			case BiomeGrassland:				
			case BiomeTundra:
			case BiomeDesert:
				base = TIME_UNIT;
				break;
			case BiomeMarsh:
			case BiomeSwamp:
			case BiomeArctic:
				base = TIME_UNIT * 2;
				break;				
			default:
				cout << "invalid land biome: " << uint16(biome) << endl;
				throw runtime_error("invalid land biome");
		}
		
		if (has_forest) {
			base += TIME_UNIT;
		}
		
		if (has_river) {
			base += TIME_UNIT;
		}		
				
		if (alt == AltHill) {
			base += TIME_UNIT;
		}
		else if (alt == AltMountain) {
			base += TIME_UNIT*2;
		}
		else if (alt == AltSea) {
			base = TIME_UNIT;
		}
		
		return base;
	}
	
	int Terr::get_movement_cost() const {
		if (has(PhysRoad) or has(PhysMinorRiver) or has(PhysMajorRiver)) {
			return TIME_UNIT / 3;
		}
		else {
			return int(0.67 * std::sqrt(float(get_roughness())/float(TIME_UNIT))*float(TIME_UNIT));
		}		
	}
	
	Travel Terr::get_travel() const {
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
		throw Error("Terr: Unknown travel type");
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
				if (alt == AltSea) {
					ret = 4;
				}
				break;
			case ItemLumber:
				if (alt == AltFlat) {
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
				if (alt > AltFlat) {
					ret = 4;
				}
				else if (alt == AltFlat) {
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
				if (alt == AltFlat) {
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
				if (alt == AltSea) {
					ret = 4;
				}
				else if (alt == AltFlat) {
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
				else if (alt == AltHill) {
					ret = 2;
				}
				break;
			case ItemCotton:
				if (alt == AltFlat) {
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
				if (alt == AltFlat) {
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
				if (alt == AltFlat) {
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
				if (alt == AltMountain) {
					ret = 1;
				}
				break;
		}
		return ret * (is_expert + 1);
	}



}