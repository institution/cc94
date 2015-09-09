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
		
		bg = Color{255,255,0,255};
		nt = Color{0,0,0,0};
					
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
	
}
