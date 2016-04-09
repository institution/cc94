#include <fstream>
#include "pixfont.hpp"

namespace front {

	bool is_glyph(Image const& img, v2s pos, Color frame_color) {
		return img(pos) != frame_color;	
	}

	bool is_first(Image const& img, v2s pos, Color fc) {
		return
			is_glyph(img, pos, fc)
			and not is_glyph(img, pos - v2s(1,0), fc)
			and not is_glyph(img, pos - v2s(0,1), fc);
	}


	v2s find_glyph_pos(Image const& img, v2s s_pos, Color fc) {
		auto d = img.get_dim();
		auto p = s_pos;
		
		// down
		while (p[1] < d[1]) {
			// right
			while (p[0] < d[0])
			{
				if (is_first(img, p, fc)) {
					return p;
				}
				++p[0];
			}
			p[0] = 0;
			++p[1];
		}
		// not found
		return v2s(-1,-1);
	}

	v2s find_glyph_dim(Image const& img, v2s g_pos, Color fc) {
			
		auto q = g_pos;
		
		// down
		while (is_glyph(img, q + v2s(0,1), fc)) ++q[1];
		
		// right
		while (is_glyph(img, q + v2s(1,0), fc)) ++q[0];
		
		return q - g_pos + v2s(1,1);
	}


	char next_char(std::ifstream & is) {
		while (!is.eof()) {
			char c;
			is.get(c);
			//print("'%||'\n", c);
			switch (c) {
				case '\n':
				case '\r':
					continue;
			}
			return c;
		}
		ext::fail("PixFontLoader: next_char: eof while scanning lst file");
	}

	void load_pixfont(Front & front, PixFont & font, filesys::Path const& path_png) {
		/* 
			This function expects 2 files:
			png: glyphs
			lst: coresponding letters			
		*/
		 
		// filesys maneuvers
		auto path_lst = format("%||.lst", path_png.substr(0, path_png.size()-4));			
		std::ifstream lst(path_lst);
		//print("%||\n", path_lst);

		Image img = load_png(path_png);
						
		v2s g_pos(0,0);
		int16_t line_height{-1};

		auto fc = img(g_pos);
		
		while (1) {				
			// find next glyph box
			g_pos = find_glyph_pos(img, g_pos, fc);
			if (g_pos == v2s(-1,-1)) {
				// no more glyphs
				break;
			}
			auto g_dim = find_glyph_dim(img, g_pos, fc);
			
			// assign box to next letter glyph
			uint32_t char_code = next_char(lst);			
			auto &gly = font.glyphs[char_code];
			gly.rect  = b2s(g_pos, g_dim);
		
			line_height = g_dim[1];
			g_pos[0] += 1;
			
			//print("glyph saved %||; pos=%||; dim=%||\n", char(char_code), gly.rect.pos, gly.rect.dim);
		}
		
		font.height = line_height;

		font.img = front.make_texture(img);
	}

}



