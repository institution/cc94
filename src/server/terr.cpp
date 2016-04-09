#include "terr.hpp"
#include "item.hpp"

namespace col{



	
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
	
	Amount Terr::get_yield(Item const& item, Amount const& base) const {
		/* 		 
		    Why this is so complicated; games are supposed to have easy to understand rules
		  ballance should be achieved otherwise (negative feedback possibly)
		 TODO: simplify in AC spirit; yields should be devised from climate parameters; 			
		*/
		int ret = 0;
		int forest = has_phys(PhysForest);
		int plow = has_phys(PhysPlow) * 2;
		switch(item) {
			case ItemFish:
				if (alt == AltSea) {
					ret = 4;
				}
				break;
			case ItemLumber:
				if (alt >= AltFlat) {
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
				}
				break;
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
							ret = 5 - forest + plow;
							break;
						case BiomeSavannah:
							ret = 4 - forest + plow;
							break;
						case BiomeTundra:
						case BiomePrairie:
						case BiomeGrassland:
						case BiomeMarsh:
						case BiomeSwamp:
							ret = 3 - forest + plow;
							break;
						case BiomeDesert:
							ret = 2 - forest + plow;
							break;
					}
				}
				else if (alt == AltHill) {
					ret = 2 + plow;
				}
				break;
			case ItemCotton:
				if (alt == AltFlat) {
					switch (biome) {
						case BiomePrairie:
							ret = 3 - forest + plow;
							break;
						case BiomePlains:
							ret = 2 - forest + plow;
							break;
						case BiomeDesert:
							ret = 1 - forest + plow;
							break;
					}
				}
				break;
			case ItemTobacco:
				if (alt == AltFlat) {
					switch (biome) {
						case BiomeGrassland:
							ret = 3 - forest + plow;
							break;
						case BiomeMarsh:
							ret = 2 - forest + plow;
							break;
					}
				}
				break;
			case ItemSugar:
				if (alt == AltFlat) {
					switch (biome) {
						case BiomeSavannah:
							ret = 3 - forest + plow;
							break;
						case BiomeSwamp:
							ret = 2 - forest + plow;
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
		return ret * base;
	}



}
