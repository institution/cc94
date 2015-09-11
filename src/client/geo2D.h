#ifndef GEO2D_H_254354
#define GEO2D_H_254354

#include "aga2.hpp"
#include "col.hpp"
#include "backend/box.hpp"
#include "col.hpp"
#include "env.h"
#include "layout.h"

namespace col{

	//using backend::v2i;
	using backend::b2i;
	//using v2f = aga2::Mv1<float>;
	using backend::Box;
	
	
	inline
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
	}

	inline
	v2i vmul(Coords const& a, v2i const& b) {
		return v2i(int(a[0])*b[0], int(a[1])*b[1]);
	}


	inline
	v2i calc_align(b2i const& par, v2i const& dim, v2f const& align = v2f(0.5, 0.5)) {
		return v2i(v2f(par.pos) + vmul(v2f(par.dim - dim), align));
	}

	inline
	v2i calc_align(v2i const& p_pos, v2i const& p_dim, v2i const& dim, v2f const& align = v2f(0.5, 0.5)) {
		return v2i(v2f(p_pos) + vmul(v2f(p_dim - dim), align));
	}
	
	inline
	v2i calc_align(v2i const& p_pos, v2i const& p_dim, v2f const& align, v2i const& dim) {
		return v2i(v2f(p_pos) + vmul(v2f(p_dim - dim), align));
	}

}


#endif
