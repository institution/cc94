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

	/** Render text at box with aligment 
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
		
		using Mode = int8_t;
		Mode const
			Link = 1,
			Text = 2;
		
		Front & win;
		Console & con;
		
		b2s box;
		v2s cpos;
		
		PixFont const& font;
		Style const& style;		
		
		
		TextRend2(Front & win, Console & con, b2s box, PixFont const& font, Style const& style):
			win(win),
			con(con),
			box(box),
			font(font),
			style(style)
		{
			cpos = box.pos;
		}
		
		void render_text(string const& text);
		void render_link(string const& text, Action a);
		void render(Mode what, string const& text, Action a) {
			if (what == Link) {
				render_link(text, a);
			}
			else {
				render_text(text);
			}
		}
		void render(Mode what, string const& text) {
			render_text(text);
		}
		
	};
	
	
	
	
}

