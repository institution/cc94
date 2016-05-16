#pragma once

#include <array>
#include "ext/vals.hpp"
#include "ext/ext.hpp"
#include "col.hpp"
#include "phys.hpp"
#include "item.hpp"
#include "biome.hpp"

namespace col {


	using Coord = int16_t;
	using Coords = ext::v2<Coord>;
	using v2c = Coords;

	using ext::fail;

	

	
	using Time = int8_t;	
	Time const TimeUnit = 12;
	Time const TimeInf = 120;		
	
	
	Time const TIME_UNIT = 12;
	int8_t const SPACE_UNIT = 12;
	// uint8 const COLONIZE_TP_COST = 6;


	char const WORK = 'w';
	char const ROAD = 'r';
	char const PLOW = 'p';
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






}

#include "dir.hpp"

