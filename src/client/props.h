#ifndef PROPS_H_4456352
#define PROPS_H_4456352

#include <unordered_map>

#include "build.h"
#include "unit.h"
#include "base.h"
#include "conf.h"



namespace col {


	v2i const tile_dim = v2i(conf.tile_dim, conf.tile_dim);

	extern std::unordered_map<Biome,string,Biome::hash> BIOME_NAMES;
	Biome get_biome_by_name(string const& name);

	extern std::unordered_map<Phys,string,Phys::hash> PHYS_NAMES;
	Phys get_phys_by_name(string const& name);

	extern std::unordered_map<Item,string,Item::hash> ITEM_NAMES;
	Item get_item_by_name(string const& name);

	string const& get_item_name(Item const& item);

	inline string const& get_name(Item const& item) { return get_item_name(item); }	
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
	auto const biome_icons_ids = array<int,10>({0,1,4,3,2,6,5,7,8,10});

	inline
	int get_biome_icon_id(Biome const& b) {
		return biome_icons_ids.at(b.get_value());
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
	int get_item_icon_id(Item const& b) {
		return item_icons_ids.at(b.get_value());
	}


}

#endif
