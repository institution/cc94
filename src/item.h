#ifndef ITEM2_H
#define ITEM2_H

#include "col.hpp"
#include "enum.hpp"



namespace col {
	struct Item: Enum<> {
		using Enum<>::Enum;
	};

	constexpr
	Item const
		ItemNone{0},
		ItemFood{23},
		ItemSugar{24},
		ItemTobacco{25},
		ItemCotton{26},
		ItemFurs{27},
		ItemLumber{28},
		ItemOre{29},
		ItemSilver{30},
		ItemHorses{31},
		ItemRum{32},
		ItemCigars{33},
		ItemCloth{34},
		ItemCoats{35},
		ItemTradeGoods{36},
		ItemTools{37},
		ItemMuskets{38},
		ItemHammers{55},
		ItemCross{57},
		ItemFish{58},
		ItemBell{63};

	string get_name(Item const& item);


}

#endif
