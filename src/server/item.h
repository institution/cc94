#ifndef ITEM_H_22354235
#define ITEM_H_22354235

#include "col.hpp"
#include "distinct.hpp"



namespace col {
	namespace detail{
		struct Item;
	}

	using Item = distinct::Enum<detail::Item, uint8>;

	inline int get_id(Item const& item) {
		return item.get_value();
	}

	constexpr
	Item const
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
		ItemRags{23}    // horses: 50 rags -> Horse		
		;
		
	int const ItemCount = 24; // total num of items
	int const NITEMS = 24; // total num of items

}

#endif
