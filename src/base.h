#ifndef BASE2_H
#define BASE2_H

#include "col.hpp"

namespace col {
	
	using Code = int16;
	Code const OK = 0;
	

	using Coord = int16;
	using Coords = Vector2<Coord>;
	
	/* Dir code is yx in mod 3-1 (2 -> -1)
	00 0  -1,-1
	01 1  -1,0 
	02 2  -1,1
	10 3  0,-1
	11 4  0,0
	12 5  0,1
	20 6  1,-1
	21 7  1,0
	22 8  1,1
	*/
	
	enum class Dir: int8{
		Q=0, W=1, E=2,
		A=3, S=4, D=5,
		Z=6, X=7, C=8
	};

	inline Coords vec4dir(Dir const& d) {
		auto n = static_cast<int8>(d);
		return Coords((n % 3) - 1, (n / 3) - 1);
	}

	inline Dir dir4vec(Coords const& d) {
		return static_cast<Dir>((d[0] + 1) + (d[1] + 1) * 3);
	}

	
	
	
	
	uint8 const TIME_UNIT = 6;
	uint8 const SPACE_UNIT = 6;
	// uint8 const COLONIZE_TP_COST = 6;
	
	
	uint8 const WORK = 'w';
	uint8 const ROAD = 'r';
	uint8 const PLOW = 'p';
	uint8 const BUILD = 'b';
	
	
	using Travel = uint8;
	Travel const LAND = 1;
	Travel const SEA = 2;
	
	enum class Biome{
		Tundra = 1, //boreal forest
		Prairie = 4, // broadleaf forest
		Plains = 3, // mixed_forest
		Desert = 2, //scrub forest
		Savannah = 6, //tropical forest				
		Grassland = 5, //confier forest		
		Marsh = 7, //wetland forest
		Swamp = 8, //rain forest
		Arctic = 10,
		Ocean = 11,
		SeaLane = 12
	};
	
	
	enum class Phys{
		// this is conceptually an enumeration
		// fact that it is also a flag is implementation specific
		None = 0,
		Hill = 1,
		Mountain = 2,	
		Road = 4,
		Forest = 8,
		MinorRiver = 16,
		MajorRiver = 32,
		Plow = 64,
		Worker = 128
	};
	
	inline uint8 flag(Phys const& p) {
		return static_cast<uint8>(p);
	}
	
	template<typename T>
	inline uint8 enum_value(T const& t) {
		return static_cast<uint8>(t);
	}
	
	enum class Item{
		None = 0,
		Food = 23,
		Sugar = 24,
		Tobacco = 25,
		Cotton = 26,
		Furs = 27,
		Lumber = 28,
		Ore = 29,
		Silver = 30,
		Horses = 31,
		Rum = 32,
		Cigars = 33,
		Cloth = 34,
		Coats = 35,
		TradeGoods = 36,
		Tools = 37 ,
		Muskets = 38,
		Hammers = 55,
		Cross = 57,		
		Fish = 58,
		Bell = 63
	};
	
	
	
	
	enum class PlaceType{
		Terr = 1,   
		Colony = 2, 
		Unit = 3,
		Build = 4,
		Europe = 5
	};
	
	struct Placeable;
	
	struct Place {
		virtual PlaceType place_type() = 0;
	};

	struct Workplace{
		virtual bool assign() = 0;
		virtual bool leave() = 0;
		virtual uint16 get_yield(Item const& item, bool const& is_expert) const = 0;
	};
	
	struct Placeable {
		Place *place;
		
		Placeable(): place(nullptr) {
			
		}
	};
	
	
	struct Cargo {
		Item item;
		uint16 amount;
		
		Cargo(Item const& item, uint16 const& amount):
			item(item), amount(amount)
		{}
		
	};
	
}

#endif
