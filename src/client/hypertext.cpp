#include "hypertext.hpp"

namespace col
{
	
	Hypertext & Hypertext::link(string const& text, Action act) {
		b2s area;
		area.pos = cpos;
		auto text_box = render_text(cpos, font, style.hl, text);
		
		cpos = text_box.pos + text_box.dim - v2s(0, font.height);
		
		area.dim = v2s(cpos[0] - area.pos[0], font.height);
		
		con.on({EventPress, KeyLMB, area.pos, area.dim}, act);
		return *this;
	}
	
	Hypertext & Hypertext::lf() {
		cpos[0] = box.pos[0] +  + font.pad[0];
		cpos[1] += font.height;
		return *this;
	}
	
	/** Render 2D text
		'\n' -- new line
		'\t' -- new column
	*/
	Hypertext & Hypertext::text(string const& text_)
	{
		for (auto t: text_)
		{
			if (t == '\n') {
				lf();
			}
			else {				
				auto & g = res(font.base, t);				
				render_aamask(font.texu, font.get_glyph_rpos(cpos, g), g, style.fg);
				cpos[0] += g.adv; // move to next position
			}			
		}		
		return *this;
	}
	
}




	
