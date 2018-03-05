#include "res.hpp"



namespace col {

	front::Front win;


	ext::darray1<Sprite, ResId> sprites;
	ext::darray1<Texture, ResId> textures;
	ext::darray1<Font, ResId> fonts;

	/// Return sprite box on texture in pixels
	b2s get_pixel_box(Sprite const& s)
	{
		v2f ratio(0,0);
		v2f df = s.box.dim();
		if (df[0] != 0) {
			ratio[0] = float(s.dim[0]) / df[0];
		}
		if (df[1] != 0) {
			ratio[1] = float(s.dim[1]) / df[1];
		}
		auto p = vmul<float_t>(s.box.pos, ratio);
		auto d = vmul<float_t>(df, ratio); // this is s.dim
		return b2s(v2s(p),v2s(d));
	}

	/// Return minimal dim that can hold any uppercase letter
	v2s get_upper_dim(Font const& font)
	{
		int16_t w{0};
		int16_t h{0};
		for (uint8_t i = 65; i <= 90; ++i)
		{
			auto dim = res(font.base, i).dim;
			if (dim[0] > w) { w = dim[0]; }
			if (dim[1] > h) { h = dim[1]; }
		}
		return v2s(w,h);
	}
		
	void load_resources(Path const& path)
	{		
		textures.resize(2);
		sprites.resize(5000);
		fonts.resize(1);

		

		// texture 0 - game sprites
		print("INFO: \"%||\"\n", path / "texture0.png");
		textures.at(0) = front::make_texture(
			front::load_png_RGBA8(path / "texture0.png")
		);

		print("INFO: \"%||\"\n", path / "texture0.rc");
		RCReader rc;
		rc.open(path / "texture0.rc");
		rc.base = 0;
		rc.tex_dim = textures.at(0).dim;
		rc.read_rc();

		
		// texture 1 - font 0 "tiny"
		print("INFO: \"%||\"\n", path / "texture1.png");
		textures.at(1) = front::make_texture(
			front::load_png_RGBA8(path / "texture1.png")
		);		

		print("INFO: \"%||\"\n", path / "texture1.rc");
		rc.open(path / "texture1.rc");
		rc.base = FONT_TINY;
		rc.tex_dim = textures.at(1).dim;
		rc.read_rc();

		auto & font = fonts.at(0);
		font.texu = TEXU_FONT0;
		font.base = FONT_TINY;
		font.upper_dim = get_upper_dim(font);


		front::bind_texture(TEXU_SPRITES, textures.at(0));
		front::bind_texture(TEXU_FONT0, textures.at(1));		
	}

	void free_resources()
	{
		sprites.clear();
		textures.clear();
	}


	void RCReader::read_sprite()
	{		
		Sprite & sprite = sprites.at(base + p.read_uint16());
		
		while (p.ok())
		{
			auto & sym = p.read_symbol();

			if (sym == "name") {				
				p.read_string();							
			}
			else if (sym == "box") {
				auto x = p.read_int16();
				auto y = p.read_int16();
				auto dx = p.read_int16();
				auto dy = p.read_int16();
				
				auto a = vdiv(v2f(x,y), v2f(tex_dim));
				auto b = vdiv(v2f(x + dx, y + dy), v2f(tex_dim));
		
				sprite.box = aabb2f(a,b);
				sprite.dim = v2s(dx,dy);
			}
			else if (sym == "delta") {
				auto x = p.read_int16();
				auto y = p.read_int16();
				sprite.delta = v2s(x,y);
			}
			else if (sym == "bearing") {
				auto x = p.read_int16();
				auto y = p.read_int16();
				sprite.delta = v2s(x,-y);
			}			
			else if (sym == "adv") {
				sprite.adv = p.read_int16();				
			}
			else {
				p.unread();
				break;
			}
		}
	}


	void RCReader::read_font()
	{
		Font & font = fonts.at(p.read_uint16());
		
		while (not (p.eof() or p.error.has()))
		{
			auto & sym = p.read_symbol();

			if (sym == "name") {				
				p.read_string();							
			}			
			else if (sym == "height") {
				font.height = p.read_int16();
			}
			else if (sym == "width") {
				font.width = p.read_int16();
			}
			else if (sym == "ascender") {
				font.ascender = p.read_int16();
			}
			else if (sym == "descender") {
				font.descender = p.read_int16();
			}
			else if (sym == "pad") {
				font.pad[0] = p.read_int16();
				font.pad[1] = p.read_int16();
			}
			else if (sym == "base") {
				font.base = base = p.read_uint16();				
			}			
			else {
				p.unread();
				break;
			}
		}
	}




	void RCReader::read_rc()
	{		
		while (p.ok())
		{		
			auto & sym = p.read_symbol();

			if (sym == "font") {
				read_font();
			}
			else if (sym == "sprite") {
				read_sprite();				
			}			
			else {
				p.set_error("unknown command: '%||", sym);
			}						
		}
		p.end();		
	} 
	
}
