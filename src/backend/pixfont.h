#ifndef PIXFONT_H
#define PIXFONT_H

#include <map>
#include "backend.h"


namespace backend {


	struct PixFont{

		struct PixGlyph{
			backend::Box rect;
		};

		void render_glyph(
			Surface &trg,
			int const& t_x,
			int const& t_y,
			PixGlyph const& g,
			backend::Color const& fgcol
			//sf::Color const& bgcol
		) const {

			int g_x = g.rect.pos[0];
			int g_y = g.rect.pos[1];

			for (int i = 0; i < g.rect.dim[0]; ++i) {
				for (int j = 0; j < g.rect.dim[1]; ++j){

					auto c = this->img.get_pixel({g_x + i, g_y + j});
					if (c.a) {
						trg.set_pixel({t_x + i, t_y + j}, fgcol);
					}
					else {
						//trg.setPixel(t_x + i, t_y + j, bgcol);
					}

				}
			}
		}





		struct PixReader {

			struct OutOfBound{};

			Surface const* img;
			PixReader(Surface const& img) {
				this->img = &img;
			}

			Color getpix(uint16_t i, uint16_t j) {
				auto dim = img->get_dim();
				if (i >= dim[0] || j >= dim[1]) {
					throw OutOfBound();
				}
				return img->get_pixel({i,j});
			}
		};


		std::map<uint16_t, PixGlyph> glyphs;
		Texture tex;
		Surface img;
		int height{-1};

		
		void colorize(Color v) {
			auto ind = img.get_pixel({0,1});
			v2i dim = get_dim(img);
			for (int i=0; i<dim[0]; ++i) {
				for (int j=0; j<dim[1]; ++j) {
					auto c = img.get_pixel({i,j});
					if (c != ind) {
						img.set_pixel({i,j}, {v.r, v.g, v.b, c.a});
					}					
				}				
			}
					
		}

		int get_height() const {
			return height;
		}

		void load(backend::Back & back, std::string const& path, Color const& col, bool verbose=0) {

			img.load_png(path);
			
			colorize(col);

			auto ind = img.get_pixel(img.get_dim() - v2i(1,1));

			PixReader r(img);

			uint16_t i = 0, j = 0;
			uint16_t i_begin = 0, i_end = 0;
			uint16_t j_begin = 0, j_end = 0;

			uint16_t char_code = 0;

			while (1) try {
				i = 0;

				// find v.ind
				while (r.getpix(0,j) != ind) ++j;
				j_begin = j;

				// read v.ind
				while (r.getpix(0,j) == ind) ++j;
				j_end = j;
				
				if (verbose)
					print("vert indicator found = %|| %||\n", j_begin, j_end);

				j = j + 1;

				while (1) try {
					// find horizontal indicator
					while (r.getpix(i,j) != ind) ++i;
					i_begin = i;

					// read h.ind
					while (r.getpix(i,j) == ind) ++i;
					i_end = i;
					
					if (verbose)
						print("horiz indicator found = %|| %||\n", i_begin, i_end);

					auto &g = glyphs[char_code];
					g.rect.pos[0] = i_begin;
					g.rect.pos[1] = j_begin;
					g.rect.dim[0] = i_end - i_begin;
					g.rect.dim[1] = j_end - j_begin;
					
					height = g.rect.dim[1];
					
					if (verbose)
						print("letter saved %||; pos=%||; dim=%||\n", char(char_code), g.rect.pos+v2i(1,1), g.rect.dim);
					
					if (char(char_code) == '_') {
						print("\n");
					}
					++char_code;
				}
				catch (PixReader::OutOfBound) {
					break;
				}
			}
			catch (PixReader::OutOfBound){
				break;
			}

			back.load_surface(tex, img);

		}


	};

}

#endif
