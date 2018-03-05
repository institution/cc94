#include "render_func.hpp"


namespace col {

	/// Return approx text dim
	v2s approx_text_dim(Font const& font, int16_t x, int16_t y)
	{
		return v2s(font.width * x, font.height * y);
	}

	/// Return text dim
	v2s get_text_dim(Font const& font, string const& text)
	{
		auto p = font.pad;
		for (auto t: text)	
		{
			auto & g = res(font.base, t);			
			p[0] += g.adv; // move to next position
			++t;
		}
		p += font.pad;
		auto end = p + v2s(0, font.height);
		return end;
	}
	
	/// Render text at pos (top-left)
	b2s render_text(
		v2s pos,
		Font const& font, RGBA8 fg,
		string const& text		
	) {		
		auto p = pos + font.pad; // current position (top-left)
		for (auto t: text)
		{
			auto & g = res(font.base, t);
			render_aamask(font.texu, font.get_glyph_rpos(p, g), g, fg);			
			p[0] += g.adv; // move to next position
			++t;
		}
		p += font.pad;
		auto end = p + v2s(0, font.height);
		return {pos, end - pos};
	}

	/// Render text
	b2s render_text(
		b2s box, v2f align,
		Font const& font,
		RGBA8 fg, RGBA8 bg,
		string const& text		
	) {
		b2s tbox;
		tbox.dim = get_text_dim(font, text);
		tbox.pos = calc_align(box, tbox.dim, align);
		if (bg.a != 0) {
			render_fill(tbox, bg);
		}
		return render_text(
			tbox.pos,
			font, fg,
			text
		);
	}

	/// Render text
	b2s render_text(
		b2s box, v2f align,
		Font const& font,
		RGBA8 fg,
		string const& text		
	) {
		b2s tbox;
		tbox.dim = get_text_dim(font, text);
		tbox.pos = calc_align(box, tbox.dim, align);
		return render_text(
			tbox.pos,
			font, fg,
			text
		);
	}

}
