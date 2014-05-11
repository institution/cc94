#include "item.h"

namespace col{
	string get_name(Item const& item) {
		switch (item) {
			default: return "<unknown-item-name>";
			case ItemNone: return "None";
			case ItemFood: return "Food";
			case ItemSugar: return "Sugar";
			case ItemTobacco: return "Tobacco";
			case ItemCotton: return "Cotton";
			case ItemFurs: return "Furs";
			case ItemLumber: return "Lumber";
			case ItemOre: return "Ore";
			case ItemSilver: return "Silver";
			case ItemHorses: return "Horses";
			case ItemRum: return "Rum";
			case ItemCigars: return "Cigars";
			case ItemCloth: return "Cloth";
			case ItemCoats: return "Coats";
			case ItemTradeGoods: return "TradeGoods";
			case ItemTools: return "Tools";
			case ItemMuskets: return "Muskets";
			case ItemHammers: return "Hammers";
			case ItemCross: return "Cross";
			case ItemFish: return "Fish";
			case ItemBell: return "Bell";
		}
	}
	
	
}
