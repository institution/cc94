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

		bool is_glyph(Surface const& img, v2i const& p);
		bool is_first(Surface const& img, v2i const& pos);

		v2i find_glyph_pos(Surface const& img, v2i const& pp);
		v2i find_glyph_dim(Surface const& img, v2i const& p);

		char next_char(std::ifstream & is) const;

		void load(backend::Back & back, PixFont & font, std::string const& path_png);
	};

	inline
	bool PixFontLoader::is_glyph(Surface const& img, v2i const& p) {
		auto c = img.get_pixel(p);
		return c.r == 0 and c.g == 0 and c.b == 0;
	}

	inline
	bool PixFontLoader::is_first(Surface const& img, v2i const& pos) {
		return
			is_glyph(img, pos) and
			!is_glyph(img, pos - v2i(1,0)) and
			!is_glyph(img, pos - v2i(0,1));
	}

	

	struct PixFont{
		
		using ColorIndex = size_t;
		
		struct PixGlyph{
			backend::Box rect;
		};

		std::map<uint16_t, PixGlyph> glyphs;
		
		Surface img;
		int height{-1};
		int adv{0};
		
		std::array<Texture, 12> textures;
		
		void colorize(Back & back, ColorIndex index, Color const& fg);
		

		PixFont() = default;
		
		PixFont(backend::Back & back, std::string const& path_png, int adv) {
			load(back, path_png, adv);
		}

		


		void render_glyph(
			Back &back,
			v2i const& r_pos,
			PixGlyph const& g,
			ColorIndex fgcol
		) const {
			back.render_clip(textures.at(fgcol), r_pos, g.rect);			
		}
		
		void render_glyph(
			Surface &trg,
			v2i const& r_pos,
			PixGlyph const& g,
			backend::Color const& fgcol
		) const;

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

		void load(backend::Back & back, std::string const& path_png, int adv) {
			PixFontLoader x;
			x.load(back, *this, path_png);
			this->adv = adv;
		}


	};

}

#endif
