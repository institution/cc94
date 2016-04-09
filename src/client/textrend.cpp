#include "textrend.hpp"


namespace col{
	
	

	/* 
	i -- start position
	return -- render dim of text line (from i to first '\n' or end of string)
	effect -- i will point to '\n' or after last char 
	*/
	v2s get_text_dim(PixFont const& font, string const& text, size_t & i) {
		
		// render dim
		v2s r_dim = {0, font.get_height()};

		// current substring [i:j>
		size_t j = i;

		// calculate sum of glyphs widths + horizontal sep
		
		int tadv = 0;  // total horizontal separation between letters
		while (j < text.size()) {
			auto c = text.at(j);
			if (c == '\n')
				break;
				
			tadv += font.adv;
			auto& g = font.get_glyph(c);
			r_dim[0] += g.rect.dim[0];
			j += 1;
		}
		tadv -= font.adv;  // one less
		//print("tadv=%||; text.size=%||;\n", tadv, text.size());
		r_dim[0] += tadv;
		
		i = j;   // set i
		return r_dim;
	}


	v2s get_text_dim2(PixFont const& font, string const& text) {
		
		v2s r_dim = {0, 0};    // render dim
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

		//print("get_text_dim2 %|| -> %||\n", text, r_dim);

		return r_dim;
	}


	/*b2i render_text_at(
		Back &win,
		v2i const& pos,
		PixFont const& font,
		Color const& fgcol,
		Color const& bgcol,
		string const& text
	) {
		v2i r_dim = get_text_dim(font, text);

		// render position
		v2i r_pos = pos;

		//print("render_text_at r_dim=%||; text=%||\n", r_dim, text);

		// may be empty string
		if (r_dim[0] > 0 and r_dim[1] > 0)
		{
			// render letters
			auto trg = Surface(r_dim, bgcol);

			size_t i = 0;
			size_t j = text.size();

			v2i g_pos{0,0};
			for (auto k = i; k < j; ++k) {

				auto& g = font.glyphs.at(text.at(k));
				
				//print(std::cerr, "render_text_at: g_pos=%||; r_dim=%||; char=%||;\n", g_pos, r_dim, text.at(k));
				
				font.render_glyph(trg, g_pos, g, fgcol);

				g_pos[0] += g.rect.dim[0] + font.adv;
				
				if (g_pos[0] < 0) {
					print(std::cerr, "WARNING: text rendering position out of bounds; charcode=%||; g.rect.dim=%||; text=%||\n", int(text.at(k)), g.rect.dim, text);
					g_pos[0] = 0;
				}
			}

			//print("%||\n", text);

			auto tex = win.make_texture(trg);
			win.render(tex, r_pos);
			
			return b2i(r_pos, trg.get_dim());
		}
		else {
			return b2i(r_pos, r_dim);
		}
		
		
	}*/

	b2s render_text_at(
		Front & win,
		v2s pos,
		PixFont const& font,
		Color fg,
		Color bg,
		string const& text
	) {
		auto r_dim = get_text_dim(font, text);

		// render position
		auto g_pos = pos;
		
		// render background
		if (bg.a != 0) {
			win.render_fill({pos, r_dim}, bg);			
		}

		// assert(fgcol == font.color);

		// may be empty string
		if (r_dim[0] > 0 and r_dim[1] > 0)
		{
			size_t i = 0;
			size_t j = text.size();

			
			for (auto k = i; k < j; ++k) {

				auto& g = font.glyphs.at(text.at(k));
				
				//print(std::cerr, "render_text_at: g_pos=%||; r_dim=%||; char=%||;\n", g_pos, r_dim, text.at(k));
				
				font.render_glyph(win, g_pos, g, fg);

				g_pos[0] += g.rect.dim[0] + font.adv;

			}
		}
		
		return b2s{pos, r_dim};
	}





	b2s render_text_at2(
		Front & win,

		v2s r_pos,

		PixFont & font,
		Color fg,
		Color bg,

		string const& text
	) {
		v2s r_dim = get_text_dim2(font, text);
		//print("text dim = %||\n", r_dim);

		if (r_dim[0] > 0 and r_dim[1] > 0) {

			auto height = font.get_height();

			size_t i = 0;
			size_t j = text.size();

			v2s g_pos = r_pos;
			for (auto k = i; k < j; ++k) {

				auto c = text.at(k);
				if (c == '\n') {
					g_pos[0] = r_pos[0];
					g_pos[1] += height;
					continue;
				}
				//print("letter=%||\n", c);
				auto& g = font.glyphs.at(c);
				
				//print("render_text_at2 pos=%|| char=%||\n", g_pos, c);
				font.render_glyph(win, g_pos, g, fg);

				g_pos[0] += g.rect.dim[0] + font.adv;
			}
		}
		return b2s{r_pos, r_dim};
	}

	
}
