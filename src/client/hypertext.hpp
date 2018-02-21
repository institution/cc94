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

	
	struct TextRend2
	{
		
		Front & win;
		Console & con;
		
		b2s box;
		v2s cpos; // cursor pos
		int8_t ccol{0}; // cursor column
		
		Font const& font;
		Style const& style;		
		
		
		array<int16_t, 8> tabs{120,120,120,120,120,120,120,120};
		array<int8_t, 8> align{-1,-1,-1,-1,-1,-1,-1,-1};
		
		v2s get_pos() const { return cpos; }
		
		int16_t get_height() const { return font.height; }
		
		v2s get_ddmenu_pos() const {
			// drop down menu position
			return get_pos() + v2s(0, get_height() + ly.line);		
		}

		
		
		void set_tabs(
			int16_t a0 = 120, int16_t a1 = 120, int16_t a2 = 120, int16_t a3 = 120, 
			int16_t a4 = 120, int16_t a5 = 120, int16_t a6 = 120, int16_t a7 = 120
		) {
			tabs = {a0,a1,a2,a3,a4,a5,a6,a7};
			for (size_t i = 1; i < tabs.size(); ++i)
			{
				tabs[i] += tabs[i-1];
			}
			
		}
		
		void set_align(
			int8_t a0 = -1, int8_t a1 = -1, int8_t a2 = -1, int8_t a3 = -1, 
			int8_t a4 = -1, int8_t a5 = -1, int8_t a6 = -1, int8_t a7 = -1
		) {
			align = {a0,a1,a2,a3,a4,a5,a6,a7};
		}
		
		
		
		TextRend2(Front & win, Console & con, b2s box, Font const& font, Style const& style):
			win(win),
			con(con),
			box(box),
			font(font),
			style(style)
		{
			cpos = box.pos;
			set_tabs();
		}
		
		TextRend2 & newline();
		TextRend2 & td();
		TextRend2 & tr();
		
		TextRend2 & text(string const& t);
		TextRend2 & link(string const& t, Action a);
		TextRend2 & link(string const& t, bool active, Action a) {
			if (active) {
				return link(t, a);
			}
			else {
				return text(t);
			}
		}
		
	
		
		
		
	};
}
