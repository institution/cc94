#pragma once

#include "base.hpp"
#include "conf.hpp"
#include "layout.hpp"
#include <string>
#include "align.hpp"
#include "rcparser.hpp"
#include "../front/front.hpp"

namespace col {
	using front::TexUnit;

	TexUnit const
		TEXU_DIRTY = 0,
		TEXU_SPRITES = 1,
		TEXU_FONT0 = 2;
				
	
	using ResId = uint16_t;

	ResId const
		ZZ = 0,
		WOODTILE = 20,
		SANDTILE = 21,

		FONT_TINY = 400,
		FONT_2 = 700,		
		
		BUILDING = 1000,
		ICON = 2000,
		PHYS = 3000,
		TERRAIN = 4000;

	extern front::Front win;
	
	struct Sprite
	{
		aabb2f box{0,0,0,0}; // normalized rect
		v2s dim{0,0};
		
		v2s delta{0,0};
		int16_t adv{0};

		//Sprite() = default;		
	};

	b2s get_pixel_box(Sprite const& s);
	

	inline std::ostream & operator<<(std::ostream & o, Sprite const& s)
	{
		print(o, "Sprite(%||, %||)", s.dim, s.box);
		return o;
	}



	struct RCReader
	{
		RCParser p;
		ResId base{0};
		v2s tex_dim;

		void open(Path const& path_rc) { p.open(path_rc); }

		void read_rc();
		void read_sprite();
		void read_font();
	};

	struct Font
	{
		ResId base{0};
		int16_t height{0};
		TexUnit texu{0};
	};

	

	extern ext::darray1<Sprite, ResId> sprites;
	extern ext::darray1<Texture, ResId> textures;
	extern ext::darray1<Font, ResId> fonts;
	

	void load_resources();
	void free_resources();
	

	inline Font const& get_font(uint16_t n) { return fonts.at(n); }
	inline Font const& font_tiny() { return get_font(0); }
	


	inline Sprite const& res(ResId id) {
		return sprites.at(id);
	}
	
	inline Sprite const& res(ResId base, ResId id) {
		return sprites.at(base + id);
	}
	
}
