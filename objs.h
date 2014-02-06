#ifndef OBJS_H
#define OBJS_H

#include <boost/format.hpp>
#include "col.h"
#include "iomm.h"






namespace col {
	
	// map
	// col
	
	using Coord = uint16;
	

	
	
	struct UnitType{
		using Id = uint8;
		
		string name;
		Id id;
		uint8 movement;
		uint8 attack;
		uint8 combat;
		uint8 cargo;
		uint8 size;
		
		//uint8 cost;
		//uint8 tools;
		//uint8 guns;
		//uint8 hull;

		UnitType() {}
		
		UnitType(const vector<string> &xs) {
			assert(xs.size() >= 11);
			name = trim_copy(xs[0]);
			id = stoi(xs[1]);
			movement = stoi(xs[2]);
			attack = stoi(xs[3]);
			combat = stoi(xs[4]);
			cargo = stoi(xs[5]);
			size = stoi(xs[6]);
			
			//cost = stoi(xs[7]);
			//tools = stoi(xs[8]);
			//guns = stoi(xs[9]);
			//hull = stoi(xs[10]);
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
		Coord x, y;
		Player const *player;	
		uint8 movement_used;  // movement used in this turn
		
		//uint8 spec_id;

		Icon(): movement_used(0) {}
		
		Icon(
			Id id, 
			UnitType const &type, 
			Coord x, 
			Coord y, 
			Player const &player
		) {	
			this->id = id; 
			this->type = &type; 
			this->x = x;
			this->y = y;
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
		
		Player() {}

		Player(Id id_, string name_, const Color &color_):
			id(id_), name(name_), color(color_) {
		}

	};
	
	ostream& operator<<(ostream &out, const Player &obj);

}
#endif