#ifndef OBJS_H
#define OBJS_H

#include <boost/format.hpp>
#include "col.h"
#include "iomm.h"



namespace col {
	
	// map
	// col
	
	const uint8 UNIT_OF_MOVEMENT = 12;
	
	using Coord = int16;
	using Coords = Vector2<Coord>;
	
	
	using MovementType = uint8;
	MovementType const MOVEMENT_TYPE_LAND = 1;
	MovementType const MOVEMENT_TYPE_NAVAL = 2;
	
	using Biome = uint8;
	
	Biome const BIOME_TUNDRA = 1; //BOREAL FOREST
	Biome const BIOME_PRAIRIE = 4; // BROADLEAF FOREST
	Biome const BIOME_PLAINS = 3; // MIXED_FOREST
	Biome const BIOME_DESERT = 2; //SCRUB FOREST
	Biome const BIOME_SAVANNAH = 6; //TROPICAL FOREST				
	Biome const BIOME_GRASSLAND = 5; //CONFIER FOREST		
	Biome const BIOME_MARSH = 7; //WETLAND FOREST
	Biome const BIOME_SWAMP = 8; //RAIN FOREST
	Biome const BIOME_ARCTIC = 10;
	Biome const BIOME_OCEAN = 11;
	Biome const BIOME_SEA_LANE = 12;
	
	using Phys = uint8;
	
	Phys const PHYS_HILL = 1;
	Phys const PHYS_MOUNTAIN = 2;	
	Phys const PHYS_ROAD = 4;
	Phys const PHYS_FOREST = 8;
	Phys const PHYS_MINOR_RIVER = 16;
	Phys const PHYS_MAJOR_RIVER = 32;
	Phys const PHYS_DOCKS = 64;
	
	using Item = uint8;
	
	Item const ITEM_FOOD = 23;
	Item const ITEM_SUGAR = 24;
	Item const ITEM_TOBACCO = 25;
	Item const ITEM_COTTON = 26;
	Item const ITEM_FURS = 27;
	Item const ITEM_LUMBER = 28;
	Item const ITEM_ORE = 29;
	Item const ITEM_SILVER = 30;
	Item const ITEM_HORSES = 31;
	Item const ITEM_RUM = 32;
	Item const ITEM_CIGARS = 33;
	Item const ITEM_CLOTH = 34;
	Item const ITEM_COATS = 35;
	Item const ITEM_TRADE_GOODS = 36;
	Item const ITEM_TOOLS = 37 ;
	Item const ITEM_MUSKETS = 38;
	
	using Stock = map<Item,float>;
	
	
	
	struct Place {
		
	};

	
	
	
	
	
	struct Player;
	/*
	struct Struct {
		using Id = uint32;
		SType *type; // name 
		
		People workers;
				
		Coord x, y;
		
		
	};*/
	

	
	
	struct Color{
		uint8 r,g,b;	
		Color() {}
		Color(uint8 r_, uint8 g_, uint8 b_): r(r_), g(g_), b(b_) {}
	};
	
	ostream& operator<<(ostream &out, const Color &color);
	
	
	struct Player {
		using Id = uint32;
		
		Id id;
		string name;
		Color color;
		uint8 flag_id;
		
		Player() {}

		Player(Id id_, string name_, const Color &color_, uint32 flag_id_):
			id(id_), name(name_), color(color_), flag_id(flag_id_) {
		}
		
		

	};
	
	ostream& operator<<(ostream &out, const Player &obj);

}
#endif