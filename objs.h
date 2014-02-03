#ifndef OBJS_H
#define OBJS_H

#include <boost/format.hpp>
#include "col.h"
#include "iomm.h"

namespace col {
	
	struct IconType{
		using Key = uint32;
		Key id;		
		uint8 move;
		uint16 pic;
		string name;
		
		IconType(uint32 id, string name, uint8 move, uint16 pic);
	};
	
	
	/*
	IconTypes map<IconType::Key, IconType> icon_types;
	
	icon_types[100] = IconType(100, "Free Colonist", 1, 100);
	icon_types[104] = IconType(104, "Scout", 3, 104);
		
	Icon create(const IconType &ic) {
		
	}
	*/
	
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