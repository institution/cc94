#ifndef OBJS_H
#define OBJS_H


#include "col.h"
#include "base.h"
#include "iomm.h"



namespace col {
	
	// map
	// col
	
	
	
	
	
	
	
	
	
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