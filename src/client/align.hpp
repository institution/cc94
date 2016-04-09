#pragma once

#include "base.hpp"

namespace col{

	using ext::vmul;
	
	/*inline
	v2f vmul(v2f const& a, v2f const& b) {
		return v2f(a[0]*b[0], a[1]*b[1]);
	}

	inline
	v2f vmul(v2i const& a, v2f const& b) {
		return v2f(float(a[0])*b[0], float(a[1])*b[1]);
	}

	inline
	v2i vmul(v2i const& a, v2i const& b) {
		return v2i(a[0]*b[0], a[1]*b[1]);
	}*/

	/*inline
	v2i vmul(Coords const& a, v2i const& b) {
		return v2i(int(a[0])*b[0], int(a[1])*b[1]);
	}*/


	inline
	v2s calc_align(b2s const& par, v2s const& dim, v2f const& align = v2f(0.5, 0.5)) {
		return v2s(v2f(par.pos) + vmul(v2f(par.dim - dim), align));
	}

	inline
	v2s calc_align(v2s const& p_pos, v2s const& p_dim, v2s const& dim, v2f const& align = v2f(0.5, 0.5)) {
		return v2s(v2f(p_pos) + vmul(v2f(p_dim - dim), align));
	}
	
	inline
	v2s calc_align(v2s const& p_pos, v2s const& p_dim, v2f const& align, v2s const& dim) {
		return v2s(v2f(p_pos) + vmul(v2f(p_dim - dim), align));
	}

}


