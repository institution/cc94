#include "pixfont.h"

namespace backend {
	void PixFontLoader::load(backend::Back & back, PixFont & font, std::string const& path_png) {
		/* 
			This function expects 2 files:
			png: glyphs
			lst: coresponding letters			
		*/
		 
		auto & img = font.img;
		 
		// filesys maneuvers
		auto path_lst = format("%||.lst", path_png.substr(0, path_png.size()-4));			
		std::ifstream lst(path_lst);			
		img.load_png(path_png);
					
		print("%||\n", path_lst);
					
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
			uint16_t char_code = next_char(lst);			
			auto &gly = font.glyphs[char_code];
			gly.rect  = backend::Box(pos, dim);
		
			line_height = dim[1];
			pos[0] += 1;
			
			print("glyph saved %||; pos=%||; dim=%||\n", char(char_code), gly.rect.pos, gly.rect.dim);
		}
		
		font.height = line_height;

	}
	
	void PixFont::colorize(Back & back, ColorIndex index, Color const& fg) {
		auto dim = img.get_dim();
		for (int j = 0; j < dim[1]; ++j) {
			for (int i = 0; i < dim[0]; ++i) {
				auto c = img.get_pixel({i,j});
				img.set_pixel({i,j}, Color(fg.r, fg.g, fg.b, c.a));
			}
		}
		back.load_surface(textures.at(index), img);
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
		throw Error("ERROR: PixFontLoader::next_char: eof while scanning lst file");
	}
	
	

	uint8_t blend(uint16_t fg, uint16_t bg, uint16_t alfa) {		
		auto x = (fg * alfa + bg * (255 - alfa)) / 255;
		assert(0 <= x);
		assert(x < 256);
		return x;
	}

	
	
	void PixFont::render_glyph(
		Surface &trg,
		v2i const& r_pos,
		PixGlyph const& g,
		backend::Color const& fgcol
	) const {

		auto g_pos = g.rect.pos;
		
		//print("g.rect %|| %||\n", g.rect.pos, g.rect.dim);
		//print("rpos %||\n", r_pos);

		for (int i = 0; i < g.rect.dim[0]; ++i) {
			for (int j = 0; j < g.rect.dim[1]; ++j) {
				auto alfa = this->img.get_pixel(g_pos + v2i(i,j)).a;
				
				//print("r_pos = %||\n", r_pos + v2i(i,j));
				auto bgcol = trg.get_pixel(r_pos + v2i(i,j));
									
				backend::Color c;
				if (alfa) {
					c.r = (uint16_t(fgcol.r) * uint16_t(alfa)) / 255;
					c.g = (uint16_t(fgcol.g) * uint16_t(alfa)) / 255;
					c.b = (uint16_t(fgcol.b) * uint16_t(alfa)) / 255;
					c.a = 255;
				}
				else {
					c = bgcol;
				}
				
				/*
				c.r = blend(fgcol.r, bgcol.r , alfa);
				c.g = blend(fgcol.r, bgcol.r , alfa);
				c.b = blend(fgcol.r, bgcol.r , alfa);
				c.a = uint8_t(uint16_t(alfa) + (uint16_t(bgcol.a) * (uint16_t(255) - uint16_t(alfa)))/255);
				*/
				 
				
				trg.set_pixel(r_pos + v2i(i,j), c);
				
			}
		}
	}

}
