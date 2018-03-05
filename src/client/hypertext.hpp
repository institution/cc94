#pragma once
#include "render_func.hpp"
#include "console.hpp"

namespace col{

	
	struct Style
	{
		RGBA8 fg; // foregorund
		RGBA8 bg; // background
		RGBA8 hl; // highlight (highlighted font)
		RGBA8 sh; // shaded (disabled font)

		Style(RGBA8 fg, RGBA8 bg): fg(fg), bg(bg), hl(RGBA8(0,0,0,0)), sh(RGBA8(0,0,0,0)) {}
		Style(RGBA8 fg, RGBA8 bg, RGBA8 hl, RGBA8 sh): fg(fg), bg(bg), hl(hl), sh(sh) {}
		
		bool has_fg() const { return fg.a != 0; }
		bool has_bg() const { return bg.a != 0; }
		bool has_hl() const { return hl.a != 0; }
		bool has_sh() const { return sh.a != 0; }
	};

	
	struct Hypertext
	{
		Console & con;
		
		b2s box;
		v2s cpos; // cursor pos
		
		Font const& font;
		Style const& style;
		
		v2s get_pos() const { return cpos; }
		
		int16_t get_height() const { return font.height; }

		/// Return drop down menu position
		v2s get_ddmenu_pos() const {			
			return get_pos() + v2s(0, get_height() + ly.line_th);		
		}

		
		Hypertext(Console & con, b2s box, Font const& font, Style const& style):
			con(con),
			box(box),
			font(font),
			style(style)
		{
			cpos = box.pos + font.pad;
		}
		
		Hypertext & lf();  // line feed		
		Hypertext & text(string const& t);
		Hypertext & link(string const& t, Action a);
		Hypertext & link(string const& t, bool active, Action a)
		{
			if (active) {
				return link(t, a);
			}
			else {
				return text(t);
			}
		}
		
	
		
		
		
	};
}
