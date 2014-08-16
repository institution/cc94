#ifndef VIEW_BASE44_H
#define VIEW_BASE44_H
// ^renderer base

#include "base.h"

#include <unordered_map>


namespace col {

	int const SCREEN_W = 320;
	int const SCREEN_H = 200;
	int const TILE_DIM = 16;
	int const GLOBAL_SCALE = 3;

	string const RES_PATH = "../COLONIZE/";
	string const FONT_PATH = "../font/";

	v2i const tile_dim = v2i(TILE_DIM, TILE_DIM);

	extern std::unordered_map<Biome,string,Biome::hash> BIOME_NAMES;
	Biome get_biome_by_name(string const& name);

	extern std::unordered_map<Phys,string,Phys::hash> PHYS_NAMES;
	Phys get_phys_by_name(string const& name);

	extern std::unordered_map<Item,string,Item::hash> ITEM_NAMES;
	Item get_item_by_name(string const& name);


	array<Item, 16> const COLONY_ITEMS = {
		ItemFood,ItemSugar,ItemTobacco,ItemCotton,ItemFurs,
		ItemLumber,ItemOre,ItemSilver,ItemHorses,
		ItemRum,ItemCigars,ItemCloth,ItemCoats,
		ItemTradeGoods,ItemTools,ItemMuskets
	};


	//
	auto const biome_icons_ids = array<int,10>({0,1,4,3,2,6,5,7,8,10});

	inline
	int get_biome_icon_id(Biome const& b) {
		return biome_icons_ids.at(b.get_value());
	}



	auto const item_icons_ids = array<int,21>({0,23,58,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,55,57,63});

	inline
	int get_item_icon_id(Item const& b) {
		return item_icons_ids.at(b.get_value());
	}


}

#endif
