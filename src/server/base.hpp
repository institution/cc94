#pragma once

#include <array>
#include "ext/vals.hpp"
#include "col.hpp"
#include "phys.hpp"
#include "item.hpp"
#include "biome.hpp"

namespace col {


	using Coord = int16;

	using Coords = Vector2<Coord>;

	using ext::fail;

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
	}

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
	}



	uint8 const TIME_UNIT = 12;
	uint8 const SPACE_UNIT = 12;
	// uint8 const COLONIZE_TP_COST = 6;


	uint8 const WORK = 'w';
	uint8 const ROAD = 'r';
	uint8 const PLOW = 'p';
	//uint8 const BUILD = 'b';



	using Travel = uint8_t;
	
	Travel const TravelNone = 0;
	Travel const TravelLand = 1;
	Travel const TravelSea = 2;


	//string get_name(Travel const& tr) {
	//
	//}






	namespace PlaceType{
		using type = uint16;
		type const
			None = 0,
			Terr = 1,
			Colony = 2,
			Unit = 3,
			Build = 4,
			Europe = 5,
			Field = 6;
	}
	
	enum struct Class: uint8_t {
		None,
		Terr,
		Colony,
		Unit,
		Build,
		Europe,
		Field,
		BuildType,
		UnitType
	};
	

	struct Placeable;

	struct Place {
		PlaceType::type place_type() { return PlaceType::None; }

		//virtual bool can_put(Placeable const& x) const = 0;
		//virtual void put(Placeable & x) = 0;

		//virtual bool can_take(Placeable const& x) const = 0;



	};



	struct : Place {
		PlaceType::type place_type() { return PlaceType::None; };
	} PlaceNone;





	struct Placeable {
		Place *place{&PlaceNone};

		virtual int get_size() { return 0; }

		//Placeable(Place &p): place(&p) {}

		//virtual int get_size() const = 0;
	};




	struct Cargo {
		Item item;
		uint16 num;
		Cargo(Item const& item, uint16 const& num):
			item(item), num(num)
		{}

	};

	/*template <class R>
	aga2::Mv1<R> operator*(aga2::Mv1<R> const& x, R const& y) {
		//return x * aga2::Mv0<R>(y);
		return x * y;
	}*/

	/*template <class R>
	aga2::Mv1<R> operator*(R const& x, aga2::Mv1<R> const& y) {
		return aga2::Mv0<R>(x) * y;
	}*/





}

/*
namespace std {
	template <>
	struct hash<col::Item>
	{
		size_t operator()(col::Item const& x) const
		{
			return hash<int>()(x.val);
		}
	};
}*/
