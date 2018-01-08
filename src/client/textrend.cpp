#include "textrend.hpp"


namespace col{
	



	/* 
	i -- start position
	return -- render dim of text line (from i to first '\n' or end of string)
	effect -- i will point to '\n' or after last char 
	*/
	v2s get_text_dim(PixFont const& font, string const& text, size_t & i) {
		
		// render dim
		auto r_dim = v2s(0, font.get_height());

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
		string const& text
	) {
		// current position
		auto g_pos = pos;
		
		// may be empty string
		if (text.size() > 0)
		{
			size_t i = 0;
			size_t j = text.size();

			
			for (auto k = i; k < j; ++k) {
				auto& g = font.get_glyph(text.at(k));
				
				//print(std::cerr, "render_text_at: g_pos=%||; r_dim=%||; char=%||;\n", g_pos, r_dim, text.at(k));

				font.render_glyph(win, g_pos, g, fg);

				g_pos[0] += g.rect.dim[0] + font.adv;

			}

			return b2s(pos, g_pos + v2s(0, font.get_height()));
		}
		else {
			return b2s{pos, {0,0}};
		}
		
		
	}


	v2s render_text_at2(
		Front & win,

		b2s box,   // lines should wrap in this box
		v2s pos,   // pos to start inside box

		PixFont const& font,
		Color fg,
		
		string const& text
	);

	b2s render_text_at(
		Front & win,
		v2s pos,
		PixFont const& font,
		Style const& style,
		string const& text
	) {
		auto r_dim = get_text_dim(font, text);

		// render position
		auto g_pos = pos;
		
		// render background if any
		if (style.has_bg()) {
			win.render_fill({pos, r_dim}, style.bg);
		}

		bool highlight{false};

		// may be empty string
		if (r_dim[0] > 0 and r_dim[1] > 0)
		{
			size_t i = 0;
			size_t j = text.size();
			
			for (auto k = i; k < j; ++k) {

				auto c = text.at(k);
				if (style.has_hl() and c == '^') {
					highlight = !highlight;
					continue;
				}

				auto& g = font.get_glyph(c);
				
				//print(std::cerr, "render_text_at: g_pos=%||; r_dim=%||; char=%||;\n", g_pos, r_dim, text.at(k));

				if (not highlight) {
					font.render_glyph(win, g_pos, g, style.fg);
				}
				else {
					font.render_glyph(win, g_pos, g, style.hl);
				}

				g_pos[0] += g.rect.dim[0] + font.adv;

			}
		}
		
		return b2s{pos, r_dim};
	}


	
	
	TextRend2 & TextRend2::link(string const& text, Action act) {
		b2s area;
		area.pos = cpos;
		cpos = render_text_at2(win, box, cpos, font, style.hl, text);
		area.dim = v2s(cpos[0] - area.pos[0], font.get_height());
		
		con.on({EventPress, KeyLMB, area.pos, area.dim}, act);
		return *this;
	}
	
	TextRend2 & TextRend2::newline() {
		cpos[0] = box.pos[0];
		cpos[1] += font.get_height();
		ccol = 0;
		return *this;
	}
	
	TextRend2 & TextRend2::td() {
		cpos[0] = box.pos[0] + tabs.at(ccol);
		ccol += 1;
		return *this;
	}
	
	TextRend2 & TextRend2::tr() {
		cpos[0] = box.pos[0];
		cpos[1] += font.get_height();
		ccol = 0;
		return *this;
	}
	
	
	TextRend2 & TextRend2::text(string const& text_) {
		/// Render 2D text:
		/// new line -- newline
		/// tab -- new column
		
		if (text_.size() > 0) {

			auto height = font.get_height();

			size_t i = 0;
			size_t j = text_.size();

			
			for (auto k = i; k < j; ++k) {

				auto c = text_.at(k);
				
				if (c == '\n') {
					cpos[0] = box.pos[0];
					cpos[1] += height;
					ccol = 0;
					continue;
				}
				
				//print("letter=%||\n", c);
				auto & g = font.get_glyph(c);
				
				//print("render_text_at2 pos=%|| char=%||\n", g_pos, c);
				font.render_glyph(win, cpos, g, style.fg);

				cpos[0] += g.rect.dim[0] + font.adv;
			}
		}
		return *this;
	}

	

	v2s render_text_at2(
		Front & win,

		b2s box,   // lines should wrap in this box
		v2s pos,   // pos to start inside box

		PixFont const& font,
		Color fg,
		
		string const& text
	) {
		//v2s r_dim = get_text_dim2(font, text);
		//print("text dim = %||\n", r_dim);

		v2s g_pos = pos;
		if (text.size() > 0) {

			auto height = font.get_height();

			size_t i = 0;
			size_t j = text.size();

			
			for (auto k = i; k < j; ++k) {

				auto c = text.at(k);
				if (c == '\n') {
					g_pos[0] = box.pos[0];
					g_pos[1] += height;
					continue;
				}
				//print("letter=%||\n", c);
				auto& g = font.get_glyph(c);
				
				//print("render_text_at2 pos=%|| char=%||\n", g_pos, c);
				font.render_glyph(win, g_pos, g, fg);

				g_pos[0] += g.rect.dim[0] + font.adv;
			}
		}
		return g_pos;		
	}














	v2s render_text_at2(
		Front & win,

		v2s r_pos,

		PixFont & font,
		Color fg,
		
		string const& text
	) {
		//v2s r_dim = get_text_dim2(font, text);
		//print("text dim = %||\n", r_dim);

		v2s g_pos = r_pos;
		if (text.size() > 0) {

			auto height = font.get_height();

			size_t i = 0;
			size_t j = text.size();

			
			for (auto k = i; k < j; ++k) {

				auto c = text.at(k);
				if (c == '\n') {
					g_pos[0] = r_pos[0];
					g_pos[1] += height;
					continue;
				}
				//print("letter=%||\n", c);
				auto& g = font.get_glyph(c);
				
				//print("render_text_at2 pos=%|| char=%||\n", g_pos, c);
				font.render_glyph(win, g_pos, g, fg);

				g_pos[0] += g.rect.dim[0] + font.adv;
			}
		}
		return g_pos;		
	}

	
}
