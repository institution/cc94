#ifndef BASE2_H
#define BASE2_H

#include "col.hpp"
#include <array>

namespace col {

	using Code = int16;
	Code const OK = 0;


	using Coord = int16;
	using Coords = Vector2<Coord>;

	/* Dir::t code is yx in mod 3-1 (2 -> -1)
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


	namespace Dir{
		using type = uint8;
		using t = type;
		type const
			Q=0, W=1, E=2,
			A=3, S=4, D=5,
			Z=6, X=7, C=8;
	};

	//array<Dir::t const, 9> ALL_DIRS;
	extern std::array<Dir::t const, 9> ALL_DIRS;

	inline Coords vec4dir(Dir::t const& d) {
		return Coords((d % 3) - 1, (d / 3) - 1);
	}

	inline Dir::t dir4vec(Coords const& d) {
		return (d[0] + 1) + (d[1] + 1) * 3;
	}


	namespace Order{
		using type = char;

		type const
			BuildRoad = 'R',
			BuildColony = 'B',
			Unknown = '-',
			Space = ' ',
			Move = 'm',
			Work = 'w',
			PlowFields = 'P',
			ClearForset = 'p';
	};



	uint8 const TIME_UNIT = 6;
	uint8 const SPACE_UNIT = 6;
	// uint8 const COLONIZE_TP_COST = 6;


	uint8 const WORK = 'w';
	uint8 const ROAD = 'r';
	uint8 const PLOW = 'p';
	//uint8 const BUILD = 'b';


	using Travel = uint8;
	Travel const LAND = 1;
	Travel const SEA = 2;

	namespace Biome{
		using type = uint8;
		type const
			None = 0,
			Tundra = 1, //boreal forest
			Prairie = 4, // broadleaf forest
			Plains = 3, // mixed_forest
			Desert = 2, //scrub forest
			Savannah = 6, //tropical forest
			Grassland = 5, //confier forest
			Marsh = 7, //wetland forest
			Swamp = 8, //rain forest
			// 9 unused
			Arctic = 10,
			Ocean = 11,
			SeaLane = 12;
	}

	inline
	bool is_water_biome(Biome::type b) {
		return b == Biome::Ocean or b == Biome::SeaLane;
	}



	namespace Phys{
		using type = uint8;
		type const
			None = 0,
			Hill = 1,
			Mountain = 2,
			Road = 4,
			Forest = 8,
			MinorRiver = 16,
			MajorRiver = 32,
			Plow = 64,
			Worker = 128;
	}



	namespace Item{
		using type = uint8;
		type const
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
			Bell = 63;
	}




	namespace PlaceType{
		using type = uint16;
		type const
			None = 0,
			Terr = 1,
			Colony = 2,
			Unit = 3,
			Build = 4,
			Europe = 5;
	};

	struct Placeable;

	struct Place {
		virtual PlaceType::type place_type() = 0;
	};

	struct Workplace{
		virtual bool assign() = 0;
		virtual bool leave() = 0;
		virtual uint16 get_yield(Item::type const& item, bool const& is_expert) const = 0;
		virtual PlaceType::type place_type() = 0;
	};

	struct Placeable {
		Place *place;

		Placeable(): place(nullptr) {

		}
	};


	struct Cargo {
		Item::type item;
		uint16 amount;

		Cargo(Item::type const& item, uint16 const& amount):
			item(item), amount(amount)
		{}

	};

	template <class R>
	aga2::Mv1<R> operator*(aga2::Mv1<R> const& x, R const& y) {
		return x * aga2::Mv0<R>(y);
	}

	template <class R>
	aga2::Mv1<R> operator*(R const& x, aga2::Mv1<R> const& y) {
		return aga2::Mv0<R>(x) * y;
	}


}

#endif
