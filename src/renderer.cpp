#include "renderer.h"

//#include <functional>
#include <boost/range/adaptor/reversed.hpp>

#include "view_base.h"

namespace col {

	using b2i = Box;
	

	Layout const ly(SCREEN_W, SCREEN_H);


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
			auto path = string(FONT_PATH + name);
			if (!font.loadFromFile(path)) {

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
			auto path = FONT_PATH + name;
			font.load(path);
			return font;
		}
	}


	v2i render_text(
		sf::RenderWindow &win,
		Vector2<int> const& pos,
		PixFont const& font,
		string const& text,
		sf::Color const& bgcol,
		int width
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

	string const
		TERR = RES_PATH + "TERRAIN_SS",
		ICON = RES_PATH + "ICONS_SS",
		PHYS = RES_PATH + "PHYS0_SS",
		ARCH = RES_PATH + "PARCH_SS",
		BUILD = RES_PATH + "BUILDING_SS",
		DIFFUSE = "DIFFUSE*",
		COAST = "*COAST*";

	Res::mapped_type const& res(string const& d, uint const& i) {

		auto key = Res::key_type(d,i);
		auto p = g_res.find(key);
		if (p != g_res.end()) {
			return (*p).second;
		}
		else {
			auto &img = g_res[key];
			auto path = str(format("./%||/%|03|.png") % d % i);
			if (!img.loadFromFile(string(path))) {
				// try default
				if (!img.loadFromFile(string("./" + ICON + "/065.png"))) {
					throw std::runtime_error("can't load default image file while loading: " + path);
				}
				// throw std::runtime_error("cant't load image file: " + path);
			}
			img.setSmooth(false);
			return img;
		}
	}


	
	
	sf::Color const ColorSelectedBG = sf::Color(0x3c,0x20,0x18,0xff);
	sf::Color const ColorFont = sf::Color(0x55,0x96,0x34,0xff);
	sf::Color const ColorNone = sf::Color(0,0,0,0);
	sf::Color const ColorBlack = sf::Color(0,0,0,255);
	
	v2f vmul(v2f const& a, v2f const& b);
	v2f vmul(v2i const& a, v2f const& b);

	
	v2i calc_align(Box const& par, v2f const& align, v2i const& dim);

	void preload(string const& d, uint const& i, Res::mapped_type const& tex) {
		auto key = Res::key_type(d,i);
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


	void render_shield(sf::RenderWindow &win, int16 x, int16 y, const Color &col) {
		win.draw(
			RectShape(
				v2i(x,y) + v2i(1,1),
				{6, 8},
				{col.r,col.g,col.b, 255},
				1,
				{0,0,0, 255}
			)
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
	
	void render_outline(sf::RenderWindow &win, v2i const& pos, v2i const& dim, Color const& c) {
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

	void render_inline(sf::RenderWindow &win, v2i const& pos, v2i const& dim, Color const& c) {
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
		sf::RenderWindow &win,
		sf::Texture const& tex,
		Vector2<int> const& area_pos,
		Vector2<int> const& area_dim
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

	void Renderer::fill(Texture const& tex, v2i const& pos, v2i const& dim) {
		render_area(win, tex, pos, dim);
	}
	
	void Renderer::fill(Color const& color, v2i const& pos, v2i const& dim) {
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
		
	

	
	void render_area(
		sf::RenderWindow &win,
		sf::Texture const& tex,
		Vector2<int> const& area_pos,
		Vector2<int> const& area_dim)
	{
		using sf::Sprite;
		using sf::IntRect;
		using sf::Vector2f;

		auto tile_dim = Vector2<int>(tex.getSize().x, tex.getSize().y);
		auto area_end = area_pos + area_dim;

		int ei,ej;

		//cerr << format("area_dim: %||\n") % area_dim;
		//cerr << format("area_pos: %||\n") % area_pos;
		//cerr << format("tile_dim: %||\n") % tile_dim;

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

		//cerr << format("e: %||,%||\n") % ei % ej;


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
	
	
	
	
	template<typename K>
	void render_select(sf::RenderWindow & win, Console & con,
		std::function<v2i(v2i const&)> cpos,
		//Box const& par, v2f const& align, 
		vector<pair<K, string>> const& kvs,
		K & selected,
		std::function<void()> onselect,
		std::function<void()> oncancel
	) {
		
		int const LINE_HEIGHT = 10;
		
		v2i const dim = v2i(ly.scr.dim[0] * 0.3, (kvs.size()+1) * LINE_HEIGHT);
		v2i pos = cpos(dim);
		
		// on cancel (click outside window))
		con.onclick(ly.scr.pos, ly.scr.dim, oncancel);
		
		// background
		render_area(win, res(RES_PATH + "WOODTILE_SS", 1), pos, dim);		
		
		// on dialog
		con.onclick(pos, dim, [](){});
				
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
			
			// on select		
			con.onclick(p, d, [&selected,key,onselect](){ selected = key; onselect(); });
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
		render.fill(sand_tex, ly.city.pos, ly.city.dim);

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


		auto& terr = env.get_terr(con.sel);
		auto& col = terr.get_colony();

		// render buildings
		{
			
			con.onhover(ly.scr.pos, ly.scr.dim,
				[&con]() { con.sel_colony_slot_id = -1; }				
			);
			
			// render in reverse order to make sure that docks clickarea will not obscure other builds
			int i = col.builds.size();
			while (i > 0) {
				i = i - 1;
				
				auto workplace_id = i;
				
				auto& b = col.builds.at(i);
				auto& build_tex = res(BUILD, b.get_type_id());
				auto build_pos = ly.city.pos + pixs.at(i);
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
				if (b.get_hammers() != b.get_cost_hammers()) 
				{
					// number of produced items
					
					
					
					//render_sprite(win, build_pos, res(ICON, get_item_icon_id(ItemHammers)));
					
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
				
				
				con.onclick(build_pos, build_dim,
					[&con, workplace_id]() { 
						if (con.sel_unit_id != 0) {
							con.command(str(format("work-build %||") % workplace_id));
						}
						else {
							con.select_build = 1;  // todo slot_id blablabla
							con.sel_slot_num = workplace_id;
						}
					}	
				);
				
				con.onhover(build_pos, build_dim,
					//str(format("sel_place %||") % i)
					[&con, i]() { con.sel_colony_slot_id = i; }
				);
				
				v2i units_frame = {25, 16};
				
				int n = b.units.size();
				int i = 0;
				for (auto& unit_ptr: b.units) {
					auto& unit = *unit_ptr;
					auto& unit_tex = res(ICON, unit.get_type_id());

					v2i unit_pos = calc_align(
						Box(
							build_pos + build_dim - units_frame + v2i(1,2), units_frame
						), 
						v2f(float(i+1)/float(n+1), 1), 
						get_dim(unit_tex)
					);
					
					v2i unit_dim = get_dim(unit_tex);

					v2i sel_pos = unit_pos;
					v2i sel_dim = unit_dim;
					
					render_sprite(win, unit_pos, unit_tex);

					auto unit_id = unit.id;

					if (unit_id == con.sel_unit_id) {
						// render selection frame
						render_inline(win, sel_pos, sel_dim, {255,100,100,255});
					}
					else {
						// or add zone for click select
						con.onclick(sel_pos, sel_dim,
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
			auto pix = ly.city_fields.pos;

			auto city_terr_pos = ly.city_fields.pos + ly.terr_dim;
			auto& city_terr = terr;
			
			auto pos = env.get_coords(terr);
			auto city_coords = Coords(pos[0]-1, pos[1]-1);
			
			
			for(size_t field_id = 0; field_id < col.fields.size(); ++field_id) {
				auto& field = col.fields.at(field_id);
				
				auto& field_terr = field.get_terr();
				
				v2i relc = (env.get_coords(*field.terr) - env.get_coords(city_terr)).cast<int>();
				auto field_pos = city_terr_pos + vmul(relc, ly.terr_dim);
				auto field_dim = ly.terr_dim;
				
				
				render_terr(win, env.get_coords(field_terr), env, field_terr, pix,
					city_coords
				);
															
				string cmd = str(format("work-field %||") % field_id);
				con.onclick(field_pos, field_dim,
					[&con,cmd](){ con.command(cmd); }
				);

				
				for (auto& unit_p: field.units) {
					auto& unit = *unit_p;
					
					v2i unit_pos = field_pos;
					v2i unit_dim = field_dim;

					v2i sel_pos = unit_pos;
					v2i sel_dim = unit_dim;
					
					auto& unit_tex = res(ICON, unit.get_type_id());
					render_sprite(win, 
						calc_align(Box(unit_pos, unit_dim), v2f(0.5, 0.5), get_dim(unit_tex)),
						unit_tex
					);

					auto unit_id = unit.id;

					if (unit_id == con.sel_unit_id) {
						// render selection frame
						render_inline(win, sel_pos, sel_dim, {255,100,100,255});
					}
					else {
						// or add zone for click select
						con.onclick(sel_pos, sel_dim,
							[&con,unit_id]() { 
								con.command("sel " + to_string(unit_id)); 					
							}
						);
					}
				}
				
				
			}
			
			
			/*
			int field_id = 16;
			for (int j = pos[1]-1; j <= pos[1]+1; ++j) {
				for (int i = pos[0]-1; i <= pos[0]+1; ++i) {
					
					auto& terr = env.get_terr(Coords(i,j));
					
					auto delta_coords = Coords(i,j) - base_coords;
					
					render_terr(win, Coords(i, j), env, terr, pix,
						base_coords
					);
															
					string cmd = str(format("work %||") % field_id);					
					con.onclick(pix + v2i(delta_coords[0], delta_coords[1]) * TILE_DIM, tile_dim,
						[&con,cmd](){ con.command(cmd); }
					);
					
					field_id += 1;
				}
			}
			 */
		}

		

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
		
		
		// right click - unselect unit
		con.on(Console::HotSpot::RightClick, ly.scr.pos, ly.scr.dim,
			[&con]() { 
				con.sel_unit_id = 0;
			}
		);

		int i = 0;
		for (auto& p: terr.units) {
			auto& unit = *p;
			
			auto& unit_tex = res(ICON, unit.get_type_id());
			v2i unit_pos = ly.city_units.pos + v2i(ly.terr_dim[0] * i, 0);
			v2i unit_dim = ly.terr_dim;
			
			v2i sel_pos = unit_pos;
			v2i sel_dim = unit_dim;
			
			// render
			render_sprite(win, 
				calc_align(Box(unit_pos, unit_dim), v2f(0.5, 0.5), get_dim(unit_tex)),
				unit_tex
			);
			
			auto unit_id = unit.id;
						
			if (unit_id == con.sel_unit_id) {
				// render selection frame
				render_inline(win, sel_pos, sel_dim, {255,100,100,255});
			}
			else {
				// or add zone for click select
				con.onclick(sel_pos, sel_dim,
					[&con,unit_id]() { 
						con.command("sel " + to_string(unit_id)); 					
					}
				);
			}
			
			++i;
		}
		
		
			
			/*
			if (unit.place->place_type() != PlaceType::Terr) {
				
				// produced item
				auto const& item_tex = res(ICON, get_item_icon_id(unit.workitem));
				auto const& item_dim = get_dim(item_tex);
				v2i item_pos;
				
				
				
				if (unit.workplace->place_type() == PlaceType::Build) {
					
					
					// unit on building
					int i = col.index_of(*static_cast<Build const*>(unit.workplace));

					//auto& build = col.builds.at(i);
					//auto& build_tex = res(BUILD, build.get_type_id());
					//auto build_dim = get_dim(build_tex);
					
					auto const& build_pos = ly.city.pos + pixs.at(i);
					auto const& build_end = ly.city.pos + pixs.at(i) + dims.at(i);
					
					// unit
					unit_dim = get_dim(unit_tex);					
					unit_pos = build_end - v2i(num_workers.at(i)*7 + 5, -1) - unit_dim;
					
					// item
					item_pos = build_pos + v2i(num_workers.at(i) * 7, -1);
					
					num_workers.at(i) += 1;
										
					
					// selection
					sel_pos = unit_pos - v2i(1,1);;
					sel_dim = unit_dim + v2i(2,2);
					
					
				}
				else  {
					// unit on field
					assert(unit.workplace->place_type() == PlaceType::Terr);
					auto const& t  = *static_cast<Terr *>(unit.workplace);
					auto const& cen = env.get_coords(t) - env.get_coords(terr) + Coords(1,1);

					// sel
					sel_pos = ly.city_fields.pos + v2i(cen[0], cen[1]) * TILE_DIM;
					sel_dim = v2i(TILE_DIM, TILE_DIM);;
										
					// unit
					unit_pos = sel_pos + v2i(TILE_DIM/2, 0);
					unit_dim = v2i(TILE_DIM/2, TILE_DIM);
					
					// item		
					item_pos = sel_pos;
					
				}
				
				if (unit.workitem) {
					auto num = unit.workplace->get_yield(unit.workitem, 0);

					// render produced item
					render_sprite(win,
							item_pos,
							item_dim + v2i(2,2),
							item_tex					
					);
					for (int i=0; i < num; ++i) {
						render_sprite(win,
							item_pos + v2i(i*3,0),
							item_dim + v2i(2,2),
							item_tex					
						);
					}

					// number of produced items
					render_text(
						win,
						item_pos,
						res_pixfont("tiny.png"),
						std::to_string(num),
						{0,0,0,255},
						0
					);

				}
				
			}
			else {
				// unit on fence
				int i = 14;
				auto& pix = pixs[i];				
				
				// unit
				unit_pos = pix + v2i(num_workers.at(i)*5 + 10, 15);
				unit_dim = get_dim(unit_tex);
				num_workers.at(i) += 1;
				
				// sel
				sel_pos = unit_pos;
				sel_dim = unit_dim;
						

			}
			*/
			
		
		
		
		


		// render storage

		render.fill({76,100,172,255}, ly.city_res.pos, ly.city_res.dim);

		int width = 16;

		auto pix = ly.city_res.pos;
		for (auto item: COLONY_ITEMS) {

			// num of items
			int num = 0;
			auto p = col.storage.find(item);
			if (p != col.storage.end()) {
				num = (*p).second;
			}

			// render icon
			render_sprite(win,
				pix,
				v2i(width,12),
				res(ICON, get_item_icon_id(item))
			);

			// render number
			render_text(
				win,
				pix + v2i(0,1+12),
				res_pixfont("tiny.png"),
				std::to_string(num),
				{0,0,0,0},
				0
			);



			pix[0] += width;
		}
		
		// render EXIT button
		render.fill({140,0,140,255}, ly.city_exit.pos, ly.city_exit.dim);
		
		render_text(
			win,
			ly.city_exit.pos + v2i(1,1),
			res_pixfont("tiny.png"),
			"Ret",
			{0,0,0,0},
			0
		);

		con.onclick(ly.city_exit.pos, ly.city_exit.dim,
			[&con, &terr](){ con.command("exit"); }
		);

		// render fields
		//render_fields(win, env, con, ly.city_fields.pos, );

		if (con.select_build > 0) {
			
			vector<pair<BuildType::Id, string>> kvs;
			
			auto& cb = env.get_terr(con.sel).get_colony().get_build(con.sel_slot_num);
			
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
					return calc_align(Box(ly.scr), {0.5, 0.5}, dim); 
				},
				kvs,
				con.select_build,
				[&con](){ 
					con.command(str(format("construct %|| %||") % con.sel_slot_num % con.select_build));
					con.select_build = 0;
					con.sel_slot_num = -1;
				},  // onselect
				[&con](){ con.select_build = 0; }   // oncancel
			);
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
		if ( get(loc, Dir::W).has(phys) ) idx |= 8;
		if ( get(loc, Dir::X).has(phys) ) idx |= 4;
		if ( get(loc, Dir::A).has(phys) ) idx |= 2;
		if ( get(loc, Dir::D).has(phys) ) idx |= 1;
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
		
		render_sprite(win, pix, res(DIFFUSE, b + offset) );
		
	}


	
	//int const TextureMountainId;
	//int const TextureHillId;


	void render_terr(sf::RenderWindow &win,
			Coords const& pos,
			Env const& env,
			Terr const& terr,
			Vector2<int> const& map_pix,
			Coords const& delta)
	{

		auto biome = terr.biome;
		auto forest = terr.has(PhysForest);
		
		auto hill = (terr.get_alt() == HILL_LEVEL);
		auto mountain = (terr.get_alt() == MOUNTAIN_LEVEL);

		int x = (pos[0] - delta[0]) * ly.TERR_W + map_pix[0];
		int y = (pos[1] - delta[1]) * ly.TERR_H + map_pix[1];
		auto pix = v2i(x,y);


		auto loc = make_terr_ext(env, pos);

		// render biome
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
			
			if (terr.has(PhysSeaLane)) {
				base = 50;
			}
			else {
				base = 0;
			}
			
			uint8 const h = TILE_DIM >> 1;

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
		if (terr.get_alt() == MOUNTAIN_LEVEL) {
			render_sprite(win, pix, res(PHYS, 33 + get_wxad_index_level(loc, terr.get_alt())));
		}

		if (terr.get_alt() == HILL_LEVEL) {
			render_sprite(win, pix, res(PHYS, 49 + get_wxad_index_level(loc, terr.get_alt())));
		}

		// phys feats & improvments
		if (terr.has(PhysPlow)) {
			render_sprite(win, pix, res(PHYS, 150));
		}

		if (terr.has(PhysForest)) {
			render_sprite(win, pix, res(PHYS, 65 + get_wxad_index(loc, PhysForest)));
		}

		if (terr.has(PhysMajorRiver)) {
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
					if ( get(loc, CLOCKWISE_DIRS[i]).has(PhysMajorRiver) ) {
						render_sprite(win, pix, res(PHYS, 141 + (i >> 1)));
					}
				}
			}

		}
		else if (terr.has(PhysMinorRiver)) {
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
					if ( get(loc, CLOCKWISE_DIRS[i]).has(PhysMinorRiver) ) {
						render_sprite(win, pix, res(PHYS, 145 + (i >> 1)));
					}
				}
			}
		}

		if (terr.has(PhysRoad)) {
			bool r = false;
			for (int i=0; i<8; ++i) {
				if ( get(loc, CLOCKWISE_DIRS[i]).has(PhysRoad) ) {
					render_sprite(win, pix, res(PHYS, 82 + i));
					r = true;
				}
			}
			if (!r) {
				render_sprite(win, pix, res(PHYS, 81));
			}
		}

	}

	using sf::RenderWindow;


	void render_unit(sf::RenderWindow &win,
			Console & con,
			Coords const& pos,
			Env const& env,
			Terr const& terr,
			Vector2<int> const& map_pix,
			Coords const& delta)
	{
		int x = (pos[0] - delta[0]) * ly.TERR_W + map_pix[0];
		int y = (pos[1] - delta[1]) * ly.TERR_H + map_pix[1];
		auto pix = v2i(x,y);

		// colony icon
		if (terr.has_colony()) {
			// render colony
			render_sprite(win, pix[0], pix[1], res(ICON, 4));
			
			con.onclick(pix, v2i(TILE_DIM, TILE_DIM),
				[&con, pos](){ 
					con.command(str(format("sel %|| %||") % pos[0] % pos[1]));
					con.command("enter"); 
				}
			);
		}

		// unit or colony flag
		if (terr.has_units()) {
			auto& unit = env.get_defender(terr);

			// tile owner
			auto& player = unit.get_player();

			if (terr.has_colony()) {
				// render owner flag over colony (unit in garrison)
				render_sprite(win, pix[0] + 5, pix[1], res(ICON, player.get_flag_id()));
				
			}
			else {

				auto tile_dim = v2i(TILE_DIM, TILE_DIM);
				auto icon = res(ICON, unit.get_icon());
				//cerr << "rend unit at " << pix << endl;
				// render unit in field
				render_shield(win, pix[0], pix[1], player.get_color());
				render_sprite(win, 
					calc_align(Box(pix, tile_dim), v2f(0.5, 0.5), get_dim(icon)), 
					icon
				);
			}
		}
	}



	void render_map(sf::RenderWindow &win, Env const& env, Console & con, v2i const& pos,
			Coords const& delta)
	{
		auto w = env.w;
		auto h = env.h;

		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				render_terr(win, Coords(i, j), env, env.get_terr(Coords(i,j)), pos,
					delta 
				);
				
				/*
				auto dim = v2i(w * ly.TERR_W, h * ly.TERR_H);
		
				//(mp.x - ly.map.pos[0])

				con.on(HotSpot::RightClick, pos, dim, [&con](){

						con.sel[0] = rel[0] / ly.TERR_W;
						con.sel[1] = rel[1] / ly.TERR_H;
					}
				};*/

				
			}
		}
		
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				render_unit(win, con, Coords(i, j), env, env.get_terr(Coords(i,j)), pos, delta);
			}
		}

		//for (auto &p: env.units) {
		//	render_icon(win, env, p.second);
		//}

		// cursor
		render_inline(win, 
			v2i(con.sel[0] * TILE_DIM + pos[0], con.sel[1] * TILE_DIM + pos[1]),
			tile_dim, 
			{255,255,255,255}
		);
		
		

		
	}



	/*
	void render_rect(sf::RenderWindow &win,
			sf::IntRect const& rr,
			sf::Color const& col)
	{
		sf::RectangleShape r;
		r.setSize(sf::Vector2f(rr.width, rr.height));
		r.setFillColor(col);
		//rectangle.setOutlineColor(sf::Color(0,50,0, 255));
		//rectangle.setOutlineThickness(1);
		r.setPosition(rr.left, rr.top);
		win.draw(r);
	}
	 */

	
	
	

	

	void render_bar(sf::RenderWindow &win,
			Env const& env,
			Console const& con,
			Vector2<int> const& pos,
			Vector2<int> const& dim
		) {
		// pos - left top pix
		// dim - size

		render_area(win, res(RES_PATH + "WOODTILE_SS", 1), pos, dim);

		/*
		if (env.in_bounds(con.sel)) {
			auto& t = env.get_terr(con.sel);
			if (t.units.size()) {
				Unit const* u = env.get_defender_if_any(t);

				auto moves = u->time_left;

				auto s = boost::str(format("%||\nmoves: %||") % u->get_name() % moves);

				render_text(win,
					pos,
					res_pixfont("tiny.png"),
					s,
					dim[0]
				);
			}
		}
		*/

	}







	void render_panel(sf::RenderWindow &win,
			Env const& env,
			Console & con,
			v2i const& pos,
			v2i const& dim
		) {

		//sf::Image img = res("COLONIZE/WOODTILE_SS", 1);  32x24

		render_area(win, res(RES_PATH + "WOODTILE_SS", 1), pos, dim);

		// terrain info
		/*
		 * Biome: Plains
		 * road
		 *
		 *
		 */

		string info;
		
		string player_name = "EDITING";
		if (env.in_progress()) {
			player_name = env.get_current_player().name;
		}
		
		
		
		
		// Turn 5, England 
		info += "Turn " + to_string(env.get_turn_no()) + ", " + player_name + "\n";
			
		if (env.in_bounds(con.sel)) {

			auto& t = env.get_terr(con.sel);


			string phys_info;
			for (auto const& item: PHYS_NAMES) {
				auto const& phys = item.first;
				auto const& name = item.second;

				if (t.has(phys)) phys_info += name + ",";
			}

			info += boost::str(
				format("\n%||\n[%||]\n") %
					BIOME_NAMES.at((t.biome)) % phys_info
			);



			if (t.units.size()) {
				Unit const& u = env.get_defender(t);

				info += boost::str(
					format("\n%||\nTime left: %||/%||") %
						u.get_name() % int(u.time_left) % int(TIME_UNIT)
				);

			}
		}
		
		//
		// altitude
		// biome
		// feats
			
		
		render_text(win,
			pos,
			res_pixfont("tiny.png"),
			info,
			{0,0,0,0},
			0
		);
		
		// top right: player indicator
		if (env.in_progress()) {
			render_pixel(win, pos + v2i(dim[0]-1,0), env.get_current_player().color);
		}
			
		auto ren = Renderer(win, env);
			
		// render START / NEXT TURN button
		{
			string lab, cmd;
			if (env.in_progress()) {
				lab = "Ready";
				cmd = "ready";
			}
			else {
				lab = "Start";
				cmd = "start";
			}

			ren.fill({140,0,140,255}, ly.city_exit.pos, ly.city_exit.dim);

			render_text(
				win,
				ly.city_exit.pos + v2i(1,1),
				res_pixfont("tiny.png"),
				lab,
				{0,0,0,0},
				0
			);

			con.onclick(ly.city_exit.pos, ly.city_exit.dim,
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
	
	
			
	
	
	
	v2i calc_align(Box const& par, v2f const& align, v2i const& dim) {
		return (par.pos.cast<float>() + vmul((par.dim - dim).cast<float>(), align)).cast<int>();
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
				
		// calculate render position taking into account the alignment
		v2i r_pos = (pos.cast<float>() + vmul((dim - r_dim).cast<float>(), align)).cast<int>();
		
		// render letters
		auto trg = sf::Image();
		trg.create(r_dim[0], r_dim[1], bgcol);
		
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
	

	v2i render_text(
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

			//cout << format("c=%1% x,y=%2%,%3%\n") % c % x % y;
			//cout << format("%1%;%2%;%3%;%4%\n") % r.Left % r.Top % r.Right % r.Bottom;

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

	void render_cmd(sf::RenderWindow & app, col::Console const& con) {
		if (con.active == ActiveNone) {
			return;
		}
		
		
		auto ln = con.output.size();
		v2i pos = v2i(ly.map.pos[0], ly.map.pos[1]);

		for (auto& line: boost::adaptors::reverse(con.output)) {
			pos = render_text(
				app,
				pos,
				res_pixfont("tiny.png"),
				line + '\r',
				{0,0,0,128},
				0
			);
		}

		if (con.active == ActiveCmd) {
			render_text(
				app,
				ly.scr.pos,
				res_pixfont("tiny.png"),
				con.buffer + "_",
				{0,0,0,0},
				0
			);
		}

	}

	


	void render(sf::RenderWindow &app, col::Env & env, col::Console & con) {
		app.clear();
		con.reset_hotspots();

		int w = app.getSize().x / GLOBAL_SCALE;
		int h = app.getSize().y / GLOBAL_SCALE;

		//cout << format("%||,%||\n") % w % h;

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

		//if (env.curr_player) {
		//	render_playerind(app, 0, 0, env.curr_player->color);
		//}

		// BACKGROUND

		render_cmd(app, con);

		app.display();

	}



}