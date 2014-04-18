#ifndef BASE2_H
#define BASE2_H

#include "col.h"

namespace col {
	
	using Code = int16;
	Code const OK = 0;
	

	using Coord = int16;
	using Coords = Vector2<Coord>;
	
	
	enum class Dir: int8{
		Q=0, W=1, E=2,
		A=3,      D=5,
		Z=6, X=7, C=8
	};

	inline Coords vec4dir(Dir const& d) {
		auto n = static_cast<int8>(d);
		return Coords((n % 3) - 1, (n / 3) - 1);
	}

	inline Dir dir4vec(Coords const& d) {
		return static_cast<Dir>((d[0] + 1) + (d[1] + 1) * 3);
	}

	
	enum class PlaceType{
		Terr = 1,
		Colony = 2,
		Unit = 3,
		Build = 4
	};
	
	struct Placeable;
	
	struct Place {
		virtual PlaceType place_type() = 0;
	};

	struct Placeable {
		Place *place;
		
		Placeable(): place(nullptr) {
			
		}
	};
	
	
	
	uint8 const UNIT_OF_MOVEMENT = 12;
	uint8 const TIME_UNIT = 6;
	// uint8 const COLONIZE_TP_COST = 6;
	
	
	using MovementType = uint8;
	using MType = MovementType;
	using Biome = uint8;
	using Phys = uint8;
	using Item = uint8;
		
	
	MovementType const MOVEMENT_TYPE_LAND = 1;
	MovementType const MOVEMENT_TYPE_NAVAL = 2;
	MovementType const MOVEMENT_TYPE_SEA = 2;
	MType const LAND = 1;
	MType const SEA = 2;
	
	
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
	
	Phys const PHYS_NONE = 0;
	Phys const PHYS_HILL = 1;
	Phys const PHYS_MOUNTAIN = 2;	
	Phys const PHYS_ROAD = 4;
	Phys const PHYS_FOREST = 8;
	Phys const PHYS_MINOR_RIVER = 16;
	Phys const PHYS_MAJOR_RIVER = 32;
	Phys const PHYS_DOCKS = 64;
	
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
	
	
	
}

#endif
