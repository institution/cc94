#pragma once

#include "base.hpp"

namespace col{

	using ext::vmul;
	
	inline
	v2s calc_align(b2s par, v2s dim, v2f const& align = v2f(0.5, 0.5)) {
		return v2s(v2f(par.pos) + vmul(v2f(par.dim - dim), align));
	}

	inline
	v2s calc_align(v2s p_pos, v2s p_dim, v2s dim, v2f const& align = v2f(0.5, 0.5)) {
		return calc_align({p_pos, p_dim}, dim, align);
	}

	inline
	v2s calc_align(b2s box, v2f const& align, v2s dim) {
		return v2s(v2f(box.pos) + vmul(v2f(box.dim - dim), align));
	}
	
	inline
	v2s calc_align(v2s p_pos, v2s p_dim, v2f const& align, v2s dim) {
		return calc_align({p_pos, p_dim}, align, dim);
	}

}


