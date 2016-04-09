#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include "../ext/ext.hpp"

namespace front {
		
	struct Color {
		uint8_t r,g,b,a;
		
		Color() = default;
		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a): r(r), g(g), b(b), a(a) {}
		Color(uint8_t r, uint8_t g, uint8_t b): Color(r,g,b,255) {}		
	};

	inline std::ostream & operator<<(std::ostream & o, Color const& c) {
		o << format("(%|| %|| %|| %||)", uint(c.r), uint(c.g), uint(c.b), uint(c.a));
		return o;
	}

	inline bool operator!=(Color const& x, Color const& y) {
		return x.r != y.r or x.g != y.g or x.b != y.b or x.a != y.a;
	}

	inline bool operator==(Color const& x, Color const& y) {
		return x.r == y.r and x.g == y.g and x.b == y.b and x.a == y.a;
	}

	struct ColorFloat {
		float_t r,g,b,a;
		
		ColorFloat() = default;
		ColorFloat(float_t r, float_t g, float_t b, float_t a): r(r), g(g), b(b), a(a) {}
		ColorFloat(Color c):
			r(float_t(c.r)/255.0f),
			g(float_t(c.g)/255.0f),
			b(float_t(c.b)/255.0f),
			a(float_t(c.a)/255.0f)
		{}
	};

}
