#include "renderer.h"

#include <boost/range/adaptor/reversed.hpp>

#include "view_base.h"

namespace col {
	
	
	
	Layout const ly(320, 200);

	
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
			auto path = str(format("./font/%||") % name);
			if (!font.loadFromFile(path)) {
				
				throw std::runtime_error("cannot load font");
			}
			// set size
			return font;
		}		
	}
	
	
	ResPixFont::mapped_type const& res_pixfont(string const& name) {
		auto p = g_pixfonts.find(name);
		if (p != g_pixfonts.end()) {
			return (*p).second;
		}
		else {
			auto &font = g_pixfonts[name];
			auto path = str(format("./font/%||") % name);
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
	
	
	
	string const
		TERR = "COLONIZE/TERRAIN_SS",
		ICON = "COLONIZE/ICONS_SS",
		PHYS = "COLONIZE/PHYS0_SS",
		ARCH = "COLONIZE/PARCH_SS",
		BUILD = "COLONIZE/BUILDING_SS",
		DIFFUSE = "DIFFUSE*",
		COAST = "COAST*";

	Res::mapped_type const& res(string const& d, uint const& i) {

		auto key = Res::key_type(d,i);
		auto p = g_res.find(key);	
		if (p != g_res.end()) {
			return (*p).second;
		}
		else {
			auto &img = g_res[key];
			auto path = str(format("./%||/%|03|.png") % d % i);
			if (!img.loadFromFile(path)) {
				// try default
				if (!img.loadFromFile("./" + ICON + "/065.png")) {
					throw std::runtime_error("can't load default image file while loading: " + path);
				}
				// throw std::runtime_error("cant't load image file: " + path);
			}
			img.setSmooth(false);
			return img;		
		}
	}
	
	
	
	
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
		
		for (auto j = 0; j < out.getSize().y; ++j) {
			for (auto i = 0; i < out.getSize().x; ++i) {
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
	
	v2i get_loffset(int l) {
		switch(l){
			case 0: return v2i(0,0);
			case 1: return v2i(8,0);
			case 2: return v2i(8,8);
			case 3: return v2i(0,8);
		}
	}
	
	void preload_coast(Biome::type bio, int start) {
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
	
	void preload_coast() {		
		// 109 - 140
		preload_coast(Biome::Ocean, 0);
		preload_coast(Biome::SeaLane, 50);		
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
	
	
	sf::RectangleShape RectShape(
			Vector2<int16> const& pos, 
			Vector2<int16> const& dim,
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
				Vector2<int16>(x,y) + Vector2<int16>(1,1), 
				{6, 8}, 
				{col.r,col.g,col.b, 255},
				1, 
				{0,0,0, 255}
			)
		);
	}
	
	
	void render_playerind(sf::RenderWindow &win, uint16 x, uint16 y, const Color &col) {
		win.draw(
			RectShape(
				Vector2<int16>(x, y), 
				Vector2<int16>(1, 1), 
				sf::Color(col.r,col.g,col.b, 255),
				0, 
				sf::Color(0,0,0, 0)
			)
		);
	}
	
	void render_cursor(sf::RenderWindow &win, uint16 x, uint16 y) {
		win.draw(
			RectShape(
				Vector2<int16>(x+1, y+1), 
				Vector2<int16>(14, 14), 
				sf::Color(0,0,0, 0),
				1, 
				sf::Color(255,255,255, 255)
			)
		);		
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
	
	
	

	void render_city(sf::RenderWindow &win, Env const& env, Console const& con) {
		
		auto bg = res(ARCH, 1);
		
		auto w = bg.getSize().x;
		auto h = bg.getSize().y;
		
		for (uint8 j = 0; j < 5; ++j) {
			for (uint8 i = 0; i < 7; ++i) {
				render_sprite(win, i*w, 8+j*h, bg);
			}
		}
		
		// tree pattern
		uint16 TREES = 43;
		uint16 HILL = 44;
		uint16 TWO = 45;
		uint16 COAST = 46;
		uint16 B1 = TWO;
		uint16 B2 = HILL;
		uint16 B3 = TREES;
		
		vector<Coords> coords({
			Coords(7,15), Coords(54,22), Coords(90,13), Coords(144,13), Coords(172,15),
			Coords(7,42), Coords(37,45), Coords(66,52), Coords(96,52), Coords(129,54),
			Coords(9,77), Coords(14,100), Coords(68,87), Coords(124,58), Coords(121,107)
		});
		
		vector<uint16> trees{
			HILL, B1, B3, B1, B1,
			B1, TWO, B1, TWO, HILL,
			HILL, B2, TREES, COAST, B2
		};
		
		
		uint i = 0;
		for (auto &iunit: env.units) {
			auto & unit = iunit.second;
			
			auto upos = env.get_coords(unit);
			
			if (upos != con.sel) {
				continue;
			}
			
			auto type = unit.type->id;
			if (type < 256 || upos != con.sel) {
				continue;
			}
			
			auto & pos = coords[i];
			switch (type/ 256) {
				case 1:
					render_sprite(win, pos[0], pos[1], res(BUILD, type % 255));
					break;
				default:
					render_sprite(win, pos[0], pos[1], res(BUILD, -1));
					break;
			}
			
			
			//cout << unit << endl;
			++i;
		}
		
		for (; i<15; ++i) {		
			auto & pos = coords[i];
			render_sprite(win, pos[0], pos[1], res(BUILD, trees[i]));
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
	
	int get_wxad_index(LocalArea const& loc, Phys::type const& phys) {
		int idx = 0;
		if ( get(loc, Dir::W).has(phys) ) idx |= 8;
		if ( get(loc, Dir::X).has(phys) ) idx |= 4;
		if ( get(loc, Dir::A).has(phys) ) idx |= 2;
		if ( get(loc, Dir::D).has(phys) ) idx |= 1;
		return idx;
	}
	
	array<Dir::t,8> CLOCKWISE_DIRS = {
		Dir::W, Dir::E, Dir::D, Dir::C, Dir::X, Dir::Z, Dir::A, Dir::Q
	};
	
			
	
	int get_coast_index(uint8 const& l, Terr const& t0, Terr const& t1, Terr const& t2) {
		uint8 k = 
			(!is_water_biome(t2.biome) << 2) +
			(!is_water_biome(t1.biome) << 1) +
			(!is_water_biome(t0.biome) << 0);
		return (k<<2) + l;
	}
	
	/*
	Biome::type get_render_biome(LocalArea const& loc) {
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
		if (!is_water_biome(b)) {
			render_sprite(win, pix, res(DIFFUSE, b + offset) );
		}
	}
	
	void render_terr(sf::RenderWindow &win, 
			Coords const& pos, 
			Env const& env,
			Terr const& terr,
			Vector2<int> const& map_pix)
	{
		
		auto biome = terr.biome;
		auto forest = terr.has(Phys::Forest);
		auto hill = terr.has(Phys::Hill);
		auto mountain = terr.has(Phys::Mountain);
		
		int x = pos[0] * ly.TERR_W + map_pix[0];
		int y = pos[1] * ly.TERR_H + map_pix[1];
		auto pix = v2i(x,y);
		
		
		auto loc = make_terr_ext(env, pos);
		
		// render biome
		render_sprite(win, pix, res(TERR, (biome)));
		
		// render neighs pattern	
		render_diffuse_from(win, pix, loc, Dir::W, 0);
		render_diffuse_from(win, pix, loc, Dir::D, 50);
		render_diffuse_from(win, pix, loc, Dir::X, 100);
		render_diffuse_from(win, pix, loc, Dir::A, 150);
		
		
		// coast
		// 01
	    // 32
		
		if (is_water_biome(terr.biome)) {
			int base;
			switch (terr.biome) {
				case Biome::Ocean: 
					base = 0;
					break;
				case Biome::SeaLane: 
					base = 50;
					break;
				default:
					assert(0); // unknown water terr type
			}
			
			render_sprite(win, pix + v2i(0,0), res(COAST, 
				base + get_coast_index(0, get(loc,Dir::A), get(loc,Dir::Q), get(loc,Dir::W))
			));
			
			render_sprite(win, pix + v2i(8,0), res(COAST, 
				base + get_coast_index(1, get(loc,Dir::W), get(loc,Dir::E), get(loc,Dir::D))
			));
			
			render_sprite(win, pix + v2i(8,8), res(COAST, 
				base + get_coast_index(2, get(loc,Dir::D), get(loc,Dir::C), get(loc,Dir::X))
			));
			
			render_sprite(win, pix + v2i(0,8), res(COAST, 
				base + get_coast_index(3, get(loc,Dir::X), get(loc,Dir::Z), get(loc,Dir::A))
			));
			
		}
		
		
		// phys feats & improvments
		if (terr.has(Phys::Mountain)) {
			render_sprite(win, pix, res(PHYS, 33 + get_wxad_index(loc, Phys::Mountain)));
		}
				
		if (terr.has(Phys::Hill)) {			
			render_sprite(win, pix, res(PHYS, 49 + get_wxad_index(loc, Phys::Hill)));
		}
		
		if (terr.has(Phys::Plow)) {
			render_sprite(win, pix, res(PHYS, 150));
		}
		
		if (terr.has(Phys::Forest)) {			
			render_sprite(win, pix, res(PHYS, 65 + get_wxad_index(loc, Phys::Forest)));
		}
		
		if (terr.has(Phys::MajorRiver)) {
			if (!is_water_biome(terr.biome)) {
				auto ind = get_wxad_index(loc, Phys::MajorRiver|Phys::MinorRiver);
				if (ind) {
					render_sprite(win, pix, res(PHYS, 1 + ind));
				}
				else {
					render_sprite(win, pix, res(PHYS, 16));
				}
			}
			else {
				for (int i = 0; i < 8; i += 2) {
					if ( get(loc, CLOCKWISE_DIRS[i]).has(Phys::MajorRiver) ) {
						render_sprite(win, pix, res(PHYS, 141 + (i >> 1)));
					}
				}
			}
			
		}
		else if (terr.has(Phys::MinorRiver)) {
			if (!is_water_biome(terr.biome)) {
				auto ind = get_wxad_index(loc, Phys::MajorRiver|Phys::MinorRiver);
				if (ind) {
					render_sprite(win, pix, res(PHYS, 17 + ind));
				}
				else {
					render_sprite(win, pix, res(PHYS, 32));
				}
			}
			else {
				for (int i = 0; i < 8; i += 2) {
					if ( get(loc, CLOCKWISE_DIRS[i]).has(Phys::MinorRiver) ) {
						render_sprite(win, pix, res(PHYS, 145 + (i >> 1)));
					}
				}
			}			
		}
					
		if (terr.has(Phys::Road)) {
			bool r = false;			
			for (int i=0; i<8; ++i) {
				if ( get(loc, CLOCKWISE_DIRS[i]).has(Phys::Road) ) {
					render_sprite(win, pix, res(PHYS, 82 + i));
					r = true;
				}
			}			
			if (!r) {
				render_sprite(win, pix, res(PHYS, 81));				
			}
		}
		
	}

	void render_unit(sf::RenderWindow &win, 
			Coords const& pos, 
			Env const& env,
			Terr const& terr,
			Vector2<int> const& map_pix)
	{
		
		int x = pos[0] * ly.TERR_W + map_pix[0];
		int y = pos[1] * ly.TERR_H + map_pix[1];
		auto pix = v2i(x,y);
		
		// colony icon
		if (terr.has_colony()) {
			// render colony
			render_sprite(win, pix[0], pix[1], res(ICON, 4));
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
				// render unit in field
				render_shield(win, pix[0], pix[1], player.get_color());
				render_sprite(win, pix[0], pix[1], res(ICON, unit.get_type_id()));
			}
		}
	}
	
	void render_map(sf::RenderWindow &win, Env const& env, Console const& con, Vector2<int> const& pos) 
	{
		auto w = env.w;
		auto h = env.h;

		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				render_terr(win, Coords(i, j), env, env.get_terr(Coords(i,j)), pos);
			}
		}

		
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				render_unit(win, Coords(i, j), env, env.get_terr(Coords(i,j)), pos);
			}
		}
		
		//for (auto &p: env.units) {
		//	render_icon(win, env, p.second);
		//}
		
		render_cursor(win, con.sel[0]*16 + pos[0], con.sel[1]*16 + pos[1]);

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
	
	void render_rect(sf::RenderWindow &win, 
			Vector2<int> const& pos, Vector2<int> const& dim, 
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
	
	
	void render_bar(sf::RenderWindow &win, 
			Env const& env,
			Console const& con,
			Vector2<int> const& pos, 
			Vector2<int> const& dim
		) {
		// pos - left top pix
		// dim - size
		
		// pos = {0,0}
		// dim = {320,7}
				
		render_area(win, res("COLONIZE/WOODTILE_SS", 1), pos, dim);
		
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
			Console const& con,
			Vector2<int> const& pos, 
			Vector2<int> const& dim
		) {
		
		//sf::Image img = res("COLONIZE/WOODTILE_SS", 1);  32x24
		
		render_area(win, res("COLONIZE/WOODTILE_SS", 1), pos, dim);
		
		// terrain info
		/*
		 * Biome: Plains 
		 * road
		 * 
		 * 
		 */
		
		
		
		if (env.in_bounds(con.sel)) {
			
			auto& t = env.get_terr(con.sel);

			
			string phys_info;
			for (auto const& item: PHYS_NAMES) {
				auto const& phys = item.first;
				auto const& name = item.second;
				
				if (t.has(phys)) phys_info += name + ",";
			}
			
			auto info = boost::str(
				format("Biome: %||\n[%||]\n") % 
					BIOME_NAMES.at((t.biome)) % phys_info
			);
			


			if (t.units.size()) {
				Unit const& u = env.get_defender(t);

				info += boost::str(
					format("Type: %||\nTime Left: %||/%||") % 
						u.get_name() % int(u.time_left) % int(TIME_UNIT)
				);

			}
			
			
			render_text(win, 
				pos,
				res_pixfont("tiny.png"),
				info,
				{0,0,0,0},
				0
			);
		}
		
		
		
		

				

		
		/*
		i = d/w;
		j = dy/h;
		
		sf::Sprite s;	
		s.SetImage();

		s.
		
		
		render_sprite(app, 320-80, 0, );
		
		
		s.SetPosition(x + rr.Left ,y + rr.Top + 6);
		s.SetSubRect(sf::IntRect(
			(r.Left * img.GetWidth()) +0.5, 
			(r.Top * img.GetHeight()) +0.5, 
			(r.Right * img.GetWidth()) +0.5, 
			(r.Bottom * img.GetHeight()) +0.5
		));
		win.Draw(s);
		*/
	}

	v2i render_text(
			sf::RenderWindow &win, 
			v2i const& pos,
			PixFont const& font,
			string const& text,
			sf::Color const& bgcol,
			int width
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

		y += 1; // horizontal separation
		for (auto c: text) {
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
		if (!con.active) return;
		
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
		
		
		render_text(
			app, 
			ly.scr.pos,
			res_pixfont("tiny.png"),
			con.buffer + "_",
			{0,0,0,0},
			0
		);
		
	}
			
	
	
	
	void render(sf::RenderWindow &app, const col::Env &env, const col::Console &con) {
		app.clear();
		
		int w = app.getSize().x/3;
		int h = app.getSize().y/3;
		
		//cout << format("%||,%||\n") % w % h;
		
		if (con.mode == Console::Mode::COLONY) {
			render_city(app, env, con);
		}
		else {
			render_map(app, env, con, {0,8});
		}

		//if (env.curr_player) {
		//	render_playerind(app, 0, 0, env.curr_player->color);
		//}

		// BACKGROUND
		
		// bar top 
		render_bar(app, env, con, {0,0}, {w,7});		
		// hline under the bar
		render_rect(app, {0,7}, {w,1}, {0,0,0,255});
		
		
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

		
		
		
		render_cmd(app, con);
		
		
		app.display();
		
	}
	

	
}