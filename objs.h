#ifndef OBJS_H
#define OBJS_H

#include <boost/format.hpp>
#include "col.h"
#include "iomm.h"



namespace col {
	
	// map
	// col
	
	const uint8 UNIT_MOVEMENT = 3;
	
	using Coord = int16;
	using Coords = Vector2<Coord>;


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
			movement_cost = stoi(xs[2]);
			defensive = stoi(xs[3]);			
			movement_type = stoi(xs[15]) * UNIT_MOVEMENT;
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
			movement = stoi(xs[2]) * UNIT_MOVEMENT;
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
	
	struct Icon {
		using Id = uint32;
		
		Id id;
		UnitType const *type;
		Coords pos;
		Player const *player;	
		uint8 movement_used;  // movement used in this turn
		
		
				
		//uint8 spec_id;

		Icon(): movement_used(0) {}
		
		Icon(
			Id id, 
			UnitType const &type, 
			Coords pos, 
			Player const &player
		) {	
			this->id = id; 
			this->type = &type; 
			this->pos = pos;
			this->movement_used = 0;
			this->player = &player;
		}
		
		void turn() {
			movement_used = 0;
		}

	}; 

	ostream& operator<<(ostream &out, const Icon &obj);
	
	
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
		uint32 flag_id;
		
		Player() {}

		Player(Id id_, string name_, const Color &color_, uint32 flag_id_):
			id(id_), name(name_), color(color_), flag_id(flag_id_) {
		}
		
		

	};
	
	ostream& operator<<(ostream &out, const Player &obj);

}
#endif