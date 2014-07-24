#ifndef OBJS_H
#define OBJS_H


#include "col.hpp"
#include "base.h"



namespace col {


	inline int stoi0(string const& s, int none = 0) {
		if (trim_copy(s).empty()) {
			return none;
		}
		return stoi(s);
	}

	struct Reserve {

	};

	struct Player;

	struct Color{
		uint8 r,g,b,a;
		Color() {}
		Color(uint8 r, uint8 g, uint8 b, uint8 a = 0): r(r), g(g), b(b), a(a) {}


		template<class A>
		void serialize(A & ar, uint const& version) {
			ar & r;
			ar & g;
			ar & b;
			ar & a;
		}
	};

	ostream& operator<<(ostream &out, const Color &color);




}
#endif