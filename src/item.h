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

	inline
	int Item::get_serial_id() const {
		return get_value();
	}

	constexpr
	Item const
		ItemNone{0},
		ItemFood{1},
		ItemSugar{2},
		ItemTobacco{3},
		ItemCotton{4},
		ItemFurs{5},
		ItemLumber{6},
		ItemOre{7},
		ItemSilver{8},
		ItemHorses{9},
		ItemRum{10},
		ItemCigars{11},
		ItemCloth{12},
		ItemCoats{13},
		ItemTradeGoods{14},
		ItemTools{15},
		ItemMuskets{16},
		ItemHammers{17},
		ItemCross{18},
		ItemFish{19},
		ItemBell{20};


}

#endif
