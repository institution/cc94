#include "view_base.h"



using std::unordered_map;

namespace col {
	unordered_map<uint8,string> BIOME_NAMES{
		{Biome::Tundra, "tundra"},
		{Biome::Prairie, "prairie"},
		{Biome::Plains, "plains"},
		{Biome::Desert, "desert"},
		{Biome::Savannah, "savannah"},
		{Biome::Grassland, "grassland"},
		{Biome::Marsh, "marsh"},
		{Biome::Swamp, "swamp"},
		{Biome::Arctic, "arctic"},
		{Biome::Ocean, "ocean"},
		{Biome::SeaLane, "sealane"},
	};

	Biome::type get_biome_by_name(string const& name) {
		for (auto& item: BIOME_NAMES) {
			if (item.second == name) {
				return item.first;
			}
		}
		return Biome::None;
	}
	
	unordered_map<Phys::type, string> PHYS_NAMES {
		{Phys::Hill, "hill"},
		{Phys::Mountain, "mountain"},
		{Phys::Road, "road"},
		{Phys::Forest, "forest"},
		{Phys::MinorRiver, "minorriver"},
		{Phys::MajorRiver, "majorriver"},
		{Phys::Plow, "plow"}
	};

	Phys::type get_phys_by_name(string const& name) {
		for (auto& item: PHYS_NAMES) {
			if (item.second == name) {
				return item.first;
			}
		}
		return Phys::None;
	}
	
	unordered_map<Item, string> ITEM_NAMES {
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
	};

	Item get_item_by_name(string const& name) {
		for (auto& item: ITEM_NAMES) {
			if (item.second == name) {
				return item.first;
			}
		}
		return Phys::None;
	}
	
	
	
	
	
}