#pragma once

#include "col.hpp"
#include "distinct.hpp"



namespace col {
	namespace detail{
		struct Item;
	}

	using Item = distinct::Enum<detail::Item, uint8_t>;

	inline int get_id(Item const& item) {
		return item.get_value();
	}



	constexpr Item const
		ItemNone{0},
		
		// Field items
		ItemFood{1},
		ItemFish{2},
		ItemSugar{3},
		ItemTobacco{4},
		ItemCotton{5},
		ItemFurs{6},
		ItemLumber{7},
		ItemOre{8},
		ItemSilver{9},
		
		// Build items
		ItemHorses{10},
		ItemRum{11},
		ItemCigars{12},
		ItemCloth{13},
		ItemCoats{14},
		ItemTradeGoods{15},
		ItemTools{16},
		ItemMuskets{17},
		ItemCannons{18},
		
		// Other
		ItemHammers{19},
		ItemCross{20},
		ItemBell{21},

		// Pop growth
		ItemToys{22},   // humans: 200 toys -> Colonist
		ItemRags{23},    // horses: 50 rags -> Horse


		
		
		ItemEnd{25}    // keep this at the end
		;
		
	int const ItemCount = ItemEnd; // total num of items
	
	int const ItemNoneCount = 1;
	int const ItemFieldCount = 10;
	int const ItemBuildCount = 22;
	
	int const NITEMS = 24; // total num of items








}
