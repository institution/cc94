#include "props.hpp"



using std::unordered_map;

namespace col {
	
	//tile_dim = v2i(conf.tile_dim, conf.tile_dim);
	

		
		
	unordered_map<Biome,string> BIOME_NAMES{
		{BiomeNone, "unknown"},
		{BiomeTundra, "tundra"},
		{BiomePrairie, "prairie"},
		{BiomePlains, "plains"},
		{BiomeDesert, "desert"},
		{BiomeSavannah, "savannah"},
		{BiomeGrassland, "grassland"},
		{BiomeMarsh, "marsh"},
		{BiomeSwamp, "swamp"},
		{BiomeArctic, "arctic"},
	};

	Biome get_biome_by_name(string const& name) {
		for (auto& item: BIOME_NAMES) {
			if (item.second == name) {
				return item.first;
			}
		}
		return BiomeNone;
	}
	
	unordered_map<Phys, string> PHYS_NAMES {
		{PhysSeaLane, "sealane"},
		{PhysRoad, "road"},
		{PhysForest, "forest"},
		{PhysMinorRiver, "minorriver"},
		{PhysMajorRiver, "majorriver"},
		{PhysPlow, "plow"}
	};

	Phys get_phys_by_name(string const& name) {
		for (auto& item: PHYS_NAMES) {
			if (item.second == name) {
				return item.first;
			}
		}
		return PhysNone;
	}
	
	/*unordered_map<Item, string> ITEM_NAMES {
		{ItemFood, "food"},
		{ItemSugar, "sugar"},
		{ItemTobacco, "tobacco"},
		{ItemCotton, "cotton"},
		{ItemFurs, "furs"},
		{ItemLumber, "lumber"},
		{ItemOre, "ore"},
		{ItemSilver, "silver"},
		{ItemHorses, "horses"},
		{ItemRum, "rum"},
		{ItemCigars, "cigars"},
		{ItemCloth, "cloth"},
		{ItemCoats, "coats"},
		{ItemTradeGoods, "tradegoods"},
		{ItemTools, "tools"},
		{ItemMuskets, "muskets"},
		{ItemHammers, "hammers"},
		{ItemCross, "cross"},		
		{ItemFish, "fish"},
		{ItemBell, "bell"}
	};*/

	/*Item get_item_by_name(string const& name) {
		for (auto& item: ITEM_NAMES) {
			if (item.second == name) {
				return item.first;
			}
		}
		return ItemNone;
	}*/
	
	string const NONE("none");
	
	/*string const& get_item_name(Item const& item) {
		auto it = ITEM_NAMES.find(item);
		if (it != ITEM_NAMES.end()) {
			return it->second;
		}
		return NONE;		
	}*/
	
	
	
}
