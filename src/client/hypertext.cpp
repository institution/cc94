#include "hypertext.hpp"

namespace col
{
	
	TextRend2 & TextRend2::link(string const& text, Action act) {
		b2s area;
		area.pos = cpos;
		auto text_box = render_text(win,
			cpos,
			font, style.hl,
			text
		);
		cpos = text_box.pos + text_box.dim - v2s(0, font.height);
		
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
	
	/** Render 2D text
		'\n' -- new line
		'\t' -- new column
	*/
	TextRend2 & TextRend2::text(string const& text_)
	{
		for (auto t: text_)
		{
			if (t == '\n') {
				// move to new line
				cpos[0] = box.pos[0];
				cpos[1] += font.height;				
			}
			else {				
				auto & g = font.glyphs.at(t);
				win.render_aamask(font.tex,
					cpos + v2s(g.bearing_x, font.ascender - g.bearing_y),
					{{g.pos_x, g.pos_y}, {g.width, g.height}},
					style.fg
				);
				cpos[0] += g.advance; // move to next position
			}			
			++t;
		}		
		return *this;
	}
	
}




	
