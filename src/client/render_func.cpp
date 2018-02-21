#include "render_func.hpp"


namespace col {

	/// Return text dim
	v2s get_text_dim(Font const& font, string const& text)
	{
		auto p = v2s(0,0); // current position (top-left)
		for (auto t: text)	
		{
			auto & g = res(font.base, t);			
			p[0] += g.adv; // move to next position
			++t;
		}
		return {p[0], font.height};
	}
	
	
	/// Render text at pos (top-left)
	b2s render_text(
		v2s pos,
		Font const& font, RGBA8 fg,
		string const& text		
	) {		
		auto p = pos; // current position (top-left)
		for (auto t: text)
		{
			auto & g = res(font.base, t);			
			render_aamask(font.texu, p + g.delta, g, fg);			
			p[0] += g.adv; // move to next position
			++t;
		}
		return {pos, v2s(p[0] - pos[0], font.height)};
	}
		
	/// Render text aligned inside box with background
	b2s render_text(
		v2s pos, v2s dim, v2f align,
		Font const& font, RGBA8 fg, RGBA8 bg,
		string const& text		
	) {
		if (bg.a != 0) {
			render_fill({pos, dim}, bg);			
		}
		return render_text(
			calc_align({pos, dim}, get_text_dim(font, text), align),
			font, fg,
			text
		);
	}

}
