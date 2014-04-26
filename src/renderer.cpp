#include "renderer.h"

namespace col {
	

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
	
	
	int16 render_text(
		sf::RenderWindow &win, 
		Vector2<int> const& pos,
		PixFont const& font,
		string const& text,
		int width = 0
	);
	
	
	
	string const
		TERR = "COLONIZE/TERRAIN_SS",
		ICON = "COLONIZE/ICONS_SS",
		PHYS = "COLONIZE/PHYS0_SS",
		ARCH = "COLONIZE/PARCH_SS",
		BUILDING = "COLONIZE/BUILDING_SS";

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

	
	
	void render_icon(sf::RenderWindow &win, Env const& env, Unit const& unit) {
		auto &p = env.get_player(unit.player->id);
		
		auto pos = env.get_coords(unit);
		
		auto type = unit.type->id;
		switch (type / 256) {
			case 0: 
				render_shield(win, pos[0]*16, pos[1]*16, p.color);
				render_sprite(win, pos[0]*16, pos[1]*16, res(ICON, type));
				break;
			case 1: 
				render_sprite(win, pos[0]*16, pos[1]*16, res(ICON, 4));
				// flag:
				render_sprite(win, pos[0]*16+5, pos[1]*16, res(ICON, unit.player->flag_id));
				break;
		}
		
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
					render_sprite(win, pos[0], pos[1], res(BUILDING, type % 255));
					break;
				default:
					render_sprite(win, pos[0], pos[1], res(BUILDING, -1));
					break;
			}
			
			
			cout << unit << endl;
			++i;
		}
		
		for (; i<15; ++i) {		
			auto & pos = coords[i];
			render_sprite(win, pos[0], pos[1], res(BUILDING, trees[i]));
		}
		
		
		
	}
	
	void render_tile(sf::RenderWindow &win, 
			Coord const& i, Coord const& j, 
			Terr const& tile,
			Vector2<int> const& pos)
	{
		
		auto biome = tile.biome;
		auto forest = tile.has(Phys::Forest);
		auto hill = tile.has(Phys::Hill);
		auto mountain = tile.has(Phys::Mountain);
		
		int x = i*16+pos[0];
		int y = j*16+pos[1];
		 
		 
		render_sprite(win, x, y, res(TERR, enum_value(biome)));
		if (forest) {
			render_sprite(win, x, y, res(PHYS, 65));
		}
		if (hill) {
			render_sprite(win, x, y, res(PHYS, 49));
		}
		if (mountain) {
			render_sprite(win, x, y, res(PHYS, 33));
		}
		
		
		/*if (forest) {
			//cout << uint16(tile) << endl;
			//cout << uint16(terrain) << endl;
			//cout << uint16(forest) << endl;
			//cout << endl;
			render_sprite(win, i*16, j*16, res(PHYS, 65));
		}*/
	}
	
	void render_map(sf::RenderWindow &win, Env const& env, Console const& con, Vector2<int> const& pos) 
	{
		auto w = env.w;
		auto h = env.h;

		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				render_tile(win, i, j, env.get_terr(Coords(i,j)), pos);
			}
		}

		for (auto &p: env.units) {
			render_icon(win, env, p.second);
		}
		
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

	int16 render_text(
			sf::RenderWindow &win, 
			Vector2<int> const& pos,
			PixFont const& font,
			string const& text,
			int width
		) {

		// width

		auto x = pos[0];
		auto y = pos[1];

		auto &img = font.tex;

		sf::Sprite s;	
		s.setTexture(img);

		//auto line_height = font.GetCharacterSize();
		//cout << "char hh:" << hh << endl;

		for (auto c: text) {
			auto &g = font.glyphs.at(c);
			auto &rr = g.rect;

			//cout << uint16(c) << endl;			
			// cout << format("c=%1% x,y=%2%,%3%\n") % c % x % y;
			// cout << format("%1%;%2%;%3%;%4%\n") % r.Left % r.Top % r.Right % r.Bottom;

			s.setPosition(x ,y);
			s.setTextureRect(g.rect);
			win.draw(s);

			x += rr.width + 1;
		}

		return x;
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
	 * */
	
	void render_cmd(sf::RenderWindow &app, const col::Console &con) {
		
		
		auto ln = con.output.size();
		for (int i = 0; i < ln; ++i) {
			render_text(
				app, 
				Vector2<int>(1, 192 - (ln-i)*8),
				res_pixfont("tiny.png"),
				con.output[i],
				0
			);			
		}
		
		
		render_text(
			app, 
			Vector2<int>(1,192),
			res_pixfont("tiny.png"),
			con.buffer,
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

		// 79 x h-8
		render_panel(app, env, con,			
			Vector2<int>(w-79,8),
			Vector2<int>(79,h-8)
		);
		
		// vline
		render_rect(app, {w-80,8}, {1,h-8}, {0,0,0,255});

		
		render_cmd(app, con);
		
		
		render_bar(app, env, con, {0,0}, {w,7});
		
		// hline
		render_rect(app, {0,7}, {w,1}, {0,0,0,255});
		
		app.display();
		
	}
	

	
}