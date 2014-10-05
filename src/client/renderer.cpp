#include "renderer.h"

//#include <functional>
#include <boost/range/adaptor/reversed.hpp>

#include "lodepng/lodepng.h"
#include "ai-env-helpers.h"
#include "view_base.h"
#include "conf.h"

namespace col {

	using b2i = Box;
	
	using Event = Console::Event;
	using Button = Console::Button;
	using Key = Console::Key;
	using Dev = Console::Dev;
	
	
	
	
	
	
	
	

	Layout const ly(conf.screen_w, conf.screen_h);


	using Res = map<pair<string,uint>, sf::Texture>;
	Res g_res;

	using ResFont = map<string, sf::Font>;
	ResFont g_fonts;

	using ResPixFont = map<string, PixFont>;
	ResPixFont g_pixfonts;
	

	ResFont::mapped_type const& res_font(string const& name, uint8 size) {
		auto p = g_fonts.find(name);
		if (p != g_fonts.end()) {
			return (*p).second;
		}
		else {
			auto &font = g_fonts[name];
			Path path = conf.font_path / name;
			if (!font.loadFromFile(path.c_str())) {
				throw std::runtime_error("cannot load font");
			}
			// set size
			return font;
		}
	}

	v2i get_dim(sf::Texture const& t) {
		auto const& s = t.getSize();
		return v2i(s.x, s.y); 
	}

	ResPixFont::mapped_type const& res_pixfont(string const& name) {
		auto p = g_pixfonts.find(name);
		if (p != g_pixfonts.end()) {
			return (*p).second;
		}
		else {
			auto &font = g_pixfonts[name];
			auto path = conf.font_path / name;
			font.load(path.c_str());
			return font;
		}
	}

	v2i get_text_dim(PixFont const& font, string const& text);


	int get_icon_id(Unit const& u) {
		return u.get_icon();
	}
	
	struct Text;
	
	v2i render_text(
		sf::RenderWindow &win,
		v2i const& pos,
		Text const& text		
	);

	inline
	bool is_water(Terr const& t) {
		return t.is_water_tile();
	}

	v2f vmul(v2f const& a, v2f const& b) {
		return v2f(a[0]*b[0], a[1]*b[1]);
	}
	
	v2f vmul(v2i const& a, v2f const& b) {
		return v2f(float(a[0])*b[0], float(a[1])*b[1]);
	}
	
	v2i vmul(v2i const& a, v2i const& b) {
		return v2i(a[0]*b[0], a[1]*b[1]);
	}
	
	v2i vmul(Coords const& a, v2i const& b) {
		return v2i(int(a[0])*b[0], int(a[1])*b[1]);
	}

	
	
	
	// categories, *xxx* -- generated to memory image
	string const
		TERR = "TERRAIN_SS",
		ICON = "ICONS_SS",
		PHYS = "PHYS0_SS",
		ARCH = "PARCH_SS",
		COLONY = "COLONY_PIK",
		BUILD = "BUILDING_SS",
		WOODBG = "WOODTILE_SS",
		DIFFUSE = "*DIFFUSE*",
		COAST = "*COAST*";

	
	// Load png file to sfml texture
	unsigned load_png(sf::Texture & tex, Path const& path) {

		std::vector<uint8> pix; 
		unsigned width, height;
		
		unsigned error = lodepng::decode(pix, width, height, path.c_str());

		if (error) {
			return error;			
		}
		
		sf::Image img;
		img.create(width, height, &pix[0]);
		
		tex.loadFromImage(img);
		return 0;
	}
	
	// Load resource file
	// cat -- category (dir name)
	// i -- frame num (file name)
	void load_res(Res::mapped_type & tex, string const& cat, uint const& i) {
		
		auto path = conf.res_path / cat / Path(format("%|03|.png", i));

		if (auto err = load_png(tex, path)) {
			print("WARNING: cannot load image file; path=%||; err=%||;\n", 
				path.c_str(), lodepng_error_text(err)
			);

			// try default
			auto def_path = conf.res_path / ICON / "065.png";
			if (auto err = load_png(tex, def_path)) {
				throw Critical("cannot load fallback image file; path=%||; err=%||;\n", 
					def_path.c_str(),
					lodepng_error_text(err)
				);
			}
		}

		tex.setSmooth(false);
	}
	
	// Get resource from global cache (load on demand)
	Res::mapped_type const& res(string const& cat, uint const& i = 1) {
		auto key = Res::key_type(cat, i);
		auto p = g_res.find(key);
		if (p != g_res.end()) {
			return (*p).second;
		}
		else {
			auto &img = g_res[key];
			load_res(img, cat, i);
			return img;
		}
	}


	
	
	sf::Color const ColorSelectedBG = sf::Color(0x3c,0x20,0x18,0xff);
	sf::Color const ColorFont = sf::Color(0x55,0x96,0x34,0xff);
	sf::Color const ColorNone = sf::Color(0,0,0,0);
	sf::Color const ColorBlack = sf::Color(0,0,0,255);
	sf::Color const ColorGreen = sf::Color(0,255,0,255);
	sf::Color const ColorBlue = sf::Color(0,0,255,255);
	sf::Color const ColorRed = sf::Color(255,0,0,255);
	sf::Color const ColorDarkGreen = sf::Color(0,127,0,255);
	sf::Color const ColorDarkBlue = sf::Color(0,0,127,255);
	sf::Color const ColorDarkRed = sf::Color(127,0,0,255);
	sf::Color const ColorWhite = sf::Color(255,255,255,255);
	sf::Color const ColorGray = sf::Color(128,128,128,128);
	
	sf::Color const ColorDarkSkyBlue = sf::Color(76,100,172,255);
	sf::Color const ColorSkyBlue = sf::Color(104,136,192,255);
	sf::Color const ColorLightSkyBlue = sf::Color(156,184,220,255);

	sf::Color const ColorDarkWoodBrown = sf::Color(84,68,44,255);
	sf::Color const ColorWoodBrown = sf::Color(132,112,80,255);
	sf::Color const ColorLightWoodBrown = sf::Color(152,128,92,255);







	
	
	v2f vmul(v2f const& a, v2f const& b);
	v2f vmul(v2i const& a, v2f const& b);

	
	v2i calc_align(Box const& par, v2i const& dim, v2f const& align = v2f(0.5, 0.5));
	v2i calc_align(v2i const& p_pos, v2i const& p_dim, v2i const& dim, v2f const& align = v2f(0.5, 0.5));
		

		
	void preload(Path const& d, uint const& i, Res::mapped_type const& tex) {
		auto key = Res::key_type(d.string(),i);
		g_res[key] = tex;
	}

	Res::mapped_type replace_black(Res::mapped_type const& inn_, Res::mapped_type const& tex_, v2i const& t) {
		/* Create new texture

		mask colors meaning:
		 transparent -> transparent
		 black -> take from tex
		 else -> take from mask

		 t -- tex offset vector

		*/

		auto out = inn_.copyToImage();
		auto inn = inn_.copyToImage();
		auto tex = tex_.copyToImage();

		for (uint j = 0; j < out.getSize().y; ++j) {
			for (uint i = 0; i < out.getSize().x; ++i) {
				if (inn.getPixel(i,j) == sf::Color(0,0,0,255)) {
					out.setPixel(i,j, tex.getPixel(t[0] + i, t[1] + j));
				}
			}
		}

		auto ret = sf::Texture();
		ret.loadFromImage(out);
		ret.setSmooth(false);
		return ret;
	}

	//void render_fields(sf::RenderWindow &win, Env const& env, Console const& con,
	//	v2i const& pix, Coords const& pos);


	v2i get_loffset(int l) {
		switch(l){
			case 0: return v2i(0,0);
			case 1: return v2i(8,0);
			case 2: return v2i(8,8);
			case 3: return v2i(0,8);
		}
		throw Critical("invalid 'l'");
	}

	void preload_coast(Biome bio, int start) {
		for (int k = 0; k < 8; ++k) {
			for (int l = 0; l < 4; ++l) {
				int ind = (k<<2) + l;
				preload(COAST, start + ind,
					replace_black(
						res(PHYS, 109+ind), res(TERR, bio), get_loffset(l)
					)
				);
			}
		}
	}

	int const TextureOceanId = 11;
	int const TextureSeaLaneId = 11;
	
	void preload_coast() {
		// 109 - 140
		
		preload_coast(TextureOceanId, 0);
		preload_coast(TextureSeaLaneId, 50);
	}

	void preload_terrain() {
		for (int i=1; i<13; ++i) {
			preload(DIFFUSE,  0+i, replace_black(res(PHYS, 105), res(TERR, i), {0,0}));
			preload(DIFFUSE, 50+i, replace_black(res(PHYS, 106), res(TERR, i), {0,0}));
			preload(DIFFUSE, 100+i, replace_black(res(PHYS, 107), res(TERR, i), {0,0}));
			preload(DIFFUSE, 150+i, replace_black(res(PHYS, 108), res(TERR, i), {0,0}));
		}

		preload_coast();
	}

	
	v2i render_text_line(
		sf::RenderWindow &win,

		v2i const& pos,
		v2i const& dim,
		v2f const& align,

		PixFont const& font,
		sf::Color const& fgcol,
		sf::Color const& bgcol,

		string const& text
	) ;
	

	sf::RectangleShape RectShape(
			v2i const& pos,
			v2i const& dim,
			sf::Color const& fill_color,
			int16 const& outline_thickness = 0,
			sf::Color const& outline_color = {0,0,0,255}
	) {
		sf::RectangleShape rectangle;
		rectangle.setPosition(pos[0], pos[1]);
		rectangle.setSize(sf::Vector2f(dim[0], dim[1]));
		rectangle.setFillColor(fill_color);
		rectangle.setOutlineColor(outline_color);
		rectangle.setOutlineThickness(outline_thickness);
		return rectangle;
	}

	
	struct Text{
		string text{""};
		string font{"tiny.png"};
		sf::Color fg{ColorWhite}, bg{ColorNone};
		
		string const& get_font() const { return font; }
		Text & set_font(string const& font) { this->font = font; return *this; }
		
		sf::Color const& get_fg() const { return fg; }
		Text & set_fg(sf::Color const& fg) { this->fg = fg; return *this; }
		
		sf::Color const& get_bg() const { return bg; }
		Text & set_bg(sf::Color const& bg) { this->bg = bg; return *this; }

		string const& get_text() const { return text; }
		Text & set_text(string const& text) { this->text = text; return *this; }
		
		v2i get_dim() const { 
			return get_text_dim(res_pixfont(font), text);		
		}
		
		Text(string const& text): text(text) {}
	};

	void render_terr(sf::RenderWindow &win,
			v2i const& pos,
			Env const& env,
			Terr const& terr);
	
	void render_shield(sf::RenderWindow &win, int16 x, int16 y, Color const& col, char letter) {
		
		auto t = Text(string() + letter).set_font("tiny.png").set_fg(ColorBlack);

		win.draw(
			RectShape(
				v2i(x,y) + v2i(1,1),
				{5, 7},
				{col.r,col.g,col.b, 255},
				1,
				{0,0,0, 255}
			)
		);
		
		render_text(
			win,
			v2i(x,y) + v2i(1,1),
			t
		);
	}

	void render_unit(sf::RenderWindow &win,
			Console & con,
			Coords const& pos,
			Env const& env,
			Terr const& terr,
			Vector2<int> const& map_pix,
			Coords const& delta);


	void render_pixel(sf::RenderWindow &win, v2i pix, const Color &col) {
		win.draw(
			RectShape(
				pix,
				v2i(1, 1),
				sf::Color(col.r,col.g,col.b, 255),
				0,
				sf::Color(0,0,0, 0)
			)
		);
	}
	

	
	void render_fill(sf::RenderWindow & win,
			v2i const& pos, 
			v2i const& dim,
			sf::Color const& col
	) {
		sf::RectangleShape r;
		r.setSize(sf::Vector2f(dim[0], dim[1]));
		r.setFillColor(col);
		//rectangle.setOutlineColor(sf::Color(0,50,0, 255));
		//rectangle.setOutlineThickness(1);
		r.setPosition(pos[0], pos[1]);
		win.draw(r);
	}
	
	void render_rect(sf::RenderWindow &win,
			v2i const& pos, v2i const& dim,
			sf::Color const& col)
	{
		sf::RectangleShape r;
		r.setSize(sf::Vector2f(dim[0], dim[1]));
		r.setFillColor(col);
		//rectangle.setOutlineColor(sf::Color(0,50,0, 255));
		//rectangle.setOutlineThickness(1);
		r.setPosition(pos[0], pos[1]);
		win.draw(r);
	}

	void render_rect(
			sf::RenderWindow &win,
			b2i const& box,
			sf::Color const& col
	) {
		render_rect(win, box.pos, box.dim, col);
	}
	
	void render_outline(sf::RenderWindow &win, v2i const& pos, v2i const& dim, sf::Color const& c) {
		/* Draw 1px thick outline border of pos,dim box 
		*/
		win.draw(
			RectShape(
				pos,
				dim,
				sf::Color(0,0,0, 0),
				1,
				sf::Color(c.r, c.g, c.b, c.a)
			)
		);
	}

	void render_inline(sf::RenderWindow &win, v2i const& pos, v2i const& dim, sf::Color const& c) {
		/* Draw 1px thick inside border of pos,dim box 
		*/
		render_outline(win, pos + v2i(1,1),	dim - v2i(2,2), c);		
	}	
	
	void render_sprite(sf::RenderWindow &win, int x, int y, const sf::Texture &img) {
		sf::Sprite s;
		s.setPosition(x, y);
		s.setTexture(img);
		win.draw(s);
	}

	void render_sprite(sf::RenderWindow & win, v2i const& pix, sf::Texture const& img) {
		sf::Sprite s;
		s.setPosition(pix[0], pix[1]);
		s.setTexture(img);
		win.draw(s);
	}

	
	void render_terr(sf::RenderWindow &win,
		Coords const& pos,
		Env const& env,
		Terr const& terr,
		Vector2<int> const& map_pix,
		Coords const& delta
	);

	
	void render_area(
		sf::RenderWindow & win,
		v2i const& area_pos,
		v2i const& area_dim,
		sf::Texture const& tex		
	);


	void render_sprite(sf::RenderWindow & win,
		v2i const& pix, v2i const& dim,
		sf::Texture const& img
	) {
		v2i tex_dim = get_dim(img);
				
		auto free = dim - tex_dim;
		v2i voffset(free[0]/2, free[1]/2);

		render_sprite(win, pix + voffset, img);
	}
	
	void render_sprite2(sf::RenderWindow & win,
		v2i const& pix, v2i const& dim,
		sf::Texture const& img
	) {
		render_sprite(win, pix, img);
	}

	using Texture = Res::mapped_type;
	
	struct Renderer{
		sf::RenderWindow &win;
		Env const& env;

		Renderer(sf::RenderWindow &win, Env const& env): win(win), env(env) {}

		void operator()(Res::mapped_type const& res, v2i const& pos);
		void operator()(Res::mapped_type const& res, v2i const& pos, v2i const& dim);
		void operator()(Unit const& unit, v2i const& pos);
		void operator()(Unit const& unit, v2i const& pos, v2i const& dim);
		void operator()(Item const& item, v2i const& pos);
		
		void fill(Texture const& tex, v2i const& pos, v2i const& dim);
		void fill(Color const& color, v2i const& pos, v2i const& dim);
		
	};

	void Renderer::operator()(Res::mapped_type const& res, v2i const& pos) {
		render_sprite(win, pos, res);
	}

	void Renderer::operator()(Res::mapped_type const& res, v2i const& pos, v2i const& dim) {
		render_sprite(win, pos, dim, res);
	}

	void Renderer::operator()(Unit const& unit, v2i const& pos, v2i const& dim) {
		render_sprite(win, pos, dim, res(ICON, unit.get_type_id()));
	}

	void Renderer::operator()(Unit const& unit, v2i const& pos) {
		render_sprite(win, pos, res(ICON, unit.get_type_id()));
	}

	void Renderer::operator()(Item const& item, v2i const& pos) {
		render_sprite(win, pos, res(ICON, item));
	}


	
	void render_area(sf::RenderWindow & win, v2i const& pos, v2i const& dim, sf::Color const& color) {
		win.draw(
			RectShape(
				pos,
				dim,
				sf::Color(color.r, color.g, color.b, color.a),
				0,
				{0,0,0,0}
			)
		);
	}
		
	v2i render_text2(
		/*sf::RenderWindow &win,

		v2i const& pos,
		v2i const& dim,
		v2i const& align,

		PixFont const& font,
		sf::Color const& fgcol,
		sf::Color const& bgcol,

		string const& text*/

		sf::RenderWindow &win,
		Vector2<int> const& pos,
		PixFont const& font,
		string const& text,
		sf::Color const& bgcol,
		int width	
	);

	
	void render_area(
		sf::RenderWindow & win,
		v2i const& area_pos,
		v2i const& area_dim,
		sf::Texture const& tex
	) {
		using sf::Sprite;
		using sf::IntRect;
		using sf::Vector2f;

		auto tile_dim = Vector2<int>(tex.getSize().x, tex.getSize().y);
		auto area_end = area_pos + area_dim;

		int ei,ej;

		// center
		{
			auto s = Sprite(tex, IntRect(0, 0, tile_dim[0], tile_dim[1]));
			int i,j;
			j = area_pos[1];
			while (j < area_end[1] - tile_dim[1]) {
				i = area_pos[0];
				while (i < area_end[0] - tile_dim[0]) {
					s.setPosition(Vector2f(i, j));
					win.draw(s);
					//cerr << i << endl;
					i += tile_dim[0];
				}
				j += tile_dim[1];
			}

			ei = i;
			ej = j;
		}

		
		// bottom
		{
			auto s = Sprite(tex, IntRect(0, 0, tile_dim[0], area_end[1] - ej));
			int i = area_pos[0], j = ej;
			while (i < area_end[0] - tile_dim[0]) {
				s.setPosition(Vector2f(i, j));
				win.draw(s);
				i += tile_dim[0];
			}
			ei = i;
		}

		// right
		{
			auto s = Sprite(tex, IntRect(0, 0, area_end[0] - ei, tile_dim[1]));
			int i = ei, j = area_pos[1];
			while (j < area_end[1] - tile_dim[1]) {
				s.setPosition(Vector2f(i, j));
				win.draw(s);
				j += tile_dim[1];
			}
			ej = j;
		}

		// corner
		{
			auto s = Sprite(tex, sf::IntRect(0, 0, area_end[0] - ei, area_end[1] - ej));
			int i = ei, j = ej;
			s.setPosition(Vector2f(i, j));
			win.draw(s);
		}



		// alt but problematic
		//sf::RectangleShape rectangle;
		//rectangle.setSize(sf::Vector2f(dim[0], dim[1]));
		//rectangle.setTexture(&res("COLONIZE/WOODTILE_SS", 1));
		//rectangle.setTextureRect(sf::IntRect(pos[0], pos[1], dim[0], dim[1]));
		//win.draw(rectangle);


	}

	//sf::Color Black = {0,0,0,255};
	//sf::Color Transparent = {0,0,0,0};
	
		
	/*
	static Res::mapped_type const* g_cursor_tex = nullptr;
	void set_cursor(Res::mapped_type const* tex = nullptr) {
		
		g_cursor_tex = tex;
	}*/
	
	
	
	template<typename K>
	void render_select(sf::RenderWindow & win, Console & con,
		std::function<v2i(v2i const& dim)> cpos,
		//Box const& par, v2f const& align, 
		vector<pair<K, string>> const& kvs,
		K & selected,
		std::function<void()> onselect,
		std::function<void()> oncancel
	) {
		
		int const LINE_HEIGHT = 10;
		
		v2i const dim = v2i(ly.scr.dim[0] * 0.3, (kvs.size()+1) * LINE_HEIGHT);
		v2i pos = cpos(dim);
		
		// click anywhere -- cancel
		con.on(Event::Press, oncancel);
		
		// background
		render_area(win, pos, dim, res("WOODTILE_SS", 1));
		
		// click on dialog -- do nothing 
		con.on(Event::Press, pos, dim, [](){});
				
		// entries
		for (size_t i = 0; i < kvs.size(); ++i) {
			
			auto p = pos + v2i(0, i * LINE_HEIGHT);
			auto d = v2i(dim[0], LINE_HEIGHT);
			
			auto& key = kvs[i].first;
			
			// selected bg
			if (key == selected) {
				render_rect(win, 
					p, d,
					ColorSelectedBG
				);
			}

			// label
			render_text_line(
				win,
				p, d, v2f(0.5, 0.5),
				res_pixfont("tiny.png"), ColorFont, ColorNone,
				kvs[i].second
			);
			
			// left click on entry -- select
			con.on(Event::Press, Button::Left, p, d, 
				[&selected,key,onselect](){ selected = key; onselect(); }
			);
		}
			
	}
	
	
	void render_unit_cargo(sf::RenderWindow &win, Console & con, v2i const& pos, v2i const& dim, Unit const& unit) {
		// render unit cargo
		
		/*
		float b = 10;
		float a = float(ly.city_unit_resources.dim[0] - b)/float(600);

		auto calc_width = [a,b](float x) {
			return a * x + b;
		};*/
		
		
		
		// render unit items		
		auto pix = pos;		
		
		for (auto item: COLONY_ITEMS) {
			int num = unit.get(item);
			
			if (num) {
				auto clones = (unit.used_space(num) / 10);
				auto width = 10 + clones * 2;
				
				auto tile_pos = pix;
				auto tile_dim = v2i(width, dim[1]);
				render_inline(win, tile_pos, tile_dim, ColorWoodBrown);
				
				// left click on cargo -- start drag cargo unload
				con.on(Event::Press, Button::Left, tile_pos, tile_dim, 
					[item,num,&con](){ con.drag_cargo(item, num, -1); }
				);
				
				auto & item_tex = res(ICON, get_item_icon_id(item));
				
				auto item_pos = calc_align(pix, v2i(12, dim[1]), get_dim(item_tex));
								
				// render icons 
				// clones
				for (uint16 i = 0; i < clones; ++i) {
					render_sprite(win,
						item_pos + v2i(i * 2, 0),
						item_tex						
					);
					
				}
				
				pix[0] += width;
			}
		}
		
		// reminder
		{
			int num = unit.get_space_left();
			if (num) {
				auto tile_pos = pix;
				auto tile_dim = v2i(int16((num+19)/20)*4, dim[1]);
				render_inline(win, tile_pos, tile_dim, ColorWoodBrown);
				
				pix[0] += tile_dim[0];
			}
		}
		
		
		
		
		// unit cargo area frame
		auto store_pos = pos;
		auto store_dim = v2i(pix[0] - pos[0], dim[1]);
		//render_inline(win, store_pos, store_dim, ColorDarkWoodBrown);

		if (con.drag_item) {
			// left release -- drop cargo
			con.on(Event::Release, Button::Left, store_pos, store_dim,
				[&con](){ con.drop_cargo(+1); }
			);
		}
		
	}
	
	
	
	void render_city_store(sf::RenderWindow & win, Console & con, v2i const& pos, v2i const& dim, Terr const& terr) {
		auto const& env = con.env; // get_render_env
		
		// render storage area
		render_area(win, pos, dim, {76,100,172,255});

		if (con.drag_item) {
			// left release city store -- unload cargo
			con.on(Event::Release, Button::Left, pos, dim,
				[&con](){ con.drop_cargo(-1); }
			);
		}
		
		int tile_width = 16;
		auto & col_st = env.get_store(terr);
		
		auto pix = pos;
		for (auto item: COLONY_ITEMS) {

			// num of items
			int num = col_st.get(item);
		
			auto tile_pos = pix;
			auto tile_dim = v2i(tile_width,12);
			
			// render item
			render_sprite(win, tile_pos, tile_dim, res(ICON, get_item_icon_id(item)));
			
			// left click on item -- start drag cargo load
			con.on(Event::Press, Button::Left, tile_pos, tile_dim, 
				[item,num,&con](){ con.drag_cargo(item, num, +1); }
			);

			// render number
			auto txt = Text(std::to_string(num)).set_font("tiny.png");
			render_text(
				win,
				calc_align(pix + v2i(0,13), v2i(tile_width,0), txt.get_dim(), v2f(0.5, 0)),
				txt				
			);
			
			pix[0] += tile_width;
		}
	}
	

	void render_city(sf::RenderWindow &win, Env const& env, Console & con) {
		/*
			// panel right
			render_panel(app, env, con,
				ly.pan.pos,
				ly.pan.dim
			);
			// vline left of the panel
			render_rect(app,
				{ly.scr.dim[0] - ly.pan.dim[0] - 1, ly.bar.end[1] + 1},
				{1, h-8},
				{0,0,0,255}
			);
		*/

		auto render = Renderer{win,env};
		
		auto T1 = v2i(23,27);
		auto T2 = v2i(44,22);
		auto T3 = v2i(53,37);
		auto T4 = v2i(75,48);
			
		
		// render area
		auto sand_tex = res(ARCH, 1);
		render_area(win, ly.city_builds.pos, ly.city_builds.dim, sand_tex);
		
		// relative positions
		vector<v2i> pixs({
			v2i(6,6), v2i(56,5), v2i(87,3), v2i(145,7), v2i(173,10),
			v2i(8,33), v2i(37,37), v2i(67,45), v2i(96,45), v2i(128,45),
			v2i(10,68), v2i(15,94), v2i(66,79), v2i(123,47), v2i(123,98)
		});
		
		vector<v2i> dims({
			T2,T1,T3,T1,T1,
			T1,T1,T1,T1,T2,
			T2,T2,T3,T4,T2			
		});


		auto& terr = *con.get_sel_terr();
		auto& col = terr.get_colony();

		// render buildings
		{
			// hover anywhere -- clear label on building
			con.on(Event::Hover,
				[&con]() { con.sel_colony_slot_id = -1; }				
			);
			
			// render in reverse order to make sure that docks clickarea will not obscure other builds
			int i = col.builds.size();
			while (i > 0) {
				i = i - 1;
				
				auto workplace_id = i;
				
				auto& b = col.builds.at(i);
				auto& build_tex = res(BUILD, b.get_type_id());
				auto build_pos = ly.city_builds.pos + pixs.at(i);
				auto build_dim = get_dim(build_tex);
				
				render_sprite(win, build_pos, build_tex);
				
				// render build name
				if (con.sel_colony_slot_id == i) {
											
					render_text_line(
						win,
		
						build_pos,
						build_dim,
						{0.5, 0.5},
		
						res_pixfont("tiny.png"),
						{255,255,255,255},
						{0,0,0,255},
							
						b.get_name()
					);
						
				}
				else 
				if (b.under_construction()) 
				{
					// render_sprite(win, build_pos, res(ICON, get_item_icon_id(ItemHammers)));					
					render_text_line(
						win,
		
						build_pos,
						build_dim,
						{0.5,0.5},
		
						res_pixfont("tiny.png"),
						{255,255,255,255},
						{0,0,0,255},
							
						std::to_string(b.get_hammers()) + '/' + std::to_string(b.get_cost_hammers())						
					);				
				}
				
				// left click on building -- assign worker or show construction popup
				con.on(Event::Press, Button::Left, build_pos, build_dim,
					[&con, workplace_id]() { 
						if (con.get_sel_unit()) {
							con.command(format("work-build %||", workplace_id));
						}
						else {
							con.select_build = 1;  // todo slot_id blablabla
							con.sel_slot_num = workplace_id;
						}
					}	
				);
				
				// number of produced items
				if (int y = misc::get_yield(env, b)) {
					auto& item_tex = res(ICON, get_item_icon_id(b.get_proditem()));
					auto item_dim = get_dim(item_tex);
					
					// item icon
					render_sprite(win, build_pos, item_tex);
								
					// number
					render_text_line(
						win,
		
						build_pos + v2i(item_dim[0], 0),
						v2i(0, item_dim[1]),
						{0, 0.5},
		
						res_pixfont("tiny.png"),
						{255,255,255,255},
						{0,0,0,255},
							
						std::to_string(y)
					);
				}
				
				
				
				
				// hover on building -- show label with building name (select buildplace)
				con.on(Event::Hover, build_pos, build_dim,
					[&con, i]() { con.sel_colony_slot_id = i; }
				);
				
				v2i units_frame = {25, 16};
				
				// units on build
				int n = b.units.size();
				int i = 0;
				for (auto& unit_ptr: b.units) {
					auto& unit = *unit_ptr;
					auto& unit_tex = res(ICON, get_icon_id(unit));

					v2i unit_pos = calc_align(
						Box(
							build_pos + build_dim - units_frame + v2i(1,2), units_frame
						), 
						get_dim(unit_tex),
						v2f(float(i+1)/float(n+1), 1)
					);
					
					v2i unit_dim = get_dim(unit_tex);

					v2i sel_pos = unit_pos;
					v2i sel_dim = unit_dim;
					
					// render unit on build
					render_sprite(win, unit_pos, unit_tex);

					auto unit_id = unit.id;

					if (unit_id == con.get_sel_unit_id()) {
						// render selection frame
						render_inline(win, sel_pos, sel_dim, {255,100,100,255});
					}
					else {
						// left click on unit -- select this unit
						con.on(Event::Press, Button::Left, sel_pos, sel_dim,
							[&con,unit_id]() { 
								con.command("sel " + to_string(unit_id)); 					
							}
						);
					}
					
					++i;
				}
				
				
			}
		}

		// render fields
		{
			
			/// render background
			// render_area(win, pos, dim, res(conf.res_path + "WOODTILE_SS", 1));
			
			
			auto pix = ly.city_fields.pos;

			auto& tex_wood = res(WOODBG, 1);
			render_area(win, ly.city_fields.pos, ly.city_fields.dim, tex_wood);
			
			auto city_terr_pos = calc_align(ly.city_fields.pos, ly.city_fields.dim, ly.terr_dim);
			
			auto& city_terr = terr;
			
			auto pos = env.get_coords(terr);
			auto city_coords = Coords(pos[0]-1, pos[1]-1);
			
			
			for(size_t field_id = 0; field_id < col.fields.size(); ++field_id) {
				auto& field = col.fields.at(field_id);
				
				auto& field_terr = field.get_terr();
				
				v2i relc = (env.get_coords(*field.terr) - env.get_coords(city_terr)).cast<int>();
				auto field_pos = city_terr_pos + vmul(relc, ly.terr_dim);
				auto field_dim = ly.terr_dim;
				
				
				render_terr(win, field_pos, env, field_terr);
															
				
				if (field.units.size() == 0) {
					// left click on field -- assign unit to work there
					string cmd = format("work-field %||", field_id);
					con.on(Event::Press, Button::Left, field_pos, field_dim,
						[&con,cmd](){ con.command(cmd); }
					);
				}

				// units on field
				for (auto& unit_p: field.units) {
					auto& unit = *unit_p;
					if (unit.in_game) {
						v2i unit_dim = v2i(field_dim[0]/2, field_dim[1]);
						v2i unit_pos = field_pos + v2i(field_dim[0]/2, 0);

						v2i item_dim = field_dim; //v2i(field_dim[0]/2, field_dim[1]);
						v2i item_pos = field_pos;					

						v2i sel_pos = field_pos;
						v2i sel_dim = field_dim;

						auto& unit_tex = res(ICON, get_icon_id(unit));

						// field production
						if (auto proditem = field.get_proditem()) {
							// render produced item
							auto& item_tex = res(ICON, get_item_icon_id(proditem));
							render_sprite(win, 
								calc_align(Box(item_pos, item_dim), get_dim(item_tex)),
								item_tex
							);

							// render produced item amount
							auto text = Text(to_string(env.get_prodnum(field)));
							text.set_font("tiny.png").set_fg(ColorWhite).set_bg(ColorBlack);					
							render_text(win, item_pos, text);
						}

						// render unit on field
						render_sprite(win, 
							calc_align(Box(unit_pos, unit_dim), get_dim(unit_tex)),
							unit_tex
						);

						auto unit_id = unit.id;

						if (unit_id == con.get_sel_unit_id()) {
							// render selection frame
							render_inline(win, sel_pos, sel_dim, {255,100,100,255});

							// left click on field with selected worker -- switch to next proditem
							con.on(Event::Press, Button::Left, sel_pos, sel_dim,
								[&con,field_id]() { 
									con.command("prodnext-field " + to_string(field_id));
								}
							);
						}
						else {
							// left click on field with worker -- select this worker
							con.on(Event::Press, Button::Left, sel_pos, sel_dim,
								[&con,unit_id]() { 
									con.command("sel " + to_string(unit_id)); 					
								}
							);
						}
					}
				}
				
				
			}
			
			
		}

		
		
		// render middle background
		render_fill(win, ly.city_middle_bg.pos, ly.city_middle_bg.dim, ColorSkyBlue);
		
		
		
		

		// render units
		array<uint8,16> num_workers;
		array<int,16> prod;
		num_workers.fill(0);
		prod.fill(0);

		/*
		for (auto& p: terr.units) {
			auto& unit = *p;
			if (unit.workplace != nullptr) {
				if (unit.workplace->place_type() == PlaceType::Build) {
					
				}
			}
		}*/
		
		
		// right click anywhere - unselect selected unit
		if (con.get_sel_unit()) {			
			con.on(Event::Press, Button::Right,
				[&con]() { 
					con.unselect_unit();
				}
			);
		}

		render_inline(win,
			ly.city_units.pos, ly.city_units.dim,
			ColorDarkGreen
		);
		
		// left click on city terrain area with selected unit -- unwork that unit
		if (con.get_sel_unit()) {
			con.on2(Event::Press, Button::Left, ly.city_units.pos, ly.city_units.dim,
				"work-none"
			);
		}
		int i = 0;
		for (auto& p: terr.units) {
			auto& unit = *p;
			
			if (unit.in_game and !unit.is_working()) {

				auto& unit_tex = res(ICON, get_icon_id(unit));
				v2i unit_pos = ly.city_units.pos + v2i(ly.terr_dim[0] * i, 0);
				v2i unit_dim = ly.terr_dim;

				v2i sel_pos = unit_pos;
				v2i sel_dim = unit_dim;

				// render unit
				render_sprite(win, 
					calc_align(Box(unit_pos, unit_dim), get_dim(unit_tex)),
					unit_tex
				);

				auto unit_id = unit.id;

				if (unit_id == con.get_sel_unit_id()) {
					// render selection frame
					render_inline(win, sel_pos, sel_dim, {255,100,100,255});
					
					// left click on selected unit (on terr) -- show equip select
					con.on(Event::Press, Button::Left, sel_pos, sel_dim,
						[&con,unit_id]() { 
							con.equip_to_unit_id = unit_id;
						}
					);
		
					render_unit_cargo(win, con, ly.city_unit_cargo.pos, ly.city_unit_cargo.dim , unit);
					
				}
				else {
					// left click on unselected unit -- select unit
					con.on(Event::Press, Button::Left, sel_pos, sel_dim,
						[&con,&env,unit_id]() { 
							con.select_unit(unit_id);							
						}
					);
				}
				
				++i;
			}
			
			
		}
		
		
		render_city_store(win, con, ly.city_resources.pos, ly.city_resources.dim, terr);
		
		
		
		
		
		
		// render EXIT button
		render_area(win, ly.city_exit.pos, ly.city_exit.dim, {140,0,140,255});
		
		auto txt = Text("Ret").set_font("tiny.png");
		render_text(
			win,
			ly.city_exit.pos + v2i(1,1),
			txt			
		);

		// left click on exit button -- exit city screen
		con.on(Event::Press, Button::Left, ly.city_exit.pos, ly.city_exit.dim,
			[&con, &terr](){ con.command("exit"); }
		);

		// render fields
		//render_fields(win, env, con, ly.city_fields.pos, );

		if (con.select_build > 0) {
			
			vector<pair<BuildType::Id, string>> kvs;
			
			auto& cb = con.get_sel_terr()->get_colony().get_build(con.sel_slot_num);
			
			for (auto& item: *env.bts) {
				auto const& b = item.second;
				if (b.place_on_id == cb.get_type_id()) {				
					kvs.emplace_back(
						item.first,
						b.get_name()
					);
				}
			}
			
			render_select<BuildType::Id>(win, con,
				[](v2i const& dim) {
					return calc_align(Box(ly.scr), dim); 
				},
				kvs,
				con.select_build,
				[&con](){ 
					con.command(format("construct %|| %||", con.sel_slot_num, con.select_build));
					con.select_build = 0;
					con.sel_slot_num = -1;
				},  // onselect
				[&con](){ con.select_build = 0; }   // oncancel
			);
		}


		
		// pop-ups
		if (con.equip_to_unit_id) {
			vector<pair<int, string>> entries;
			for (auto& t: misc::equip_to_types(env, env.get<Unit>(con.equip_to_unit_id))) {
				entries.push_back(pair<int, string>{
					t->id, 
					format("%|| (%||) (%||)", t->get_name(), t->get_num1(), t->get_num2())
				});
			}

			// onclick -> equip/orders menu
			render_select<int>(win, con,
				[](v2i const& dim) {
					return calc_align(Box(ly.scr), dim); 
				},
				entries,
				con.equip_to_unit_type_id,
				[&con](){ 
					con.command(string("equip ") + to_string(con.equip_to_unit_type_id));
					con.equip_to_unit_id = 0;
					con.equip_to_unit_type_id = 0;
				},  // onselect
				[&con](){ 
					con.equip_to_unit_id = 0;
					con.equip_to_unit_type_id = 0;
				}   // oncancel
			);
		}
		
		
		


	}
	
	
	void render_units(sf::RenderWindow &win, Console & con,
		Vector2<int> const& pos, 
		Vector2<int> const& dim, 
		Env const& env, 
		Terr const& terr) 
	{
		int i = 0;
		int j = 0;
		v2i unit_dim = ly.terr_dim;
		
		for (auto& p: terr.units) {
			auto& unit = *p;
			if (unit.in_game and !unit.is_working()) {

				auto& unit_tex = res(ICON, get_icon_id(unit));
				v2i unit_pos = pos + vmul(unit_dim, v2i(i,j));
				
				v2i sel_pos = unit_pos;
				v2i sel_dim = unit_dim;

				// render unit
				render_sprite(win, 
					calc_align(Box(unit_pos, unit_dim), get_dim(unit_tex)),
					unit_tex
				);
				
				// select
				auto unit_id = unit.id;

				if (unit_id == con.get_sel_unit_id()) {
					// render selection frame
					render_inline(win, sel_pos, sel_dim, {255,100,100,255});					
				}
				else {
					// left click on unselected unit -- select unit
					con.on(Event::Press, Button::Left, sel_pos, sel_dim,
						[&con,unit_id]() { 
							con.command("sel " + to_string(unit_id)); 					
						}
					);
				}
				
				
				
				if ((i-1) * unit_dim[0] >= dim[0]) {
					i = 0;
					++j;
				}
				else {
					++i;
				}
			}
		}
	}

	Terr const& get_terr_ext(Env const& env, Coords const& p) {
		Coords q;
		auto w = env.w, h = env.h;
		auto x = p[0], y = p[1];
		if (x < 0) {
			if (y < 0) {
				q = Coords(x+1,y+1);
			}
			else if (h <= y) {
				q = Coords(x+1,y-1);
			}
			else {
				q = Coords(x+1,y);
			}
		}
		else if (w <= x) {
			if (y < 0) {
				q = Coords(x-1,y+1);
			}
			else if (h <= y) {
				q = Coords(x-1,y-1);
			}
			else {
				q = Coords(x-1,y);
			}
		}
		else {
			if (y < 0) {
				q = Coords(x,y+1);
			}
			else if (h <= y) {
				q = Coords(x,y-1);
			}
			else {
				q = Coords(x,y);
			}
		}
		return env.get_terr(q);
	}

	using LocalArea = array<Terr const*, 9>;

	array<Terr const*, 9> make_terr_ext(Env const& env, Coords const& p) {
		array<Terr const*, 9> arr;
		for (auto dir: ALL_DIRS) {
			arr[(dir)] = &get_terr_ext(env, p + vec4dir(dir));
		}
		return arr;
	}

	Terr const& get(array<Terr const*, 9> const& loc, Dir::t const& dir) {
		return *loc[(dir)];
	}

	int get_wxad_index(LocalArea const& loc, Phys const& phys) {
		int idx = 0;
		if ( get(loc, Dir::W).has_phys(phys) ) idx |= 8;
		if ( get(loc, Dir::X).has_phys(phys) ) idx |= 4;
		if ( get(loc, Dir::A).has_phys(phys) ) idx |= 2;
		if ( get(loc, Dir::D).has_phys(phys) ) idx |= 1;
		return idx;
	}

	int get_wxad_index_level(LocalArea const& loc, Alt const& lvl) {
		int idx = 0;
		if ( get(loc, Dir::W).get_alt() == lvl ) idx |= 8;
		if ( get(loc, Dir::X).get_alt() == lvl ) idx |= 4;
		if ( get(loc, Dir::A).get_alt() == lvl ) idx |= 2;
		if ( get(loc, Dir::D).get_alt() == lvl ) idx |= 1;
		return idx;
	}

	
	array<Dir::t,8> CLOCKWISE_DIRS = {
		Dir::W, Dir::E, Dir::D, Dir::C, Dir::X, Dir::Z, Dir::A, Dir::Q
	};

	
	int get_coast_index(uint8 const& l, Terr const& t0, Terr const& t1, Terr const& t2) {
		uint8 k =
			(!is_water(t2) << 2) +
			(!is_water(t1) << 1) +
			(!is_water(t0) << 0);
		return (k<<2) + l;
	}

	/*
	Biome get_render_biome(LocalArea const& loc) {
		auto b = get(loc, Dir::S).biome;

		if (is_water_biome(b)) {
			auto c = get(loc, Dir::A);
			if (!is_water_biome(c)) return c;

			c = get(loc, Dir::X);
			if (!is_water_biome(c)) return c;

			c = get(loc, Dir::D);
			if (!is_water_biome(c)) return c;

			c = get(loc, Dir::W);
			if (!is_water_biome(c)) return c;
		}
		return b;
	}*/

	void render_diffuse_from(sf::RenderWindow &win,
		v2i const& pix,
		LocalArea const& loc,
		Dir::type d,
		int offset
	) {
		auto b = get(loc, d).biome;
		if (b != BiomeNone) {
			render_sprite(win, pix, res(DIFFUSE, get_biome_icon_id(b) + offset) );
		}
		else {
			// no diffuse from unexplored terrain (looks as bad as diffuse on water from terrain)
		}
	}


	
	//int const TextureMountainId;
	//int const TextureHillId;

	
	
	

	
	void render_terr(sf::RenderWindow &win,
			v2i const& pos,
			Env const& env,
			Terr const& terr)			
	{
		
		auto& pix = pos;
		
		auto coords = env.get_coords(terr);

		auto loc = make_terr_ext(env, coords);

		auto biome = terr.biome;
		auto forest = terr.has_phys(PhysForest);
		
		auto hill = (terr.get_alt() == AltHill);
		auto mountain = (terr.get_alt() == AltMountain);

		
		// render biome
		if (biome == BiomeNone) {
			render_sprite(win, pix, res(PHYS, 149));			
			return;  // unknown terrain - early return !!!
		}
		
		render_sprite(win, pix, res(TERR, get_biome_icon_id(biome)));
		
		// render neighs pattern
		render_diffuse_from(win, pix, loc, Dir::W, 0);
		render_diffuse_from(win, pix, loc, Dir::D, 50);
		render_diffuse_from(win, pix, loc, Dir::X, 100);
		render_diffuse_from(win, pix, loc, Dir::A, 150);

		
		

		// coast
		// 01
	    // 32

		if (is_water(terr)) {
			int base;
			
			if (terr.has_phys(PhysSeaLane)) {
				base = 50;
			}
			else {
				base = 0;
			}
			
			uint8 const h = conf.tile_dim >> 1;

			render_sprite(win, pix + v2i(0,0), res(COAST,
				base + get_coast_index(0, get(loc,Dir::A), get(loc,Dir::Q), get(loc,Dir::W))
			));

			render_sprite(win, pix + v2i(h,0), res(COAST,
				base + get_coast_index(1, get(loc,Dir::W), get(loc,Dir::E), get(loc,Dir::D))
			));

			render_sprite(win, pix + v2i(h,h), res(COAST,
				base + get_coast_index(2, get(loc,Dir::D), get(loc,Dir::C), get(loc,Dir::X))
			));

			render_sprite(win, pix + v2i(0,h), res(COAST,
				base + get_coast_index(3, get(loc,Dir::X), get(loc,Dir::Z), get(loc,Dir::A))
			));

		}

		
		// level
		if (terr.get_alt() == AltMountain) {
			render_sprite(win, pix, res(PHYS, 33 + get_wxad_index_level(loc, terr.get_alt())));
		}

		if (terr.get_alt() == AltHill) {
			render_sprite(win, pix, res(PHYS, 49 + get_wxad_index_level(loc, terr.get_alt())));
		}

		// phys feats & improvments
		if (terr.has_phys(PhysPlow)) {
			render_sprite(win, pix, res(PHYS, 150));
		}

		if (terr.has_phys(PhysForest)) {
			render_sprite(win, pix, res(PHYS, 65 + get_wxad_index(loc, PhysForest)));
		}

		if (terr.has_phys(PhysMajorRiver)) {
			if (!is_water(terr)) {
				auto ind = get_wxad_index(loc, PhysMajorRiver|PhysMinorRiver);
				if (ind) {
					render_sprite(win, pix, res(PHYS, 1 + ind));
				}
				else {
					render_sprite(win, pix, res(PHYS, 16));
				}
			}
			else {
				for (int i = 0; i < 8; i += 2) {
					if ( get(loc, CLOCKWISE_DIRS[i]).has_phys(PhysMajorRiver) ) {
						render_sprite(win, pix, res(PHYS, 141 + (i >> 1)));
					}
				}
			}

		}
		else if (terr.has_phys(PhysMinorRiver)) {
			if (!is_water(terr)) {
				auto ind = get_wxad_index(loc, PhysMajorRiver|PhysMinorRiver);
				if (ind) {
					render_sprite(win, pix, res(PHYS, 17 + ind));
				}
				else {
					render_sprite(win, pix, res(PHYS, 32));
				}
			}
			else {
				for (int i = 0; i < 8; i += 2) {
					if ( get(loc, CLOCKWISE_DIRS[i]).has_phys(PhysMinorRiver) ) {
						render_sprite(win, pix, res(PHYS, 145 + (i >> 1)));
					}
				}
			}
		}

		if (terr.has_phys(PhysRoad)) {
			bool r = false;
			for (int i=0; i<8; ++i) {
				if ( get(loc, CLOCKWISE_DIRS[i]).has_phys(PhysRoad) ) {
					render_sprite(win, pix, res(PHYS, 82 + i));
					r = true;
				}
			}
			if (!r) {
				render_sprite(win, pix, res(PHYS, 81));
			}
		}

	}
	
	void render_selected_unit(sf::RenderWindow & win, Console & con, v2i const& pos, Unit & unit);

	
	
	void render_terr(sf::RenderWindow &win,
			Coords const& pos,
			Env const& env,
			Terr const& terr,
			Vector2<int> const& map_pix,
			Coords const& delta)
	{

		int x = (pos[0] - delta[0]) * conf.terr_w + map_pix[0];
		int y = (pos[1] - delta[1]) * conf.terr_h + map_pix[1];
		
		render_terr(win, v2i(x,y), env, terr);
		
	}

	using sf::RenderWindow;


	Color get_unit_color(Unit const& u) {
		auto c = u.get_nation().get_color();
		if (u.get_time_left() == 0) {
			return Color(c.r/2, c.g/2, c.b/2, c.a/2);
		}
		return Color(c.r, c.g, c.b, c.a);
	}
	
	void render_stack(sf::RenderWindow &win,
			Console & con,
			v2i const& pos,
			Env const& env,
			Terr const& terr)
	{
		auto coords = env.get_coords(terr);
		
		bool sel_unit_here = false;
		if (auto up = con.get_sel_unit()) {
			if (env.get_coords(*up) == coords) {
				sel_unit_here = true;
			}
		}
		
		// first render colony icon
		if (terr.has_colony()) {
			// render colony
			render_sprite(win, pos[0], pos[1], res(ICON, 4));
						
			// left click on colony (on map) -- enter colony screen
			con.on(Event::Press, Button::Left, pos, v2i(conf.tile_dim, conf.tile_dim),
				[&con, coords](){ 
					con.select_terr(coords);
					con.command("enter"); 
				}
			);
			
			// colony flag
			if (auto p = env.get_control(terr)) {
				// render owner flag over colony (unit in garrison)
				render_sprite(win, pos[0] + 5, pos[1], res(ICON, p->get_flag_id()));
			}
			
		}
		else if (terr.has_units() and !sel_unit_here) {
			// unit
			auto& unit = env.get_defender(terr);

			auto icon = res(ICON, get_icon_id(unit));
			// render unit in field
			render_shield(win, pos[0], pos[1], get_unit_color(unit), con.get_letter(unit));
			render_sprite(win, 
				calc_align(Box(pos, ly.terr_dim), get_dim(icon)), 
				icon
			);
			
			// left click on unit -- select unit
			auto unit_id = unit.id;
			con.on(Event::Press, Button::Left, pos, ly.terr_dim, 				
				[&con,unit_id](){
					con.select_unit(unit_id);
				}
			);
			
		}
		
		
		// now render selected unit
		if (sel_unit_here) {	

			render_selected_unit(win, con, pos, *con.get_sel_unit());

		}
		
	}


	void render_selected_unit(RenderWindow & win, Console & con, v2i const& pos, Unit & unit) {
		auto icon = res(ICON, unit.get_icon());
		auto unit_id = unit.id;
		
		// render blinking shield
		if (int(con.time * 10.0) % 10 >= 5) {
			render_shield(win, pos[0], pos[1], get_unit_color(unit), con.get_letter(unit));
		}

		// render unit in field
		render_sprite(win, 
			calc_align(Box(pos, ly.terr_dim), get_dim(icon)), 
			icon
		);

		// selected unit keyboard shortcuts (numpad -> move)
		con.on(Event::Press, Key::Numpad2,
			[&con](){ con.command("move 0 1"); }
		);
		con.on(Event::Press, Key::Numpad8,
			[&con](){ con.command("move 0 -1"); }
		);
		con.on(Event::Press, Key::Numpad6,
			[&con](){ con.command("move 1 0"); }
		);
		con.on(Event::Press, Key::Numpad4,
			[&con](){ con.command("move -1 0"); }
		);
		con.on(Event::Press, Key::Numpad3,
			[&con](){ con.command("move 1 1"); }
		);
		con.on(Event::Press, Key::Numpad7,
			[&con](){ con.command("move -1 -1"); }
		);
		con.on(Event::Press, Key::Numpad1,
			[&con](){ con.command("move -1 1"); }
		);
		con.on(Event::Press, Key::Numpad9,
			[&con](){ con.command("move 1 -1"); }
		);
		
		// build colony
		con.on(Event::Press, Key::B,
			[&con](){ con.command("build-colony Unnamed"); }
		);
		// build road
		con.on(Event::Press, Key::R,
			[&con,unit_id](){ con.order_unit(unit_id, 'R'); }
		);
		// plow fields
		con.on(Event::Press, Key::P,
			[&con,unit_id](){ con.order_unit(unit_id, 'P'); }
		);
		// plow fields
		con.on(Event::Press, Key::O,
			[&con,unit_id](){ con.order_unit(unit_id, 'O'); }
		);
		// space - skip unit
		con.on(Event::Press, Key::Space,
			[&con](){ con.skip_unit(); }
		);
		
	}

	//void select_area()
		
	
	void render_map(sf::RenderWindow &win, Env const& env, Console & con, v2i const& pos,
			Coords const& delta)
	{
		
		auto w = 15;
		auto h = 12;

		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
		
				auto coords = Coords(i,j);
				
				if (env.in_bounds(coords)) {

					auto pos = ly.map.pos + vmul(ly.terr_dim, v2i(i,j));

					auto& terr = env.get_terr(coords);
					render_terr(win, pos, env, terr);

					
					// right press on terr -- select terr
					con.on(Event::Press, Button::Right, pos, ly.terr_dim, 
						[&con,coords](){
							con.select_terr(coords);
						}
					);
					
					// right press on terr with shift -- add select terr
					con.on(Event::Press, Button::Right, halo::ModButton|halo::ModShift, pos, ly.terr_dim, 
						[&con,coords](){
							con.select_terr(coords, +1);
						}
					);
					
					// right press on terr with shift -- sub select terr
					con.on(Event::Press, Button::Right, halo::ModButton|halo::ModCtrl, pos, ly.terr_dim, 
						[&con,coords](){
							cout << "ctrel\n";
							con.select_terr(coords, -1);
						}
					);
					
					// hover with button down over terr -- select set
					con.on(Event::Hover, halo::ModButton, pos, ly.terr_dim, 
						[&con,coords](){
							con.select_terr(coords);
						}
					);
					
					// hover with button down with shift over terr -- select add
					con.on(Event::Hover, halo::ModButton|halo::ModShift, pos, ly.terr_dim, 
						[&con,coords](){
							con.select_terr(coords, +1);
						}
					);
					
					// hover with button down with shift over terr -- select add
					con.on(Event::Hover, halo::ModButton|halo::ModCtrl, pos, ly.terr_dim, 
						[&con,coords](){
							con.select_terr(coords, -1);
						}
					);
					
					
				}
				
				
			}
		}
		
		
		

		
		
		// stacks on map
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				auto coords = Coords(i,j);
				if (env.in_bounds(coords)) {

					auto pos = ly.map.pos + vmul(ly.terr_dim, v2i(i,j));

					auto& terr = env.get_terr(coords);

					render_stack(win, con, pos, env, terr);

				}
			}
		}

		//for (auto &p: env.units) {
		//	render_icon(win, env, p.second);
		//}

		// render cursor on selected tile
		if (!con.get_sel_unit()) {
			if (Terr* tp = con.get_sel_terr()) {
				
				for (auto tp: con.get_sel_terrs()) {
					auto coords = env.get_coords(*tp);
					
					render_inline(win, 
						vmul(coords, ly.terr_dim) + pos,
						ly.terr_dim, 
						{128,128,128,255}
					);
				}
				
				auto coords = env.get_coords(*tp);

				// blink main terr
				if (int(con.time * 10.0) % 10 >= 5) {
					// rect frame on tile
					render_inline(win, 
						vmul(coords, ly.terr_dim) + pos,
						ly.terr_dim, 
						{255,255,255,255}
					);
				}
				
				
			}
		}
		
		
	}


	

	void render_bar(sf::RenderWindow &win,
			Env const& env,
			Console const& con,
			Vector2<int> const& pos,
			Vector2<int> const& dim
		) {
		// pos - left top pix
		// dim - size

		render_area(win, pos, dim, res("WOODTILE_SS", 1));

	}





	void render_panel(RenderWindow & win,
			Env const& env,
			Console & con,
			v2i const& pos,
			v2i const& dim
		) {

		//sf::Image img = res("COLONIZE/WOODTILE_SS", 1);  32x24

		render_area(win, pos, dim, res("WOODTILE_SS", 1));

		// terrain info
		/*
		 * Biome: Plains
		 * road
		 */

		string info;
		
		string nation_name = "EDITING";
		if (env.in_progress()) {
			nation_name = env.get_current_nation().name;
		}
		
		// Turn 5, England 
		info += "Turn " + to_string(env.get_turn_no()) + ", " + nation_name + "\n";
		
		if (con.nation_id) {			
			//info += format("You are (id): %||", env.get<Nation>(con.nation_id).get_name());
			info += format("You are (id): %||", con.nation_id);
		}
		
			
		
		if (Terr const* tp = con.get_sel_terr()) {
			auto& t = *tp;
			string phys_info;
			for (auto const& item: PHYS_NAMES) {
				auto const& phys = item.first;
				auto const& name = item.second;

				if (t.has_phys(phys)) phys_info += name + ",";
			}

			info += format("\n%||\n[%||]\n", BIOME_NAMES.at(t.biome), phys_info);
			
			info += format("\nmove: %||\nimprov: %||\n",
				env.get_movement_cost(t, t, TravelLand),
				env.get_improv_cost(t)
			);
			

		}
		
		if (auto u = con.get_sel_unit()) {
			
			info += format("\n%||\nTime left: %||/%||",
				u->get_name(),
				int(u->time_left),
				int(TIME_UNIT)
			);
			
			info += format("\nmoves: %||", misc::get_moves(*u));
			
			
			info += format("\ntransported: %||", u->get_transported());
			
			info += format("\nspace left: %||", u->get_space_left());
			
			if (u->get_item1() == ItemTools) {
				info += format("\ntools: %||", u->get_num1());
			}

		}
		
		//
		// altitude
		// biome
		// feats
			
		
		render_text2(win,
			pos,
			res_pixfont("tiny.png"),
			info,
			{0,0,0,0},
			0
		);
		
		// top right: nation indicator
		if (env.in_progress()) {
			render_pixel(win, pos + v2i(dim[0]-1,0), env.get_current_nation().color);
		}
			
		auto ren = Renderer(win, env);
			
		
		
		if (auto t = con.get_sel_terr()) {
			v2i pos = ly.pan.pos + v2i(0,100);
			v2i dim = v2i(ly.pan.dim[0], ly.terr_dim[1]*3);
			
			render_fill(win, pos, dim, 
				sf::Color(0,0,0,64)
			);
			
			render_units(win, con, pos, dim, 
				env, 
				*t
			); 
		}
		
		// End of turn/Start game
		// nothing to move -> enter to ready turn
		{
			auto lab = string();
			auto cmd = string();
			auto col = ColorNone;
			
			if (env.in_progress()) {
				auto& p = env.get_current_nation();
				
				auto & mem = con.mem;
				
				if (mem.get_next_to_repeat(env, p)) {
					lab = "Move all";
					cmd = "move-all";					
					col = ColorWhite;
					
					// Enter -> move all
					con.on(Event::Press, Key::Return,
						[&con](){ con.repeat_all(); }
					);
				}
				else {
					lab = "End of turn";
					cmd = "ready";				
					col = (mem.get_next_to_order(env, p)) ? ColorGray : ColorWhite;

					// Enter -> end turn
					con.on(Event::Press, Key::Return,
						[&con](){ con.command("ready"); }
					);
				}
			}
			else {
				lab = "Start game";
				cmd = "start";				
				col = (env.nations.size() < 1) ? ColorGray : ColorWhite;
			}

			auto text = Text(lab).set_font("tiny.png").set_fg(col);
			v2i text_pos = calc_align(ly.pan.pos, ly.pan.dim, text.get_dim(), v2f(0.5, 1.0));
								
			// label			
			render_text(win, text_pos, text);
			
			// left click -> start/ready
			con.on(Event::Press, Button::Left, text_pos, text.get_dim(),
				[&con,cmd](){ con.command(cmd); }
			);
			
		}

		
		
	}

	/*
	v2i calc_text_dim(PixFont const& font, string const& text) {
		// line height 7px (with sep)
		int const FONT_HEIGHT = 6;

		// width
		auto dim = v2i(0,0);

		auto x = 0;
		auto y = 0;

		y += 1; // horizontal separation
		for (auto c: text) {

			// newline
			if (c == '\r' or c =='\n') {
				x += 1;  // separation
				x = 0;
				y += FONT_HEIGHT;
				y += 1; // horizontal separation

			}
			else {
				auto &g = font.glyphs.at(c);
				auto &r = g.rect;

				// out of space
				if (width and x + rr.width >= width) {
					x = pos[0];
					y += FONT_HEIGHT;
					y += 1; // horizontal separation
				}

				x += 1; // vertical separation



				s.setPosition(x ,y);
				s.setTextureRect(g.rect);
				win.draw(s);


				x += rr.width;
			}
		}

	}*/
	
	
	/*
	v2i render_line(
		sf::RenderWindow &win,
				
		v2i const& pos,
		v2i const& dim,
		v2i const& align,
		
		PixFont const& font,
		sf::Color const& fgcol,
		sf::Color const& bgcol,
		
		bool const& wrap,
		
		string const& text
	)*/
	/*
	void align_box(Box & box, Box const& area, v2f align) {

		box.pos[i] = pos[i] + (area.dim[i] - box.dim[i]) * align[i];
	}*/
	
	
			
	
	
	
	v2i calc_align(Box const& par, v2i const& dim, v2f const& align) {
		return (par.pos.cast<float>() + vmul((par.dim - dim).cast<float>(), align)).cast<int>();
	}

	v2i calc_align(v2i const& p_pos, v2i const& p_dim, v2i const& dim, v2f const& align) {
		return (p_pos.cast<float>() + vmul((p_dim - dim).cast<float>(), align)).cast<int>();
	}

	
	v2i get_text_dim(PixFont const& font, string const& text) {
		// font prop
		int const FONT_HEIGHT = 6;
		
		// horizontal and vertical separation in px
		v2i sep = {1,1};
				
		// render dim
		v2i r_dim = {0,FONT_HEIGHT};
		
		// current substring [i:j>
		size_t i = 0;
		size_t j = i;
		
		// calculate rendered text line dimension without margins		
		while (j < text.size()) {
			auto& g = font.glyphs.at(text.at(j));
			r_dim[0] += g.rect.width;
			j += 1;
		}
		r_dim[0] += sep[0] * (j-i);   // horizontal letter separation
		
		// add margins
		r_dim += sep; // left, top
		r_dim += sep; // right, bottom
		
		return r_dim;
	}
	

	v2i render_text_line_min(
		sf::RenderWindow &win,
		v2i const& pos,		
		PixFont const& font,
		sf::Color const& fgcol,
		sf::Color const& bgcol,		
		string const& text
	) {
		v2i sep = {1,1};
		v2i r_dim = get_text_dim(font, text);
				
		// calculate render position taking into account the alignment
		v2i r_pos = pos;
		
		// render letters
		auto trg = sf::Image();
		trg.create(r_dim[0], r_dim[1], bgcol);
		
		size_t i = 0;
		size_t j = text.size();
		
		v2i dd = sep;
		for (auto k = i; k < j; ++k) {
						
			auto xy = dd;			
			
			auto& g = font.glyphs.at(text.at(k));
			
			font.render_glyph(trg, xy[0], xy[1], g, fgcol);
			
			dd[0] += g.rect.width;
			dd[0] += 1;
			
			
			
		}
		
		sf::Texture tex;
		tex.loadFromImage(trg);
		
		sf::Sprite s;
		s.setPosition(r_pos[0], r_pos[1]);
		s.setTexture(tex);
		win.draw(s);
		
		return {0,0};		
	}
	
	
	v2i render_text_line(
		sf::RenderWindow &win,

		v2i const& pos,
		v2i const& dim,
		v2f const& align,

		PixFont const& font,
		sf::Color const& fgcol,
		sf::Color const& bgcol,

		string const& text
	) {
		return render_text_line_min(win,
			calc_align(pos, dim, get_text_dim(font, text), align),
			font,
			fgcol,
			bgcol,
			text
		);		
	}
	
	v2i render_text(
		sf::RenderWindow &win,
		v2i const& pos,
		Text const& text		
	) {
		return render_text_line_min(win,
			pos,
			res_pixfont(text.get_font()),
			text.get_fg(),
			text.get_bg(),
			text.get_text()
		);		
	}
	
	

	v2i render_text2(
			/*sf::RenderWindow &win,
			
			v2i const& pos,
			v2i const& dim,
			v2i const& align,
			
			PixFont const& font,
			sf::Color const& fgcol,
			sf::Color const& bgcol,
			
			string const& text*/
			
			sf::RenderWindow &win,
			Vector2<int> const& pos,
			PixFont const& font,
			string const& text,
			sf::Color const& bgcol,
			int width
	
			//bool wrap = 0
		) {

		// line height 7px (with sep)
		int const FONT_HEIGHT = 6;

		// width

		auto x = pos[0];
		auto y = pos[1];

		auto &img = font.tex;

		sf::Sprite s;
		s.setTexture(img);

		//auto line_height = font.GetCharacterSize();
		//cout << "char hh:" << hh << endl;

		char last_c;
		
		//auto width = dim[0];
		
		y += 1; // horizontal separation
		for (auto c: text) {
			last_c = c;
			// newline
			if (c == '\r' or c =='\n') {
				// nice 1px ending
				if (bgcol.a != 0) {
					render_rect(win, v2i(x,y-1), v2i(1, FONT_HEIGHT+1), bgcol);
				}
				x = pos[0];
				y += FONT_HEIGHT;
				y += 1; // horizontal separation
				continue;
			}

			auto &g = font.glyphs.at(c);
			auto &rr = g.rect;

			// out of space
			if (width and x + rr.width >= width) {
				x = pos[0];
				y += FONT_HEIGHT;
				y += 1; // horizontal separation
			}

			x += 1; // vertical separation

			// background
			if (bgcol.a != 0) {
				//cout << v2i(x-1,y-1) << v2i(rr.width+1, rr.height+1) << endl;
				render_rect(win, v2i(x-1,y-1), v2i(rr.width+1, rr.height+1), bgcol);
			}

			s.setPosition(x ,y);
			s.setTextureRect(g.rect);
			win.draw(s);


			x += rr.width;
		}
		
		// nice 1px ending
		if (last_c != '\r' and last_c != '\n') {
			if (bgcol.a != 0) {
				render_rect(win, v2i(x,y-1), v2i(1, FONT_HEIGHT+1), bgcol);
			}
		}
		
		return v2i(x,y-1);
	}

	/*
	int16 render_text(
			sf::RenderWindow &win,
			Vector2<int16> const& pos,
			sf::Font const& font,
			string const& text,
			int16 width = 0
		) {

		// width

		auto x = pos[0];
		auto y = pos[1];

		auto &img = font.getTexture();

		((sf::Image&)img).SetSmooth(false);

		sf::Sprite s;
		s.setTexture(img);

		//auto line_height = font.GetCharacterSize();
		//cout << "char hh:" << hh << endl;

		for (auto c: text) {
			auto &g = font.getGlyph(c);
			auto &rr = g.Rectangle;

			auto &r = g.TexCoords;

			s.setPosition(x + rr.left ,y + rr.top + 6);

			auto dim = img.getSize();

			s.setTextureRect(sf::Rect<int>(
				(r.left * dim.x) +0.5,
				(r.top * dim.y) +0.5,
				(r.width * dim.x) +0.5,
				(r.height * dim.y) +0.5
			));
			win.draw(s);

			x += g.Advance;
		}

		return x;
	}
	*/

	void render_cmd(sf::RenderWindow & app, col::Console & con) {
		if (con.is_active()) {

			auto ln = con.output.size();
			v2i pos = v2i(ly.map.pos[0], ly.map.pos[1]);

			for (auto& line: boost::adaptors::reverse(con.output)) {
				
				auto t = Text(line + '\r').set_font("tiny.png").set_bg({0,0,0,128});
				render_text(
					app,
					pos,
					t
				);
					
				pos[1] += t.get_dim()[1];
			}

		
			render_text(
				app,
				ly.scr.pos,
				Text(con.buffer + "_").set_font("tiny.png")				
			);
		
			// trap all keyboard
			con.on(Dev::Keyboard);
			
			// char entered
			con.on(Event::Char, [&con](halo::Pattern const& p){
				con.handle_char(*p.unicode);
			});
			
			// deactivate
			con.on(Event::Char, u'`', [&con](){
				con.set_active(false);
			});
				
			// history up
			con.on(Event::Press, Key::Up, [&con](){
				con.history_up();
			});
			
			// history down
			con.on(Event::Press, Key::Down, [&con](){
				con.history_down();
			});
			
			
		}
		else {
			// activate
			con.on(Event::Char, u'`', [&con](){
				con.set_active(true);
			});
		}

	}

	
	void render_cursor(sf::RenderWindow & win, Console const& con) {
		if (con.drag_item) {
			
			sf::Vector2f mp = win.mapPixelToCoords(sf::Mouse::getPosition(win));

			auto pos = v2i(mp.x, mp.y);
			//print("cursor: %||, %||\n", mp.x, mp.y);

			render_sprite(win, pos, res(ICON, get_item_icon_id(con.drag_item)));
		}
	}

	
	
	void render_drag_clear(Console & con) {
		if (con.drag_item) {
			// left release anywhere -- end drag cargo
			con.on(Event::Release, Button::Left,
				[&con](){ con.drop_cargo(0); }
			);
		}
	}
	
	
	void render(sf::RenderWindow &app, col::Env & env, col::Console & con) {
		app.clear();
		con.reset_hotspots();

		int w = app.getSize().x / conf.global_scale;
		int h = app.getSize().y / conf.global_scale;

		render_drag_clear(con);
		
		// bar top
		render_bar(app, env, con, {0,0}, {w,7});
		// hline under the bar
		render_rect(app, {0,7}, {w,1}, {0,0,0,255});


		if (con.mode == Console::Mode::COLONY) {
			render_city(app, env, con);
		}
		else {
			// map
			render_map(app, env, con, {0,8}, Coords(0,0));

			// panel right
			render_panel(app, env, con,
				ly.pan.pos,
				ly.pan.dim
			);
			// vline left of the panel
			render_rect(app,
				{ly.scr.dim[0] - ly.pan.dim[0] - 1, ly.bar.end[1] + 1},
				{1, h-8},
				{0,0,0,255}
			);
		}

		//if (env.curr_nation) {
		//	render_nationind(app, 0, 0, env.curr_nation->color);
		//}

		// BACKGROUND

		render_cmd(app, con);

		render_cursor(app, con);
		
		app.display();

	}

	
	
	

}
