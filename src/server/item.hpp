#pragma once

#include "col.hpp"
#include "enums.hpp"

namespace col {


	using Item = int8_t;

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
		ItemHammers{19},
		ItemCross{20},
		ItemBell{21},

		// Pop growth
		ItemToys{22},
		ItemRags{23},

		ItemEnd{24};
		

	Item const 
		ItemNoneEnd = ItemNone + 1,
		ItemFieldEnd = ItemSilver + 1,
		ItemBuildEnd = ItemBell + 1;


	array<char const*, ItemEnd> const item_names = {
		"None",
		"Food",
		"Fish",
		"Sugar",
		"Tobacco",
		"Cotton",
		"Furs",
		"Lumber",
		"Ore",
		"Silver",
		"Horses",
		"Rum",
		"Cigars",
		"Cloth",
		"Coats",
		"TradeGoods",
		"Tools",
		"Muskets",
		"Cannons",
		"Hammers",
		"Cross",
		"Bell",
		"Toys",
		"Rags"
	};
	
	inline char const* get_item_name(Item x) {
		return item_names.at(x);
	}
	
	inline Item get_item_by_name(char const* name) {
		return _get_by_name<Item, ItemEnd>(item_names, name);
	}

	inline Item get_item_by_name(string const& name) {
		return _get_by_name<Item, ItemEnd>(item_names, name);
	}


}
