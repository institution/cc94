#pragma once

#include <string>
#include "align.hpp"
#include "base.hpp"

#include "console.hpp"

namespace col{

	using std::string;

	v2s get_text_dim(PixFont const& font, string const& text, size_t & i);


	struct Style {
		Color fg; // foregorund
		Color bg; // background
		Color hl; // highlight (highlighted font)
		Color sh; // shaded (disabled font)
		Style(Color fg, Color bg, Color hl, Color sh): fg(fg), bg(bg), hl(hl), sh(sh) {}
		Style(Color fg, Color bg): fg(fg), bg(bg), hl(Color(0,0,0,0)), sh(Color(0,0,0,0)) {}
		bool has_bg() const { return bg.a != 0; }
		bool has_hl() const { return hl.a != 0; }
		bool has_sh() const { return sh.a != 0; }
	};
	
	
	
	/** Return render dim of text line (trimmed by '\n')
	*/
	inline v2s get_text_dim(PixFont const& font, string const& text) {
		size_t i = 0;
		return get_text_dim(font, text, i);
	}

	/** Render text at position 
	*/
	b2s render_text_at(
		Front & win,
		v2s pos,
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
	);

	/** Render text at box with aligment and possibly background
	*/
	inline b2s render_text(
		Front &win,

		v2s pos,
		v2s dim,
		v2f align,

		PixFont const& font,
		Color fg,
		Color bg,

		string const& text
	) {
		/*if (bg != ColorNone) {
			win.render_fill(win, pos, dim, bg);
		}*/
		return render_text_at(win,
			calc_align(pos, dim, get_text_dim(font, text), align),
			font, Style(fg, bg),
			text
		);
	}
	
	
	/** Render text at box with aligment 
	*/
	inline b2s render_text(
		Front &win,

		v2s pos,
		v2s dim,
		v2f align,

		PixFont const& font,
		Color fg,
	
		string const& text
	) {
		return render_text_at(win,
			calc_align(pos, dim, get_text_dim(font, text), align),
			font, fg,
			text
		);
	}

	/** Return render dim of multiline text 
	*/
	v2s get_text_dim2(PixFont const& font, string const& text);
	

	/** Render multiline text at position
	*/
	v2s render_text_at2(
		Front & win,

		v2s r_pos,

		PixFont & font,
		Color fg,
	
		string const& text
	);
	
	struct TextRend2{
		
		Front & win;
		Console & con;
		
		b2s box;
		v2s cpos; // cursor pos
		int8_t ccol{0}; // cursor column
		
		PixFont const& font;
		Style const& style;		
		
		
		array<int16_t, 8> tabs{120,120,120,120,120,120,120,120};
		array<int8_t, 8> align{-1,-1,-1,-1,-1,-1,-1,-1};
		
		v2s get_pos() const { return cpos; }
		
		int get_height() const { return font.get_height(); }
		
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
		
		
		
		TextRend2(Front & win, Console & con, b2s box, PixFont const& font, Style const& style):
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

