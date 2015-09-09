#include "textrend.h"


namespace col{
	
	

	/* 
	i -- start position
	return -- render dim of text line (from i to first '\n' or end of string)
	effect -- i will point to '\n' or after last char 
	*/
	v2i get_text_dim(backend::PixFont const& font, string const& text, size_t & i) {
		
		// render dim
		v2i r_dim = {0, font.get_height()};

		// current substring [i:j>
		size_t j = i;

		// sum of glyphs widths + horizontal sep
		int tadv = -font.adv;
		while (j < text.size()) {
			tadv += font.adv;
			auto c = text.at(j);
			if (c == '\n')
				break;
			auto& g = font.get_glyph(c);
			r_dim[0] += g.rect.dim[0];
			j += 1;
		}
		r_dim[0] += tadv;

		i = j;   // set i
		return r_dim;
	}


	v2i get_text_dim2(backend::PixFont const& font, string const& text) {
		v2i r_dim = {0, 0};    // render dim
		size_t i = 0;
		while (i < text.size()) {

			auto line_dim = get_text_dim(font, text, i);

			// max line width
			if (r_dim[0] < line_dim[0]) {
				r_dim[0] = line_dim[0];
			}

			// sum of heights
			r_dim[1] += line_dim[1];

			i += 1;
		}

		return r_dim;
	}


	b2i render_text_at(
		backend::Back &win,
		v2i const& pos,
		backend::PixFont const& font,
		backend::Color const& fgcol,
		backend::Color const& bgcol,
		string const& text
	) {
		v2i r_dim = get_text_dim(font, text);

		// render position
		v2i r_pos = pos;

		// render letters
		auto trg = backend::Surface(r_dim, bgcol);

		size_t i = 0;
		size_t j = text.size();

		v2i g_pos{0,0};
		for (auto k = i; k < j; ++k) {

			auto& g = font.glyphs.at(text.at(k));

			font.render_glyph(trg, g_pos, g, fgcol);

			g_pos[0] += g.rect.dim[0] + font.adv;
		}

		//print("%||\n", text);

		auto tex = win.make_texture(trg);
		win.render(tex, r_pos);

		return b2i(r_pos, trg.get_dim());
	}






	b2i render_text_at2(
		backend::Back &win,

		v2i const& r_pos,

		backend::PixFont const& font,
		backend::Color const& fgcol,
		backend::Color const& bgcol,

		string const& text
	) {
		v2i r_dim = get_text_dim2(font, text);

		auto height = font.get_height();

		// render letters
		auto trg = backend::Surface(r_dim, bgcol);

		size_t i = 0;
		size_t j = text.size();

		v2i g_pos{0,0};
		for (auto k = i; k < j; ++k) {

			auto c = text.at(k);
			if (c == '\n') {
				g_pos[0] = 0;
				g_pos[1] += height;
				continue;
			}

			auto& g = font.glyphs.at(c);
			font.render_glyph(trg, g_pos, g, fgcol);

			g_pos[0] += g.rect.dim[0] + font.adv;
		}

		auto tex = win.make_texture(trg);
		win.render(tex, r_pos);

		return b2i(r_pos, trg.get_dim());
	}

	
}
