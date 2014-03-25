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
		Vector2<int16> const& pos,
		PixFont const& font,
		string const& text,
		int16 width = 0
	);
	
	
	
	const string 
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
			int16 const& outline_thickness,
			sf::Color const& outline_color
	) {
		sf::RectangleShape rectangle;
		rectangle.setPosition(pos[0], pos[1]);
		rectangle.setSize(sf::Vector2f(dim[0], dim[1]));
		rectangle.setFillColor(fill_color);
		rectangle.setOutlineColor(outline_color);
		rectangle.setOutlineThickness(outline_thickness);		
		return rectangle;
	}
	
	
	void render_shield(sf::RenderWindow &win, uint16 x, uint16 y, const Color &col) {
		win.draw(
			RectShape(
				Vector2<int16>(x+1, y+1), 
				Vector2<int16>(6, 8), 
				sf::Color(col.r,col.g,col.b, 255),
				1, 
				sf::Color(0,0,0, 255)
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
	
	void render_sprite(sf::RenderWindow &win, uint16 x, uint16 y, const sf::Texture &img) {
		sf::Sprite s;
		s.setPosition(x, y);
		s.setTexture(img); 
		win.draw(s);
	}

	
	
	void render_icon(sf::RenderWindow &win, const Env &env, const Icon &icon) {
		auto &p = env.get_player(icon.player->id);
		
		auto type = icon.type->id;
		switch (type / 256) {
			case 0: 
				render_shield(win, icon.pos[0]*16, icon.pos[1]*16, p.color);
				render_sprite(win, icon.pos[0]*16, icon.pos[1]*16, res(ICON, type));
				break;
			case 1: 
				render_sprite(win, icon.pos[0]*16, icon.pos[1]*16, res(ICON, 4));
				// flag:
				render_sprite(win, icon.pos[0]*16+5, icon.pos[1]*16, res(ICON, icon.player->flag_id));
				break;
		}
		
	}
	

	void render_city(sf::RenderWindow &win, const Env &env, const Console &con) {
		
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
		for (auto &iicon: env.icons) {
			auto & icon = iicon.second;
			if (icon.pos != con.sel) {
				continue;
			}
			
			auto type = icon.type->id;
			if (type < 256 || icon.pos != con.sel) {
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
			
			
			cout << icon << endl;
			++i;
		}
		
		for (; i<15; ++i) {		
			auto & pos = coords[i];
			render_sprite(win, pos[0], pos[1], res(BUILDING, trees[i]));
		}
		
		
		
	}
	
	void render_tile(sf::RenderWindow &win, 
			Coord const& i, Coord const& j, 
			Terr const& tile) 
	{
		
		auto terrain = tile & 0x0F;
		auto forest = bool(tile & (1 << 4));
		
		
		render_sprite(win, i*16, j*16, res(TERR, terrain));
		if (forest) {
			//cout << uint16(tile) << endl;
			//cout << uint16(terrain) << endl;
			//cout << uint16(forest) << endl;
			//cout << endl;
			render_sprite(win, i*16, j*16, res(PHYS, 65));
		}
	}
	
	void render_map(sf::RenderWindow &win, const Env &env, const Console &con) 
	{
		auto w = env.w;
		auto h = env.h;

		for (uint j = 0; j < h; ++j) {
			for (uint i = 0; i < w; ++i) {
				render_tile(win, i, j, env.get_terr_id(i,j));
			}
		}

		for (auto &p: env.icons) {
			render_icon(win, env, p.second);
		}
		
		render_cursor(win, con.sel[0]*16, con.sel[1]*16);

	}
	
	
	
	



	void render_panel(sf::RenderWindow &win, 
			Env const& env,
			Console const& con,
			Vector2<int16> const& pos, 
			Vector2<int16> const& dim
		) {
		
		//sf::Image img = res("COLONIZE/WOODTILE_SS", 1);
		
		
		
		//get_icon_at
		sf::RectangleShape rectangle;
		rectangle.setSize(sf::Vector2f(dim[0], dim[1]));
		rectangle.setFillColor(sf::Color(50,0,0, 255));
		rectangle.setOutlineColor(sf::Color(0,50,0, 255));
		rectangle.setOutlineThickness(1);
		rectangle.setPosition(pos[0], pos[1]);
		
		win.draw(rectangle);
		
		

		
		Icon const* u = env.get_icon_at(con.sel);
		
		if (u) {
			
			auto &ut = *(u->type);
			auto moves = (ut.movement - u->movement_used) / UNIT_MOVEMENT;
			
			auto s = str(format("%||\nmoves: %||") % ut.name % uint16(moves));
			
			render_text(win, 
				pos,
				res_pixfont("tiny.png"),
				s,
				dim[0]
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

	int16 render_text(
			sf::RenderWindow &win, 
			Vector2<int16> const& pos,
			PixFont const& font,
			string const& text,
			int16 width
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
				Vector2<int16>(1, 192 - (ln-i)*8),
				res_pixfont("tiny.png"),
				con.output[i],
				0
			);			
		}
		
		
		render_text(
			app, 
			Vector2<int16>(1,192),
			res_pixfont("tiny.png"),
			con.buffer,
			0
		);
	}
			
	
	
	
	void render(sf::RenderWindow &app, const col::Env &env, const col::Console &con) {
		app.clear();
		
		
		if (con.mode == Console::Mode::COLONY) {
			render_city(app, env, con);
		}
		else {
			render_map(app, env, con);
		}

		render_playerind(app, 0, 0, env.curr_player->color);

			
		render_panel(app, env, con,			
			Vector2<int16>(240,0),
			Vector2<int16>(80,200)
		);

		
		render_cmd(app, con);
		app.display();
		
	}
	

	
}