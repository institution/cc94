#include "hypertext.hpp"


namespace col{

	void read_section_id(std::ifstream & f, char const* mark)
	{
		char cs[5];
		ftb::read(f, cs, 4);
		cs[4] = '\0';

		if (strcmp(mark, cs) != 0) {
			ext::fail("ftb: %||: invalid section_id at position %||: %||'\n", mark, f.tellg(), cs);
		}		
	}
	
	/** Load font from ftb file
	*/
	void load_font_ftb(front::Front & win, Font & font, Path const& path)
	{
		/**
		FTB File Format Specification
		-----------------------------
		File is in little endian encoding.

		uint32_t file_id; // 'FTBF' 0x46425446
		uint32_t version; // 1

		uint32_t section_id; // 'HEAD' 0x44414548
		int16_t height;
		int16_t ascender;
		int16_t descender;
		
		uint32_t section_id; // 'GLYP' 0x50594c47
		uint32_t nglyph; 
		struct {
			uint32_t code; // less than 256 in version 1
			int16_t bearing_x;
			int16_t bearing_y;
			int16_t advance;
			int16_t width;
			int16_t height;
			int16_t pos_x;
			int16_t pos_y;
		} glyphs[nglyph];

		uint32_t section_id; // 'AAMA' 0x414d4141
		int16_t dim_x;
		int16_t dim_y;
		uint8_t aamask[dim_x][dim_y];
		*/
		
		std::ifstream f(path, std::ios::in | std::ios::binary);
		if (not f.is_open()) {
			ext::fail("ERROR: cannot open file: %||\n", path);
		}
		
		read_section_id(f, "FTBF");		
		uint32_t version;
		ftb::read(f, version);
		if (version != 1) {
			ext::fail("unknown version number for a ftb file: %||", version);
		}		

		read_section_id(f, "HEAD");
		ftb::read(f, font.height); 
		ftb::read(f, font.ascender);
		ftb::read(f, font.descender);

		read_section_id(f, "GLYP");	
		uint32_t nglyph;
		ftb::read(f, nglyph);		
		font.glyphs.resize(256);
		for (uint32_t i = 0; i < nglyph; ++i)
		{
			ftb::GlyphHead gh;
			ftb::read(f, gh);		
			font.glyphs.at(gh.code) = gh;
		}

		read_section_id(f, "AAMA");	
		ImageA8 mask;
		v2s dim;
		ftb::read(f, dim[0]);
		ftb::read(f, dim[1]);
		mask.resize(dim);		
		for (size_t i = 0; i < mask.size(); ++i)
		{
			ftb::read(f, mask.at(i).a);
		}
		font.tex = win.make_texture(mask);		
		f.close();
	}

	/** Return text dim
	*/
	v2s get_text_dim(Font const& font, string const& text)
	{
		auto p = v2s(0,0); // current position (top-left)
		for (auto t: text)	
		{
			auto & g = font.glyphs.at(t);
			p[0] += g.advance; // move to next position
			++t;
		}
		return {p[0], font.height};
	}
	
	
	/** Render text at pos (top-left)
	*/
	b2s render_text(
		front::Front & win, v2s pos,
		Font const& font, RGBA8 fg,
		string const& text		
	) {		
		auto p = pos; // current position (top-left)
		for (auto t: text)
		{
			auto & g = font.glyphs.at(t);
			win.render_aamask(font.tex,
				p + v2s(g.bearing_x, font.ascender - g.bearing_y),
				{{g.pos_x, g.pos_y}, {g.width, g.height}},
				fg
			);
			p[0] += g.advance; // move to next position
			++t;
		}
		return {pos, v2s(p[0] - pos[0], font.height)};
	}
		
	/** Render text aligned inside box with background
	*/
	b2s render_text(front::Front & win,
		v2s pos, v2s dim, v2f align,
		Font const& font, RGBA8 fg, RGBA8 bg,
		string const& text		
	) {
		if (bg.a != 0) {
			win.render_fill({pos, dim}, bg);			
		}
		return render_text(
			win, calc_align({pos, dim}, get_text_dim(font, text), align),
			font, fg,
			text
		);
	}





		
}
