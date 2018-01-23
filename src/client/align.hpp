#pragma once

#include "base.hpp"

namespace col{

	using ext::vmul;



	/** Align dim inside box (p_pos, p_dim)
	*/
	/*inline v2s calc_align(
		v2s p_pos, v2s p_dim, v2s dim,
		v2f const& align = v2f(0.5, 0.5)
	) {
		return p_pos + v2s(vmul(v2f(p_dim - dim), align));
	}*/

	inline v2s calc_align(
		b2s box, v2s dim,
		v2f const& align = v2f(0.5, 0.5)
	) {
		return box.pos + v2s(vmul(v2f(box.dim - dim), align));
	}
		
	/** Align dim inside par_dim
	*/	
	/*inline v2s calc_align(
		v2s par_dim, v2s dim,
		v2f const& align = v2f(0.5, 0.5)
	) {
		return v2s(vmul(v2f(par_dim - dim), align));
	}*/

	/** Align dim inside box
	*/
	/*inline v2s calc_align(
		b2s par, v2s dim,
		v2f const& align = v2f(0.5, 0.5)
	) {
		return par.pos + calc_align(par.dim, dim, align);
	}*/



}


