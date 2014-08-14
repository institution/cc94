#ifndef ITEM2_H
#define ITEM2_H

#include "col.hpp"
#include "enum.hpp"



namespace col {
	struct Item: Enum<> {
		using Enum<>::Enum;

		string const& get_name() const;
		int get_serial_id() const;

		using Id = int;

	};

	inline int get_id(Item const& item) {
		return item.get_value();
	}

	inline
	int Item::get_serial_id() const {
		return get_value();
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
		ItemHammers{18},
		ItemCross{19},
		ItemBell{20};

	int const NITEMS = 21; // total num of items

}

#endif
