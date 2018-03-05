#pragma once

#include <unordered_map>

#include "build.hpp"
#include "unit.hpp"
#include "base.hpp"



namespace col {

	inline char const* get_name(Item const& item) { return get_item_name(item); }	
	inline string const& get_name(BuildType const& x) { return x.get_name(); }
	inline string const& get_name(UnitType const& x) { return x.get_name(); }
	inline string const& get_name(Makeable const& x) { return x.get_name(); }
	

	array<Item, 16> const COLONY_ITEMS = {
		ItemFood,ItemSugar,ItemTobacco,ItemCotton,ItemFurs,
		ItemLumber,ItemOre,ItemSilver,ItemHorses,
		ItemRum,ItemCigars,ItemCloth,ItemCoats,
		ItemTradeGoods,ItemTools,ItemMuskets
	};

		
	//
	auto const biome_icons_ids = array<int,BiomeEnd>({0,1,4,3,2,6,5,7,8,10});

	inline
	int get_biome_icon_id(Biome const& b) {
		return biome_icons_ids.at(b);
	}


	auto const item_icons_ids = array<int,22>({
		0,
		23,
		58,
		24,
		25,
		26,
		27,
		28,
		29,
		30,
		31,
		32,
		33,
		34,
		35,
		36,
		37,
		38,
		66,
		55,
		57,
		63
	});

	inline
	int get_item_icon_id(Item b) {
		return item_icons_ids.at(b);
	}


}
