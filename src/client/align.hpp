#pragma once

#include "base.hpp"

namespace col {
	
	using ext::vmul;

	inline v2s calc_align(
		b2s box, v2s dim,
		v2f const& align = v2f(0.5, 0.5)
	) {
		return box.pos + v2s(vmul(v2f(box.dim - dim), align));
	}

}


