#include "pixfont.h"

namespace backend {
	void PixFontLoader::load(backend::Back & back, PixFont & font, std::string const& path_png) {
		/* 
			This function expects 2 files:
			png: glyphs
			txt: coresponding letters			
		*/
		 
		auto & img = font.img;
		 
		// filesys maneuvers
		auto path_txt = format("%||.txt", path_png.substr(0, path_png.size()-4));			
		std::ifstream txt(path_txt);			
		img.load_png(path_png);
					
		print("%||\n", path_txt);
		
		fg = Color{ 85, 150,  52, 255};
		bg = Color{255, 255,   0, 255};
		nt = Color{178, 178, 178, 255};
					
		v2i pos(0,0);
		int line_height{-1};
		
		while (1) {				
			// find next glyph box
			pos = find_glyph_pos(img, pos);
			if (pos == None) {
				// no more glyphs
				break;
			}
			auto dim = find_glyph_dim(img, pos);
			
			// assign box to next letter glyph
			uint16_t char_code = next_char(txt);			
			auto &gly = font.glyphs[char_code];
			gly.rect  = backend::Box(pos, dim);
		
			line_height = dim[1];
			pos[0] += 1;
			
			//print("letter saved %||; pos=%||; dim=%||\n", char(char_code), gly.rect.pos, gly.rect.dim);
		}
		
		
		back.load_surface(font.tex, img);
		font.bg = bg;
		font.height = line_height;

	}
	
	v2i PixFontLoader::find_glyph_pos(Surface const& img, v2i const& pp) {
		auto d = img.get_dim();
		auto p = pp;
		// down
		while (p[1] < d[1]) {
			// right
			while (p[0] < d[0]) {
				
				if (is_first(img, p)) {
					return p;
				}
				++p[0];
			}
			p[0] = 0;
			++p[1];
		}
		return None;
	}
	
	v2i PixFontLoader::find_glyph_dim(Surface const& img, v2i const& p) {
			
		v2i q = p;
		
		// down
		while (is_glyph(img, q + v2i(0,1))) ++q[1];
		
		// right
		while (is_glyph(img, q + v2i(1,0))) ++q[0];
		
		return q - p + v2i(1,1);
	}
	
	char PixFontLoader::next_char(std::ifstream & is) const {
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
		throw Error("load font: eof while scanning txt file");
	}

}
