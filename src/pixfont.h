#ifndef PIXFONT_H
#define PIXFONT_H


#include "col.h"
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

namespace col {
	
	struct PixFont{

		struct PixGlyph{
			sf::IntRect rect;
		};

		struct PixReader {

			struct OutOfBound{};

			sf::Image const* img;
			PixReader(sf::Image const& img) {
				this->img = &img;
			}

			sf::Color getpix(uint16 i, uint16 j) {
				if (i >= img->GetWidth() || j >= img->GetHeight()) {
					throw OutOfBound();
				}
				return img->GetPixel(i,j);
			}
		};
		

		map<uint16, PixGlyph> glyphs;
		sf::Image img;

		PixFont() {}

		void load(string const& path) {
			
			if (!img.LoadFromFile(path)) {
				throw std::runtime_error("cant't load font file: " + path);
			}
			img.SetSmooth(false);
			
			auto ind = img.GetPixel(0,1);

			PixReader r(img);
			
			uint16 i = 0, j = 0;
			uint16 i_begin = 0, i_end = 0;
			uint16 j_begin = 0, j_end = 0;
			
			uint16 char_code = 0;
			
			while (1) try {
				i = 0;
				
				// find v.ind
				while (r.getpix(0,j) != ind) ++j;
				j_begin = j;

				// read v.ind				
				while (r.getpix(0,j) == ind) ++j;
				j_end = j;

				j = j + 1;

				while (1) try {
					// find horizontal indicator
					while (r.getpix(i,j) != ind) ++i;
					i_begin = i;

					// read h.ind
					while (r.getpix(i,j) == ind) ++i;
					i_end = i;

					auto &g = glyphs[char_code];
					g.rect.Top = j_begin;
					g.rect.Bottom = j_end;
					g.rect.Left = i_begin;
					g.rect.Right = i_end;
					
					++char_code;					
				}
				catch (PixReader::OutOfBound) {
					break;
				}
			} 
			catch (PixReader::OutOfBound){
				break;
			}
			
			
		}


	};

}

#endif
