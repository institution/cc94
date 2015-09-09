#ifndef PIXFONT_H
#define PIXFONT_H

#include <map>
#include <fstream>
#include "backend.h"
#include "error.h"


namespace backend {

	struct PixFont;

	struct PixFontLoader{
		
		v2i const None{-1,-1};
		
		Color nt, bg;
		
		bool is_glyph(Surface const& img, v2i const& p) {
			return img.get_pixel(p) != nt;
		}
		
		bool is_first(Surface const& img, v2i const& pos) {
			
			return 
				is_glyph(img, pos) and 
				!is_glyph(img, pos - v2i(1,0)) and 
				!is_glyph(img, pos - v2i(0,1));
		}
		
		v2i find_glyph_pos(Surface const& img, v2i const& pp) {
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
		
		v2i find_glyph_dim(Surface const& img, v2i const& p) {
			
			v2i q = p;
			
			// down
			while (is_glyph(img, q + v2i(0,1))) ++q[1];
			
			// right
			while (is_glyph(img, q + v2i(1,0))) ++q[0];
			
			return q - p + v2i(1,1);
		}
		
		
		char next_char(std::ifstream & is) const {			
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
			error("load font: eof while scanning txt file");
			throw Error();
		}
		
		void load(backend::Back & back, PixFont & font, std::string const& path_png);

		
	};




	struct PixFont{

		struct PixGlyph{
			backend::Box rect;
		};

		std::map<uint16_t, PixGlyph> glyphs;
		Texture tex;
		Surface img;
		int height{-1};
		Color bg;
		
		int adv{0};


		void render_glyph(
			Surface &trg,
			v2i const& r_pos,
			PixGlyph const& g,
			backend::Color const& fgcol			
		) const {

			auto g_pos = g.rect.pos;
						
			for (int i = 0; i < g.rect.dim[0]; ++i) {
				for (int j = 0; j < g.rect.dim[1]; ++j){
					
					auto c = this->img.get_pixel(g_pos + v2i(i,j));
					if (c != bg) {
						trg.set_pixel(r_pos + v2i(i,j), fgcol);
					}					
				}
			}
		}
		
		PixGlyph const& get_glyph(uint16_t c) const {
			return glyphs.at(c);
			/*auto it = glyphs.find(c);
			if (it == glyphs.end()) {
				
			}
			return *it.second;*/
		}
		
		int get_height() const {
			return height;
		}

		void load(backend::Back & back, std::string const& path_png) {
			PixFontLoader x;
			x.load(back, *this, path_png);
		}
		

	};

}

#endif
