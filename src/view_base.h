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

	extern std::unordered_map<uint8,string> BIOME_NAMES;
	Biome::type get_biome_by_name(string const& name);

	extern std::unordered_map<uint8,string> PHYS_NAMES;
	Phys::type get_phys_by_name(string const& name);

	extern std::unordered_map<Item,string> ITEM_NAMES;
	Item get_item_by_name(string const& name);


	array<Item, 16> const COLONY_ITEMS = {
		ItemFood,ItemSugar,ItemTobacco,ItemCotton,ItemFurs,
		ItemLumber,ItemOre,ItemSilver,ItemHorses,
		ItemRum,ItemCigars,ItemCloth,ItemCoats,
		ItemTradeGoods,ItemTools,ItemMuskets
	};

}

#endif
