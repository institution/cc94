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
	
	unordered_map<Item::type, string> ITEM_NAMES {
		{Item::Food, "food"},
		{Item::Sugar, "sugar"},
		{Item::Tobacco, "tobacco"},
		{Item::Cotton, "cotton"},
		{Item::Furs, "furs"},
		{Item::Lumber, "lumber"},
		{Item::Ore, "ore"},
		{Item::Silver, "silver"},
		{Item::Horses, "horses"},
		{Item::Rum, "rum"},
		{Item::Cigars, "cigars"},
		{Item::Cloth, "cloth"},
		{Item::Coats, "coats"},
		{Item::TradeGoods, "tradegoods"},
		{Item::Tools, "tools"},
		{Item::Muskets, "muskets"},
		{Item::Hammers, "hammers"},
		{Item::Cross, "cross"},		
		{Item::Fish, "fish"},
		{Item::Bell, "bell"}
	};

	Item::type get_item_by_name(string const& name) {
		for (auto& item: ITEM_NAMES) {
			if (item.second == name) {
				return item.first;
			}
		}
		return Phys::None;
	}
	
	
	
	
	
}