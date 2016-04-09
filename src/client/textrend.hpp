#pragma once

#include <string>
#include "align.hpp"
#include "base.hpp"

namespace col{

	using std::string;

	v2s get_text_dim(PixFont const& font, string const& text, size_t & i);


	
	/** Return render dim of text line (trimmed by '\n')
	*/
	inline v2s get_text_dim(PixFont const& font, string const& text) {
		size_t i = 0;
		return get_text_dim(font, text, i);
	}

	/** Render text at position 
	*/
	b2s render_text_at(
		Front & win,
		v2s pos,
		PixFont const& font,
		Color fg,
		Color bg,
		string const& text
	);

	/** Render text at box with aligment 
	*/
	inline b2s render_text(
		Front &win,

		v2s pos,
		v2s dim,
		v2f align,

		PixFont const& font,
		Color fg,
		Color bg,

		string const& text
	) {
		return render_text_at(win,
			calc_align(pos, dim, get_text_dim(font, text), align),
			font,
			fg,
			bg,
			text
		);
	}

	/** Return render dim of multiline text 
	*/
	v2s get_text_dim2(PixFont const& font, string const& text);
	

	/** Render multiline text at position
	*/
	b2s render_text_at2(
		Front & win,

		v2s r_pos,

		PixFont & font,
		Color fg,
		Color bg,

		string const& text
	);
	
	
	
	
}

