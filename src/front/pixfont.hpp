#pragma once
#include <unordered_map>
#include "front.hpp"

namespace front {

	struct PixFont;

	void load_pixfont(Front & front, PixFont & font, filesys::Path const& path_png);

	struct PixFont{
		
		struct PixGlyph{
			b2s rect;
		};

		std::unordered_map<uint32_t, PixGlyph> glyphs;
		
		Texture img;
		
		int height{-1};
		int adv{0};
		
		PixFont() = default;
		
		PixFont(Front & front, filesys::Path const& path_png, int adv) {
			load(front, path_png, adv);
		}
		
		void render_glyph(
			Front & front,
			v2s r_pos,
			PixGlyph g,
			Color fg
		) const {
			front.render_texture(img, r_pos, g.rect, fg);
		}
				
		PixGlyph const& get_glyph(uint32_t c) const {
			
			auto it = glyphs.find(c);
			if (it == glyphs.end()) {
				return glyphs.at('@');
			}
			return (*it).second;
		}

		int get_height() const { 
			return height; 
		}
		
		int get_width() const { 
			return height/2; 
		}

		void load(Front & front, filesys::Path const& path_png, int adv) {
			load_pixfont(front, *this, path_png);
			this->adv = adv;
		}


	};


}
