#ifndef OBJS_H
#define OBJS_H

#include <boost/format.hpp>
#include "col.h"
#include "iomm.h"






namespace col {
	
	// map
	// col
		
	struct IconType{
		using Key = uint32;
		
		Key id;
		
		string name;
		uint8 pic;
		uint8 movement;
		uint8 attack;
		uint8 combat;
		uint8 cargo;
		uint8 size;
		
		//uint8 cost;
		//uint8 tools;
		//uint8 guns;
		//uint8 hull;

		IconType() {}
		
		IconType(const vector<string> &xs) {
			assert(xs.size() >= 11);
			name = trim_copy(xs[0]);
			pic = stoi(xs[1]);
			movement = stoi(xs[2]);
			attack = stoi(xs[3]);
			combat = stoi(xs[4]);
			cargo = stoi(xs[5]);
			size = stoi(xs[6]);
			
			//cost = stoi(xs[7]);
			//tools = stoi(xs[8]);
			//guns = stoi(xs[9]);
			//hull = stoi(xs[10]);
			
			id = pic;
		}
				
	};
	
	
	
	
	
	struct Icon {
		uint32 id;
		uint32 type_id;
		uint16 x;
		uint16 y;
		uint8 move_left;
		uint32 player_id;

		Icon() {}

		Icon(uint32 id_, uint16 type_id_, uint16 x_, uint16 y_, uint32 player_id_): 
			id(id_), type_id(type_id_), x(x_), y(y_), player_id(player_id_) {
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
		uint32 id;
		string name;
		Color color;
		
		Player() {}

		Player(uint32 id_, string name_, const Color &color_):
			id(id_), name(name_), color(color_) {
		}

	};
	
	ostream& operator<<(ostream &out, const Player &obj);

}
#endif