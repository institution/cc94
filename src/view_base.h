#ifndef VIEW_BASE44_H
#define VIEW_BASE44_H
// ^renderer base

#include "base.h"

#include <unordered_map>


namespace col {
	extern std::unordered_map<uint8,string> BIOME_NAMES;
	Biome::type get_biome_by_name(string const& name);
	
	extern std::unordered_map<uint8,string> PHYS_NAMES;
	Phys::type get_phys_by_name(string const& name);
	
	extern std::unordered_map<uint8,string> ITEM_NAMES;
	Item::type get_item_by_name(string const& name);
	
	
	array<Item::type, 16> const COLONY_ITEMS = {
		Item::Food,Item::Sugar,Item::Tobacco,Item::Cotton,Item::Furs,
		Item::Lumber,Item::Ore,Item::Silver,Item::Horses,
		Item::Rum,Item::Cigars,Item::Cloth,Item::Coats,
		Item::TradeGoods,Item::Tools,Item::Muskets
	};

}

#endif
