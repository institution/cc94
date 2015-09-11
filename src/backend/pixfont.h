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

		Color fg, bg, nt;

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
		if (c != fg and c != bg and c != nt) {
			throw Error("invalid color; pos=%||; col=%||", p, c);
		}
		return c != nt;
	}

	inline
	bool PixFontLoader::is_first(Surface const& img, v2i const& pos) {
		return
			is_glyph(img, pos) and
			!is_glyph(img, pos - v2i(1,0)) and
			!is_glyph(img, pos - v2i(0,1));
	}

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
				for (int j = 0; j < g.rect.dim[1]; ++j) {
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
