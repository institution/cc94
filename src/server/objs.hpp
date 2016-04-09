#pragma once

#include "col.hpp"
#include "base.hpp"



namespace col {


	inline int stoi0(string const& s, int none = 0) {
		if (trim_copy(s).empty()) {
			return none;
		}
		return stoi(s);
	}

	struct Reserve {

	};

	struct Nation;

	struct NationColor{
		uint8_t r,g,b,a;
		NationColor() {}
		NationColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0): r(r), g(g), b(b), a(a) {}
		

		template<class A>
		void serialize(A & ar, uint const& version) {
			ar & r;
			ar & g;
			ar & b;
			ar & a;
		}
	};

	ostream& operator<<(ostream &out, NationColor color);




}
