#pragma once

#include <string>
#include "align.hpp"
#include "base.hpp"
#include "../front/ftb.hpp"


namespace col{

	using std::string;

	struct Font
	{
		int16_t height;
		int16_t ascender;
		int16_t descender;
		ext::darray1<ftb::GlyphHead, uint32_t> glyphs;
		Texture tex;

		int16_t get_height() const { return height; }
		int16_t get_ascender() const { return ascender; }
		ftb::GlyphHead const& get_glyph(uint32_t code) const { return glyphs.at(code); }
	};

	/** Load font from ftb file
	*/
	void load_font_ftb(front::Front & win, Font & font, Path const& path);

	/** Return text dim
	*/
	v2s get_text_dim(Font const& font, string const& text);
	
	
	/** Render text at pos (top-left)
	*/
	b2s render_text(
		front::Front & win, v2s pos,
		Font const& font, RGBA8 fg,
		string const& text		
	);
		
	/** Render text aligned inside box with background
	*/
	b2s render_text(front::Front & win,
		v2s pos, v2s dim, v2f align,
		Font const& font, RGBA8 fg, RGBA8 bg,
		string const& text		
	);

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


	
	
	
	
	
}

