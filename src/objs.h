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
	
			
	

	
	struct TerrType{
		using Id = uint8;
		
		string name;
		Id id;
		uint8 movement_cost;
		uint8 movement_type;
		uint8 defensive;
		
		TerrType() {}
		
		TerrType(const vector<string> &xs) {
			assert(xs.size() >= 16);
			
			name = trim_copy(xs[0]);
			id = stoi(xs[1]);
			movement_cost = stoi(xs[2]) * UNIT_OF_MOVEMENT;
			defensive = stoi(xs[3]);			
			movement_type = stoi(xs[15]);
		}
				
	};
	
	struct UnitType{
		using Id = uint32;
		
		string name;
		Id id;
		uint8 movement;
		uint8 attack;
		uint8 combat;
		uint8 cargo;
		uint8 size;
		uint8 movement_type;

		UnitType() {}
		
		UnitType(const vector<string> &xs) {
			assert(xs.size() >= 8);
			name = trim_copy(xs[0]);
			id = stoi(xs[1]);
			movement = stoi(xs[2]) * UNIT_OF_MOVEMENT;
			attack = stoi(xs[3]);
			combat = stoi(xs[4]);
			cargo = stoi(xs[5]);
			size = stoi(xs[6]);
			// unused column 7 
			// unused column 8
			movement_type = stoi(xs[9]);			
		}
				
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