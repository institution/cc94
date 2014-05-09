#ifndef OBJS_H
#define OBJS_H


#include "col.hpp"
#include "base.h"



namespace col {



	struct Reserve {

	};

	struct Player;

	struct Color{
		uint8 r,g,b;
		Color() {}
		Color(uint8 r, uint8 g, uint8 b): r(r), g(g), b(b) {}


		template<class A>
		void serialize(A & ar, uint const& version) {
			ar & r;
			ar & g;
			ar & b;
		}
	};

	ostream& operator<<(ostream &out, const Color &color);




}
#endif