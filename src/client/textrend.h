#ifndef TEXTREND_H_697655
#define TEXTREND_H_697655

#include "col.hpp"
#include "backend/pixfont.h"
#include "geo2D.h"

namespace col{

	v2i get_text_dim(backend::PixFont const& font, string const& text, size_t & i);

	
	/** Return render dim of text line (trimmed by '\n')
	*/
	inline v2i get_text_dim(backend::PixFont const& font, string const& text) {
		size_t i = 0;
		return get_text_dim(font, text, i);
	}

	/** Render text at position 
	*/
	b2i render_text_at(
		backend::Back &win,
		v2i const& pos,
		backend::PixFont const& font,
		backend::Color const& fgcol,
		backend::Color const& bgcol,
		string const& text
	);

	/** Render text at box with aligment 
	*/
	inline b2i render_text(
		backend::Back &win,

		v2i const& pos,
		v2i const& dim,
		v2f const& align,

		backend::PixFont const& font,
		backend::Color const& fgcol,
		backend::Color const& bgcol,

		string const& text
	) {
		return render_text_at(win,
			calc_align(pos, dim, get_text_dim(font, text), align),
			font,
			fgcol,
			bgcol,
			text
		);
	}

	/** Return render dim of multiline text 
	*/
	v2i get_text_dim2(backend::PixFont const& font, string const& text);
	

	/** Render multiline text at position
	*/
	b2i render_text_at2(
		backend::Back &win,

		v2i const& r_pos,

		backend::PixFont const& font,
		backend::Color const& fgcol,
		backend::Color const& bgcol,

		string const& text
	);
	
	
	
	
}


#endif
