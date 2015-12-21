#include "renderer.h"

//#include <functional>
#include <boost/range/adaptor/reversed.hpp>

#include "logic.h"
#include "props.h"
#include "conf.h"
#include "geo2D.h"
#include "textrend.h"


//namespace Key = backend::Key;

namespace col {

	using namespace logic;



	using Event = Console::Event;
	using Button = Console::Button;
	using Key = backend::Key;
	using Dev = Console::Dev;


	bool NEW_STYLE = false;




	Layout const ly(conf.screen_w, conf.screen_h);


	using Res = map<pair<string,uint>, backend::Texture>;
	Res g_res;



	
	// Texture get dim
	using backend::get_dim;



	backend::PixFont FontTiny;
	




	int get_icon_id(Unit const& u) {
		return u.get_icon();
	}


	inline
	bool is_water(Terr const& t) {
		return t.is_water_tile();
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
	unsigned load_png(backend::Back & back, backend::Texture & tex, Path const& path) {
		back.load(tex, path);
		return 0;
	}


	Path get_res_path(string const& category, uint const& index) {
		return conf.tile_path / category / Path(format("%|03|.png", index));
	}

	// Load resource file
	// cat -- category (dir name)
	// i -- frame num (file name)
	void load_res(backend::Back & back, Res::mapped_type & tex, string const& cat, uint const& i) {

		auto path = get_res_path(cat, i);

		if (auto err = load_png(back, tex, path)) {
			print("WARNING: cannot load image file; path=%||; err=%||;\n",
				path.c_str(), lodepng_error_text(err)
			);

			// try default
			auto def_path = get_res_path(ICON, 65);
			if (auto err = load_png(back, tex, def_path)) {
				throw Error("cannot load fallback image file; path=%||; err=%||;\n",
					def_path.c_str(),
					lodepng_error_text(err)
				);
			}
		}

		// tex.setSmooth(false);
	}

	// Get resource from global cache (load on demand)
	Res::mapped_type const& res(backend::Back & back, string const& cat, uint const& i = 1) {
		auto key = Res::key_type(cat, i);
		auto p = g_res.find(key);
		if (p != g_res.end()) {
			return (*p).second;
		}
		else {
			auto &img = g_res[key];
			load_res(back, img, cat, i);
			return img;
		}
	}




	backend::Color const ColorSelectedBG = backend::Color(0x3c,0x20,0x18,0xff);

	backend::Color const ColorNone = backend::Color(0,0,0,0);

	backend::Color const ColorFontSelected = backend::Color(0xc7,0xa2,0x20,0xff);
	backend::Color const ColorFont = backend::Color(0x55,0x96,0x34,0xff);

	backend::Color const ColorBlack = backend::Color(0,0,0,255);
	backend::Color const ColorGreen = backend::Color(0,255,0,255);
	backend::Color const ColorBlue = backend::Color(0,0,255,255);
	backend::Color const ColorRed = backend::Color(255,0,0,255);
	backend::Color const ColorDarkGreen = backend::Color(0,127,0,255);
	backend::Color const ColorDarkBlue = backend::Color(0,0,127,255);
	backend::Color const ColorDarkRed = backend::Color(127,0,0,255);
	backend::Color const ColorYellow = backend::Color(255,255,0,255);
	backend::Color const ColorWhite = backend::Color(255,255,255,255);
	backend::Color const ColorGray = backend::Color(128,128,128,128);

	backend::Color const ColorDarkSkyBlue = backend::Color(76,100,172,255);
	backend::Color const ColorSkyBlue = backend::Color(104,136,192,255);
	backend::Color const ColorLightSkyBlue = backend::Color(156,184,220,255);


	backend::Color const ColorDarkBrown = backend::Color(60,32,24);
	backend::Color const ColorLightBrown = backend::Color(121,73,52);

	backend::Color const ColorDarkWoodBrown = backend::Color(84,68,44,255);
	backend::Color const ColorWoodBrown = backend::Color(132,112,80,255);
	backend::Color const ColorLightWoodBrown = backend::Color(152,128,92,255);







	



	void preload(Path const& d, uint const& i, Res::mapped_type && tex) {
		auto key = Res::key_type(d,i);
		g_res[key] = std::move(tex);
	}

	backend::Surface replace_black(backend::Surface const& inn, backend::Surface const& tex, v2i const& toff) {
		/* Create new surface

		mask colors meaning:
		 black -> take from tex
		 else -> take from mask

		 toff -- tex offset vector

		*/

		auto dim = inn.get_dim();
		auto out = backend::Surface(dim);

		for (int j = 0; j < dim[1]; ++j) {
			for (int i = 0; i < dim[0]; ++i) {
				auto c = inn.get_pixel({i,j});
				auto pos = v2i(i,j);

				//print("c = %||\n", c);

				if (c.a == 0) {
					out.set_pixel(pos, backend::Color(0,0,0,0));
				}
				else if (c == backend::Color(0,0,0,255)) {

					out.set_pixel(pos, tex.get_pixel(toff + pos));
				}
				else {
					out.set_pixel(pos, inn.get_pixel(pos));
				}

			}
		}

		return out;
	}

	//void render_fields(backend::Back &win, Env const& env, Console const& con,
	//	v2i const& pix, Coords const& pos);


	v2i get_loffset(int l) {
		switch(l){
			case 0: return v2i(0,0);
			case 1: return v2i(8,0);
			case 2: return v2i(8,8);
			case 3: return v2i(0,8);
		}
		throw Error("invalid 'l'");
	}

	using ResInd = std::pair<string, uint>;

	backend::Texture make_combined_texture(
		backend::Back & back,
		ResInd p,
		ResInd b,
		v2i const& off
	) {
		auto p_img = back.make_surface_from_png(
			get_res_path(p.first, p.second)
		);

		auto b_img = back.make_surface_from_png(
			get_res_path(b.first, b.second)
		);

		auto surf = replace_black(p_img, b_img, off);

		return back.make_texture(surf);
	}



	void preload_coast(backend::Back & back, Biome bio, int start) {
		for (int k = 0; k < 8; ++k) {
			for (int l = 0; l < 4; ++l) {
				int ind = (k<<2) + l;

				preload(COAST, start + ind,
					make_combined_texture(back,
						{PHYS, 109+ind},
						{TERR, bio},
						get_loffset(l)
					)
				);
			}
		}
	}

	int const TextureOceanId = 11;
	int const TextureSeaLaneId = 11;

	void preload_coast(backend::Back & back) {
		// 109 - 140

		preload_coast(back, TextureOceanId, 0);
		preload_coast(back, TextureSeaLaneId, 50);
	}




	void preload_terrain(backend::Back & back) {
		for (int i=1; i<13; ++i) {
			preload(DIFFUSE,  0+i,
				make_combined_texture(back, {PHYS, 105}, {TERR, i}, {0,0})
			);
			preload(DIFFUSE, 50+i,
				make_combined_texture(back, {PHYS, 106}, {TERR, i}, {0,0})
			);
			preload(DIFFUSE, 100+i,
				make_combined_texture(back, {PHYS, 107}, {TERR, i}, {0,0})
			);
			preload(DIFFUSE, 150+i,
				make_combined_texture(back, {PHYS, 108}, {TERR, i}, {0,0})
			);
		}

		preload_coast(back);
	}


	
	backend::PixFont::ColorIndex const
		ColIndNone = 0,
		ColIndFontSelected = 1,
		ColIndBlack = 2,
		ColIndWhite = 3,
		ColIndYellow = 4,
		ColIndGreen = 5,
		ColIndRed = 6,
		ColIndGray = 7,
		ColIndFont = 8;

	
	void load_pixfont(backend::Back & back, backend::PixFont & font, Path const& path) {
		print("load font; path=%||;\n", path);
		font.load(back, path.c_str(), 0);
		
		font.colorize(back, ColIndFont, ColorFont);
		font.colorize(back, ColIndFontSelected, ColorFontSelected);
		font.colorize(back, ColIndBlack, ColorBlack);
		font.colorize(back, ColIndWhite, ColorWhite);
		font.colorize(back, ColIndYellow, ColorYellow);
		font.colorize(back, ColIndGreen, ColorGreen);
		font.colorize(back, ColIndRed, ColorRed);
		font.colorize(back, ColIndGray, ColorGray);
		font.colorize(back, ColIndNone, ColorNone);
		font.colorize(back, 9, ColorRed);
		font.colorize(back, 10, ColorRed);
		font.colorize(back, 11, ColorRed);
	}
		

	void preload_renderer(backend::Back & back) {
		preload_terrain(back);
		load_pixfont(back, FontTiny, conf.font_tiny_path);
	}


	void render_fill(backend::Back & win,
			v2i const& pos,
			v2i const& dim,
			backend::Color const& col
	) {
		win.render_fill(pos, dim, col);
	}

	void render_inline(backend::Back &win, v2i const& pos, v2i const& dim, backend::Color const& c, int t) {
		/* Draw inside border of pos,dim box; t -- thickness
		*/
		int w,h;
		(dim - v2i(t,t)).unpack(w,h);


		// top
		render_fill(win, pos + v2i{0,0}, {w,t}, c);
		// right
		render_fill(win, pos + v2i{w,0}, {t,h}, c);
		// bottom
		render_fill(win, pos + v2i{t,h}, {w,t}, c);
		// left
		render_fill(win, pos + v2i{0,t}, {t,h}, c);
	}

	void render_outline(backend::Back &win, v2i const& pos, v2i const& dim, backend::Color const& c, int t) {
		/* Draw 1px thick outline border of pos,dim box
		*/
		render_inline(win, pos - v2i(t,t), dim + v2i(t,t)*2, c, t);
	}



	/*struct RenderText{
		backend::Back &win;

		v2i pos{0,0}; v2i dim{0,0};
		v2f align{0.5f, 0.5f};

		string font{"tiny.png"};
		backend::Color fg{ColorWhite}, bg{ColorNone};

		string text{""};

		RenderText(
			backend::Back &win,
			v2i pos, v2i dim,
			v2f align,
			string font,
			backend::Color fg, backend::Color bg,
			string text
		):
			win(win), pos(pos), dim(dim), align(align),
			font(font), fg(fg), bg(bg), text(text)
		{}

		void operator()() {
			render_text(win,pos,dim,align,font,fg,bg,text);
		}
	};*/

	void render_terr(backend::Back &win,
			v2i const& pos,
			Env const& env,
			Terr const& terr);



	void render_shield(backend::Back &win, v2i const& pos, backend::Color const& color, char letter) {

		auto inner_pos = pos + v2i(1,1) * ly.line;
		auto inner_dim = v2i(ly.S(5), ly.font_tiny);

		render_outline(win,
			inner_pos,
			inner_dim,
			{0,0,0, 255},
			ly.line
		);

		render_fill(win,
			inner_pos,
			inner_dim,
			color
		);

		render_text_at(win,
			pos + v2i(1,1) * ly.line,
			FontTiny, ColIndBlack, ColorNone,
			string() + letter
		);
	}

	void render_unit(backend::Back &win,
			Console & con,
			Coords const& pos,
			Env const& env,
			Terr const& terr,
			Vector2<int> const& map_pix,
			Coords const& delta);


	void render_pixel(backend::Back &win, v2i pix, const backend::Color &colr) {
		render_fill(win,
			pix,
			v2i(1, 1),
			colr
		);
	}





	void render_sprite(backend::Back & win, int x, int y, backend::Texture const& img) {
		win.render(img, {x,y});
	}

	void render_sprite(backend::Back & win, v2i const& pix, backend::Texture const& img) {
		win.render(img, pix);
	}

	void render_sprite(backend::Back & win, 
		v2i const& pos, v2i const& dim, v2f const& align,
		backend::Texture const& img
	) {
		win.render(img, 
			calc_align(pos, dim, align, get_dim(img))
		);
	}


	void render_terr(backend::Back &win,
		Coords const& pos,
		Env const& env,
		Terr const& terr,
		Vector2<int> const& map_pix,
		Coords const& delta
	);


	void render_area(
		backend::Back & win,
		v2i const& area_pos,
		v2i const& area_dim,
		backend::Texture const& tex
	);


	void render_sprite(backend::Back & win,
		v2i const& pix, v2i const& dim,
		backend::Texture const& img
	) {
		v2i tex_dim = get_dim(img);

		auto free = dim - tex_dim;
		v2i voffset(free[0]/2, free[1]/2);

		render_sprite(win, pix + voffset, img);
	}

	void render_sprite2(backend::Back & win,
		v2i const& pix, v2i const& dim,
		backend::Texture const& img
	) {
		render_sprite(win, pix, img);
	}

	using Texture = Res::mapped_type;

	struct Renderer{
		backend::Back &win;
		Env const& env;

		Renderer(backend::Back &win, Env const& env): win(win), env(env) {}

		void operator()(Res::mapped_type const& res, v2i const& pos);
		void operator()(Res::mapped_type const& res, v2i const& pos, v2i const& dim);
		void operator()(Unit const& unit, v2i const& pos);
		void operator()(Unit const& unit, v2i const& pos, v2i const& dim);
		void operator()(Item const& item, v2i const& pos);

		void fill(Texture const& tex, v2i const& pos, v2i const& dim);
		void fill(backend::Color const& color, v2i const& pos, v2i const& dim);

	};

	void Renderer::operator()(Res::mapped_type const& res, v2i const& pos) {
		render_sprite(win, pos, res);
	}

	void Renderer::operator()(Res::mapped_type const& res, v2i const& pos, v2i const& dim) {
		render_sprite(win, pos, dim, res);
	}

	void Renderer::operator()(Unit const& unit, v2i const& pos, v2i const& dim) {
		render_sprite(win, pos, dim, res(win, ICON, unit.get_type_id()));
	}

	void Renderer::operator()(Unit const& unit, v2i const& pos) {
		render_sprite(win, pos, res(win, ICON, unit.get_type_id()));
	}

	void Renderer::operator()(Item const& item, v2i const& pos) {
		render_sprite(win, pos, res(win, ICON, item));
	}



	void render_area(backend::Back & win, v2i const& pos, v2i const& dim, backend::Color const& color) {
		render_fill(win, pos, dim, color);
	}


	inline backend::Color get_nation_color(Nation const& n) {
		auto c = n.color;
		return {c.r,c.g,c.b,255};
	}


	void render_area(
		backend::Back & win,
		v2i const& area_pos,
		v2i const& area_dim,
		backend::Texture const& tex
	) {
		auto tile_dim = tex.get_dim();
		auto area_end = area_pos + area_dim;

		int ei,ej;

		// center
		{
			int i,j;
			j = area_pos[1];
			while (j < area_end[1] - tile_dim[1]) {
				i = area_pos[0];
				while (i < area_end[0] - tile_dim[0]) {

					win.render(tex, {i,j});

					i += tile_dim[0];
				}
				j += tile_dim[1];
			}

			ei = i;
			ej = j;
		}


		// bottom
		{
			auto src_box = backend::Box({0, 0}, {tile_dim[0], area_end[1] - ej});

			int i = area_pos[0], j = ej;
			while (i < area_end[0] - tile_dim[0]) {
				win.render_clip(tex, {i, j}, src_box);
				i += tile_dim[0];
			}
			ei = i;
		}

		// right
		{
			auto src_box = backend::Box({0, 0}, {area_end[0] - ei, tile_dim[1]});
			int i = ei, j = area_pos[1];
			while (j < area_end[1] - tile_dim[1]) {
				win.render_clip(tex, {i, j}, src_box);
				j += tile_dim[1];
			}
			ej = j;
		}

		// corner
		{
			auto src_box = backend::Box({0, 0}, {area_end[0] - ei, area_end[1] - ej});
			int i = ei, j = ej;
			win.render_clip(tex, {i, j}, src_box);
		}

	}

	//backend::Color Black = {0,0,0,255};
	//backend::Color Transparent = {0,0,0,0};


	/*
	static Res::mapped_type const* g_cursor_tex = nullptr;
	void set_cursor(Res::mapped_type const* tex = nullptr) {

		g_cursor_tex = tex;
	}*/

	struct Col {
		string name{""};
		int width{0};
		float align{0};

		Col(string n, int w, float a):
			name(n), width(w), align(a) {}
		Col() = default;
	};

	using Row = vector<string>;

	int get_elems_stretch(int nelem, int elem_space, int sep) {
		return nelem * elem_space + (nelem + 2) * sep;
	}

	template<class Key>
	void render_select_f(backend::Back & win, Console & con,
		v2i const& g_pos,
		v2i const& g_dim,
		v2f const& align,

		vector<Col> const& cols,
		vector<Key> const& keys,
		vector<vector<string>> const& rows,

		Key & selected,
		std::function<void()> onselect,
		std::function<void()> oncancel
	) {

		auto line_height = ly.font_tiny;
		auto sep = v2i(ly.S(1), ly.S(1));   // cell sep

		int width = 0;
		for (auto& col: cols) {
			width += col.width;
		}
		width += get_elems_stretch(cols.size(), 0, sep[0]);

		int height = get_elems_stretch(rows.size(), line_height, sep[1]);


		v2i dim = v2i(width, height);
		auto pos = calc_align(Box(g_pos, g_dim), dim, align);


		// click anywhere -- cancel
		con.on(Event::Press, oncancel);

		// background & outline border
		render_area(win, pos, dim, res(win, "WOODTILE_SS", 1));
		render_outline(win, pos, dim, ColorDarkBrown, ly.line);

		// click on dialog -- do nothing
		con.on(Event::Press, pos, dim, [](){});


		auto row_dim = v2i(dim[0] - sep[0]*2, line_height);
		auto row_pos = pos + sep;

		for (size_t j = 0; j < rows.size(); ++j) {
			auto & row = rows[j];
			auto & key = keys[j];

			auto cpos = row_pos;

			// selected bg and font fg
			auto fgind = ColIndFont;
			if (key == selected) {
				render_fill(win,
					row_pos, row_dim, ColorSelectedBG
				);
				fgind = ColIndFontSelected;
			}

			// left click on row -- select
			con.on(Event::Press, Button::Left, row_pos, row_dim,
				[&selected,key,onselect](){ selected = key; onselect(); }
			);

			// render row by cell
			for (size_t i = 0; i < cols.size(); ++i) {
				auto & col = cols[i];
				auto & cell = row[i];

				auto cdim = v2i(col.width, line_height);

				render_text(win,
					cpos, cdim, v2f(col.align, 0.5),
					FontTiny, fgind, ColorNone,
					cell
				);

				cpos[0] += col.width + sep[0];
			}

			row_pos[1] += line_height + sep[1];
		}

	}

	template<typename K>
	void render_select(backend::Back & win, Console & con,
		std::function<v2i(v2i const& dim)> cpos,
		//Box const& par, v2f const& align,
		vector<pair<K, string>> const& kvs,
		K & selected,
		std::function<void()> onselect,
		std::function<void()> oncancel
	) {

		int const LINE_HEIGHT = ly.font_tiny;

		v2i const dim = v2i(ly.scr.dim[0] * 0.5, (kvs.size()+1) * LINE_HEIGHT);
		v2i pos = cpos(dim);

		// click anywhere -- cancel
		con.on(Event::Press, oncancel);

		// background
		render_area(win, pos, dim, res(win, "WOODTILE_SS", 1));
		render_outline(win, pos, dim, ColorDarkBrown, ly.line);


		// click on dialog -- do nothing
		con.on(Event::Press, pos, dim, [](){});

		// entries
		for (size_t i = 0; i < kvs.size(); ++i) {

			auto p = pos + v2i(0, i * LINE_HEIGHT);
			auto d = v2i(dim[0], LINE_HEIGHT);

			auto& key = kvs[i].first;

			auto fg = ColIndFont;

			// selected bg
			if (key == selected) {
				render_fill(win,
					p, d,
					ColorSelectedBG
				);

				fg = ColIndFontSelected;
			}

			// label
			render_text(win,
				p, d, v2f(0.5, 0.5),
				FontTiny, fg, ColorNone,
				kvs[i].second
			);

			// left click on entry -- select
			con.on(Event::Press, Button::Left, p, d,
				[&selected,key,onselect](){ selected = key; onselect(); }
			);
		}

	}


	void render_unit_cargo(backend::Back &win, Console & con, 
		v2i const& pos, 
		v2i const& dim, 
		Unit const& unit
	) {
		// render unit cargo


		// render unit items
		auto cpos = pos;
		auto item_dim = v2i(ly.S(12), dim[1]);		
		auto text_dim = v2i(ly.S(4) * 3 + ly.S(2), dim[1]);
		auto tile_dim = v2i(item_dim[0] + text_dim[0], dim[1]);
		
		auto & box_tex = res(win, ICON, 123);
		
		// empty space
		{
			int num = unit.get_space_left();
			if (num) {
				
				// frame
				render_fill(win, cpos, tile_dim, ColorDarkWoodBrown);
				render_inline(win, cpos, tile_dim, ColorWoodBrown, ly.line);

				render_sprite(win,
					cpos, item_dim, v2f(0.5, 0.5),
					box_tex
				);

				render_text(win,
					cpos + v2i(item_dim[0], 0), text_dim, v2f(0, 0.5),
					FontTiny, ColIndWhite, ColorNone,
					to_string(num)				
				);

				cpos[0] += tile_dim[0] - ly.S(1);
			}
		}
		
		// items
		for (auto item: COLONY_ITEMS) {
			Amount num = unit.get(item);
			
			if (num) {
				// frame
				render_fill(win, cpos, tile_dim, ColorDarkWoodBrown);
				render_inline(win, cpos, tile_dim, ColorWoodBrown, ly.line);

				// left click on cargo -- start drag cargo unload
				con.on(Event::Press, Button::Left, cpos, tile_dim,
					[item,num,&con](){ con.drag_cargo(item, num, -1); }
				);

				render_sprite(win,
					cpos, item_dim, v2f(0.5, 0.5),
					box_tex
				);

				// item icon
				auto & item_tex = res(win, ICON, get_item_icon_id(item));
				
				render_sprite(win,
					cpos, item_dim, v2f(0.5, 0.5),
					item_tex
				);

				// amount
				render_text(win,
					cpos + v2i(item_dim[0], 0), text_dim, v2f(0, 0.5),
					FontTiny, ColIndWhite, ColorNone,
					to_string(num)				
				);

				cpos[0] += tile_dim[0] - ly.S(1);;
			}
		}

	

		// unit cargo area frame
		if (con.drag_item) {
			// left release -- drop cargo
			con.on(Event::Release, Button::Left, pos, dim,
				[&con](){ con.drop_cargo(+1); }
			);
		}

	} // render_unit_cargo



	void render_city_store(backend::Back & win, Console & con, v2i const& pos, v2i const& dim, Terr const& terr) {
		auto const& env = con.env; // get_render_env

		auto const& supp_reg = env.get_store(terr);

		Register prod_reg, cons_reg;
		fill_colony_regs(terr, prod_reg, cons_reg);

		// render storage area
		render_area(win, pos, dim, {76,100,172,255});

		if (con.drag_item) {
			// left release city store -- unload cargo
			con.on(Event::Release, Button::Left, pos, dim,
				[&con](){ con.drop_cargo(-1); }
			);
		}

		int tile_width = ly.S(16);
		int line_height = FontTiny.get_height();

		auto cpos = pos;
		auto cell_dim = v2i(tile_width, line_height);
		auto up = v2i(0, -line_height);
		for (auto item: COLONY_ITEMS) {

			auto supp_num = supp_reg.get(item);
			auto prod_num = prod_reg.get(item);
			auto cons_num = cons_reg.get(item);			
			auto delta = prod_num - cons_num;
			
			auto tile_pos = cpos;
			auto tile_dim = v2i(tile_width, ly.S(12));

			// render item
			render_sprite(win, tile_pos, tile_dim, res(win, ICON, get_item_icon_id(item)));

			// left click on item -- start drag cargo load
			con.on(Event::Press, Button::Left, tile_pos, tile_dim,
				[item,supp_num,&con](){ con.drag_cargo(item, supp_num, +1); }
			);

			// render supp num
			{
				auto pos = cpos + v2i(0, tile_dim[1]);
				auto dim = v2i(tile_width, line_height);
					
				auto proj = supp_num - cons_num + prod_num;
				if (proj < 0) {
					render_text(win,
						pos, cell_dim, v2f(0.5f, 0),
						FontTiny, ColIndYellow, ColorNone,
						format("%||", supp_num)
					);
				}
				else {
					render_text(win,
						pos, cell_dim, v2f(0.5f, 0),				
						FontTiny, ColIndWhite, ColorNone,
						std::to_string(supp_num)
					);
				}
			}
		
			// render prod num
			if (prod_num) {
				render_text(win,
					cpos + up*3, cell_dim, v2f(1, 0),
					FontTiny, ColIndWhite, ColorNone,
					format("+%||", prod_num)
				);
			}

			// render cons num
			if (cons_num) {
				render_text(win,
					cpos + up*2, cell_dim, v2f(1, 0),
					FontTiny, ColIndWhite, ColorNone,
					format("-%||", cons_num)
				);
			}
		
			// render delta
			if (delta > 0) {
				render_text(win,
					cpos + up, cell_dim, v2f(1, 0),
					FontTiny, ColIndGreen, ColorNone,
					format("+%||", delta)
				);
			}
			else if (delta < 0) {
				render_text(win,
					cpos + up, cell_dim, v2f(1, 0),
					FontTiny, ColIndYellow, ColorNone,
					format("%||", delta)
				);
			}
		
			
			cpos[0] += tile_width;
		}
	}



	void render_city(backend::Back &win, Env const& env, Console & con) {
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



		// render area
		auto & sand_tex = res(win, ARCH, 1);
		render_area(win, ly.city_builds.pos, ly.city_builds.dim, sand_tex);

		auto & pixs = ly.pixs;
		auto & dims = ly.dims;


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
				auto& build_tex = res(win, BUILD, b.get_type_id());
				auto build_pos = ly.city_builds.pos + pixs.at(i);
				auto build_dim = get_dim(build_tex);

				render_sprite(win, build_pos, build_tex);

				int nominal_prod = logic::get_nominal_prod(b, b.get_proditem());

				// production button under the building
				if (b.get_proditem() == ItemHammers)
				{

					auto button_pos = build_pos + v2i(0, build_dim[1]);
					auto button_dim = v2i(build_dim[0], ly.font_tiny);

					
					int blink = 0;
					string label, progress;
					if (b.task) {
						label = get_name(*b.task.what);
						label = label.substr(0,11);

						progress = format("%||/%||", b.task.get(), b.task.cap());
					}
					else {
						label = "(empty)";

						if ( nominal_prod and con.blink() ) {
							blink = 1;
						}
						progress = "";
					}
					
					if (blink) {
						render_text(win,
							button_pos, button_dim, {0.5,0.5},
							FontTiny, ColIndGray, ColorWhite,
							label
						);
					}
					else {
						render_text(win,
							button_pos, button_dim, {0.5,0.5},
							FontTiny, ColIndWhite, ColorNone,
							label
						);
					}

					render_outline(win,
						button_pos, button_dim, ColorWhite, ly.S(1)
					);

					// progress ind
					render_text(win,
						build_pos, build_dim, {1, 0},
						FontTiny, ColIndWhite, ColorBlack,
						progress
					);


					auto* makeable = b.task.what;

					// left click on button -- select production
					con.on(Event::Press, Button::Left, button_pos, button_dim,
						[&con, &b, makeable]() {
							con.selprod_build = &b;
							con.selprod_makeable = makeable;
						}
					);


				}



				// render build name
				if (con.sel_colony_slot_id == i) {

					render_text(win,
						build_pos, build_dim, {0.5, 0.5},
						FontTiny, ColIndWhite, ColorBlack,
						b.get_name()
					);

				}

				// left click on building -- assign worker
				con.on(Event::Press, Button::Left, build_pos, build_dim,
					[&con, workplace_id]() {
						if (con.get_sel_unit()) {
							con.command(format("work-build %||", workplace_id));
						}
					}
				);

				// number of produced items
				if (int y = nominal_prod) {
					auto& item_tex = res(win, ICON, get_item_icon_id(b.get_proditem()));
					auto item_dim = get_dim(item_tex);

					// item icon
					render_sprite(win, build_pos, item_tex);

					// number
					render_text(win,

						build_pos + v2i(item_dim[0], 0),
						v2i(0, item_dim[1]),
						{0, 0.5},

						FontTiny, ColIndWhite, ColorBlack,
						std::to_string(y)
					);
				}


				// hover on building -- show label with building name (select buildplace)
				con.on(Event::Hover, build_pos, build_dim,
					[&con, i]() { con.sel_colony_slot_id = i; }
				);

				v2i units_frame = {ly.S(25), ly.S(16)};

				// units on build
				int n = b.units.size();
				int i = 0;
				for (auto& unit_ptr: b.units) {
					auto& unit = *unit_ptr;
					auto& unit_tex = res(win, ICON, get_icon_id(unit));

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
						render_inline(win, sel_pos, sel_dim, {255,100,100,255}, ly.line_sel);
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

			auto& tex_wood = res(win, WOODBG, 1);
			render_area(win, ly.city_fields.pos, ly.city_fields.dim, tex_wood);

			auto city_terr_pos = calc_align(ly.city_fields.pos, ly.city_fields.dim, ly.terr_dim);

			auto& city_terr = terr;

			auto pos = env.get_coords(terr);
			auto city_coords = Coords(pos[0]-1, pos[1]-1);


			for(size_t field_id = 0; field_id < col.fields.size(); ++field_id) {
				auto& field = col.fields.at(field_id);

				auto& field_terr = field.get_terr();

				v2i relc = v2i(env.get_coords(*field.terr) - env.get_coords(city_terr));
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

						auto& unit_tex = res(win, ICON, get_icon_id(unit));

						// field production
						if (auto proditem = field.get_proditem()) {
							// render produced item
							auto& item_tex = res(win, ICON, get_item_icon_id(proditem));
							render_sprite(win,
								calc_align(Box(item_pos, item_dim), get_dim(item_tex)),
								item_tex
							);

							// render produced item amount
							auto text = to_string(logic::get_nominal_prod(field, field.get_proditem()));

							render_text_at(win,
								item_pos,
								FontTiny, ColIndWhite, ColorBlack,
								text
							);
						}

						// render unit on field
						render_sprite(win,
							calc_align(Box(unit_pos, unit_dim), get_dim(unit_tex)),
							unit_tex
						);

						auto unit_id = unit.id;

						if (unit_id == con.get_sel_unit_id()) {
							// render selection frame
							render_inline(win, sel_pos, sel_dim, {255,100,100,255}, ly.line);

							// left click on field with selected worker -- switch to next proditem
							/*con.on(Event::Press, Button::Left, sel_pos, sel_dim,
								[&con,field_id]() {
									con.command("prodnext-field " + to_string(field_id));
								}
							);*/



							// left click on field with selected worker -- show select proditem menu
							con.on(Event::Press, Button::Left, sel_pos, sel_dim,
								[&con, &field]() {
									con.prod_to_workplace = &field;
									con.selected_id = get_id(field.get_proditem());
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



		//array<uint8,16> num_workers;
		//array<int,16> prod;
		//num_workers.fill(0);
		//prod.fill(0);

		/*
		for (auto& p: terr.units) {
			auto& unit = *p;
			if (unit.workplace != nullptr) {
				if (unit.workplace->place_type() == PlaceType::Build) {

				}
			}
		}*/

		// render terr units

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
			ColorDarkGreen,
			ly.line
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

				auto& unit_tex = res(win, ICON, get_icon_id(unit));
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
					render_inline(win, sel_pos, sel_dim, {255,100,100,255}, ly.line);

					// left click on selected unit (on terr) -- show equip select
					con.on(Event::Press, Button::Left, sel_pos, sel_dim,
						[&con,&unit]() {
							con.equip_to_unit_id = unit.id;
							con.selected_id = unit.get_type_id();
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

		render_text(win,
			ly.city_exit.pos + v2i(1,1),
			{0,0}, {0,0},
			FontTiny, ColIndFont, ColorNone,
			"RET"
		);

		// left click on exit button -- exit city screen
		con.on(Event::Press, Button::Left, ly.city_exit.pos, ly.city_exit.dim,
			[&con, &terr](){ con.command("exit"); }
		);

		// render fields
		//render_fields(win, env, con, ly.city_fields.pos, );


		// pop-ups
		if (con.selprod_build) {

			vector<pair<Makeable const*, string>> entries;


			auto& b = *con.selprod_build;

			auto gen_row = [&con,&b](Makeable const& mk) -> vector<string>
			{
				string name = get_name(mk);

				string cost;
				if (&mk == con.selprod_makeable) {
					cost = format("%||", mk.get_cost() - b.task.get());
				}
				else {
					cost = format("%||", mk.get_cost());
				}

				vector<string> row;
				row.push_back(name);
				row.push_back(cost);
				return row;
			};


			// cols format
			auto cols = vector<Col>();
			cols.push_back({"Name", ly.S(1) * 24 * 4, 0.0f});
			cols.push_back({"Cost", ly.S(1) * 7 * 4, 1.0f});


			auto keys = vector<Makeable const*>();
			auto rows = vector<vector<string>>();

			Colony &c = con.get_sel_terr()->get_colony();

			vector<string> np_row;
			np_row.push_back("(no production)");
			np_row.push_back("");

			// clear construction
			keys.push_back(nullptr);
			rows.push_back(np_row);

			// construct building
			for (auto& p: *env.bts) {
				auto & mk = p.second;

				if (env.can_make(b, mk) and c.select_place_for(mk)) {
					keys.push_back(&mk);
					rows.push_back(gen_row(mk));
				}
			}

			// construct unit
			for (auto& p: *env.uts) {
				auto & mk = p.second;

				if (env.can_make(b, mk)) {
					keys.push_back(&mk);
					rows.push_back(gen_row(mk));
				}
			}


			render_select_f(win, con,
				ly.scr.pos, ly.scr.dim, v2f(0.5, 0.5),

				cols, keys, rows,

				con.selprod_makeable,
				[&con,&b](){
					b.task.reset(con.selprod_makeable);
					con.selprod_build = nullptr;
					con.selprod_makeable = nullptr;
				},  // onselect
				[&con](){
					con.selprod_build = nullptr;
					con.selprod_makeable = nullptr;
				}   // oncancel
			);



		}
		else if (con.equip_to_unit_id) {
			vector<int> keys;
			vector<vector<string>> rows;

			for (auto const* ut: equip_to_types(env, env.get<Unit>(con.equip_to_unit_id))) {
				auto item1 = ut->get_item1();
				auto item2 = ut->get_item2();
				auto num1 = ut->get_num1();
				auto num2 = ut->get_num2();

				keys.push_back(ut->id);

				string req;
				if (item1 != ItemNone) {
					req += format("(%|| %||)", num1, get_name(item1));
				}
				if (item2 != ItemNone) {
					req += format("(%|| %||)", num2, get_name(item2));
				}

				vector<string> row;
				row.push_back(ut->get_name());
				row.push_back(req);

				rows.push_back(row);
			}

			// cols
			vector<Col> cols;
			cols.push_back({"Name", ly.S(1) * 16 * 4, 0.0f});
			cols.push_back({"Equip", ly.S(1) * 24 * 4, 1.0f});



			// onclick -> equip/orders menu
			render_select_f(win, con,
				ly.scr.pos, ly.scr.dim, v2f(0.5, 0.5),

				cols, keys, rows,

				con.selected_id,
				[&con](){
					con.command(string("equip ") + to_string(con.selected_id));
					con.equip_to_unit_id = 0;
					con.selected_id = 0;
				},  // onselect
				[&con](){
					con.equip_to_unit_id = 0;
					con.selected_id = 0;
				}   // oncancel
			);
		}


		if (con.prod_to_workplace != nullptr) {
			auto & wp = *con.prod_to_workplace;

			vector<Col> cols;
			vector<int> keys;
			vector<Row> rows;


			// no prod
			keys.push_back(0);
			Row row;
			row.push_back("(empty)");
			row.push_back("");
			rows.push_back(row);

			// prod items
			for (auto& item: get_all_items(env)) {

				auto prod = logic::get_nominal_prod(wp, item);

				if (prod) {
					keys.push_back(get_id(item));

					Row row;
					row.push_back(get_name(item));
					row.push_back(to_string(prod));

					rows.push_back(row);
				}
			}

			// cols
			cols.push_back({"Item", 14 * 4 * ly.S(1), 0.0f});
			cols.push_back({"Prod", 5 * 4 * ly.S(1), 1.0f});

			// onclick -> assign proditem to wp
			render_select_f(win, con,
				ly.scr.pos, ly.scr.dim, v2f(0.5, 0.5),

				cols, keys, rows,

				con.selected_id,
				[&con,&wp]() {
					wp.set_proditem(Item(con.selected_id));
					con.prod_to_workplace = nullptr;
					con.selected_id = 0;
				},  // onselect
				[&con](){
					con.prod_to_workplace = nullptr;
					con.selected_id = 0;
				}   // oncancel
			);
		}




	}


	void render_units(backend::Back &win, Console & con,
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

				auto& unit_tex = res(win, ICON, get_icon_id(unit));
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
					render_inline(win, sel_pos, sel_dim, {255,100,100,255}, ly.line);
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

	void render_diffuse_from(backend::Back &win,
		v2i const& pix,
		LocalArea const& loc,
		Dir::type d,
		int offset
	) {
		auto b = get(loc, d).biome;
		if (b != BiomeNone) {
			render_sprite(win, pix, res(win, DIFFUSE, get_biome_icon_id(b) + offset) );
		}
		else {
			// no diffuse from unexplored terrain (looks as bad as diffuse on water from terrain)
		}
	}



	//int const TextureMountainId;
	//int const TextureHillId;






	void render_terr(backend::Back &win,
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
			render_sprite(win, pix, res(win, PHYS, 149));
			return;  // unknown terrain - early return !!!
		}

		render_sprite(win, pix, res(win, TERR, get_biome_icon_id(biome)));

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

			render_sprite(win, pix + v2i(0,0), res(win, COAST,
				base + get_coast_index(0, get(loc,Dir::A), get(loc,Dir::Q), get(loc,Dir::W))
			));

			render_sprite(win, pix + v2i(h,0), res(win, COAST,
				base + get_coast_index(1, get(loc,Dir::W), get(loc,Dir::E), get(loc,Dir::D))
			));

			render_sprite(win, pix + v2i(h,h), res(win, COAST,
				base + get_coast_index(2, get(loc,Dir::D), get(loc,Dir::C), get(loc,Dir::X))
			));

			render_sprite(win, pix + v2i(0,h), res(win, COAST,
				base + get_coast_index(3, get(loc,Dir::X), get(loc,Dir::Z), get(loc,Dir::A))
			));

		}


		// level
		if (terr.get_alt() == AltMountain) {
			render_sprite(win, pix, res(win, PHYS, 33 + get_wxad_index_level(loc, terr.get_alt())));
		}

		if (terr.get_alt() == AltHill) {
			render_sprite(win, pix, res(win, PHYS, 49 + get_wxad_index_level(loc, terr.get_alt())));
		}

		// phys feats & improvments
		if (terr.has_phys(PhysPlow)) {
			render_sprite(win, pix, res(win, PHYS, 150));
		}

		if (terr.has_phys(PhysForest)) {
			render_sprite(win, pix, res(win, PHYS, 65 + get_wxad_index(loc, PhysForest)));
		}

		if (terr.has_phys(PhysMajorRiver)) {
			if (!is_water(terr)) {
				auto ind = get_wxad_index(loc, PhysMajorRiver|PhysMinorRiver);
				if (ind) {
					render_sprite(win, pix, res(win, PHYS, 1 + ind));
				}
				else {
					render_sprite(win, pix, res(win, PHYS, 16));
				}
			}
			else {
				for (int i = 0; i < 8; i += 2) {
					if ( get(loc, CLOCKWISE_DIRS[i]).has_phys(PhysMajorRiver) ) {
						render_sprite(win, pix, res(win, PHYS, 141 + (i >> 1)));
					}
				}
			}

		}
		else if (terr.has_phys(PhysMinorRiver)) {
			if (!is_water(terr)) {
				auto ind = get_wxad_index(loc, PhysMajorRiver|PhysMinorRiver);
				if (ind) {
					render_sprite(win, pix, res(win, PHYS, 17 + ind));
				}
				else {
					render_sprite(win, pix, res(win, PHYS, 32));
				}
			}
			else {
				for (int i = 0; i < 8; i += 2) {
					if ( get(loc, CLOCKWISE_DIRS[i]).has_phys(PhysMinorRiver) ) {
						render_sprite(win, pix, res(win, PHYS, 145 + (i >> 1)));
					}
				}
			}
		}

		if (terr.has_phys(PhysRoad)) {
			bool r = false;
			for (int i=0; i<8; ++i) {
				if ( get(loc, CLOCKWISE_DIRS[i]).has_phys(PhysRoad) ) {
					render_sprite(win, pix, res(win, PHYS, 82 + i));
					r = true;
				}
			}
			if (!r) {
				render_sprite(win, pix, res(win, PHYS, 81));
			}
		}

	}

	void render_selected_unit(backend::Back & win, Console & con, v2i const& pos, Unit & unit);



	void render_terr(backend::Back &win,
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

	using backend::Back;


	backend::Color get_unit_color(Unit const& u) {
		auto c = get_nation_color(u.get_nation());
		if (u.get_time_left() == 0) {
			return backend::Color(c.r/2, c.g/2, c.b/2, c.a/2);
		}
		return {c.r, c.g, c.b, c.a};
	}

	void render_stack(backend::Back &win,
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
			render_sprite(win, pos[0], pos[1], res(win, ICON, 4));

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
				render_sprite(win, pos[0] + ly.S(5), pos[1], res(win, ICON, p->get_flag_id()));
			}

		}
		else if (terr.has_units() and !sel_unit_here) {
			// unit
			auto & unit = env.get_defender(terr);

			auto & icon = res(win, ICON, get_icon_id(unit));
			// render unit in field
			render_shield(win, pos, get_unit_color(unit), con.get_letter(unit));
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



	void render_selected_unit(backend::Back & win, Console & con, v2i const& pos, Unit & unit) {
		auto & icon = res(win, ICON, unit.get_icon());
		auto unit_id = unit.id;

		// render blinking shield
		if (con.blink()) {
			render_shield(win, pos, get_unit_color(unit), con.get_letter(unit));
		}

		// render unit in field
		render_sprite(win,
			calc_align(Box(pos, ly.terr_dim), get_dim(icon)),
			icon
		);


		// selected unit keyboard shortcuts (numpad -> move)
		con.on(Event::Press, Key::Numpad2,
			[&con,unit_id](){ con.command("move 0 1"); }
		);
		con.on(Event::Press, Key::Numpad8,
			[&con,unit_id](){ con.command("move 0 -1"); }
		);
		con.on(Event::Press, Key::Numpad6,
			[&con,unit_id](){ con.command("move 1 0"); }
		);
		con.on(Event::Press, Key::Numpad4,
			[&con,unit_id](){ con.command("move -1 0"); }
		);
		con.on(Event::Press, Key::Numpad3,
			[&con,unit_id](){ con.command("move 1 1"); }
		);
		con.on(Event::Press, Key::Numpad7,
			[&con,unit_id](){ con.command("move -1 -1"); }
		);
		con.on(Event::Press, Key::Numpad1,
			[&con,unit_id](){ con.command("move -1 1"); }
		);
		con.on(Event::Press, Key::Numpad9,
			[&con,unit_id](){ con.command("move 1 -1"); }
		);

		// move on nums
		con.on(Event::Press, Key::Num2,
			[&con,unit_id](){ con.command("move 0 1"); }
		);
		con.on(Event::Press, Key::Num8,
			[&con,unit_id](){ con.command("move 0 -1"); }
		);
		con.on(Event::Press, Key::Num6,
			[&con,unit_id](){ con.command("move 1 0"); }
		);
		con.on(Event::Press, Key::Num4,
			[&con,unit_id](){ con.command("move -1 0"); }
		);
		con.on(Event::Press, Key::Num3,
			[&con,unit_id](){ con.command("move 1 1"); }
		);
		con.on(Event::Press, Key::Num7,
			[&con,unit_id](){ con.command("move -1 -1"); }
		);
		con.on(Event::Press, Key::Num1,
			[&con,unit_id](){ con.command("move -1 1"); }
		);
		con.on(Event::Press, Key::Num9,
			[&con,unit_id](){ con.command("move 1 -1"); }
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
		// clear forest
		con.on(Event::Press, Key::O,
			[&con,unit_id](){ con.order_unit(unit_id, 'O'); }
		);
		// space - skip unit
		con.on(Event::Press, Key::Space,
			[&con](){ con.skip_unit(); }
		);

	}

	//void select_area()


	void render_map(backend::Back &win, Env const& env, Console & con, v2i const& pos,
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

					// right press on terr with ctrl -- sub select terr
					con.on(Event::Press, Button::Right, halo::ModButton|halo::ModCtrl, pos, ly.terr_dim,
						[&con,coords](){
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

					// hover with button down with ctrl over terr -- select sub
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
						{128,128,128,255},
						ly.line
					);
				}

				auto coords = env.get_coords(*tp);

				// blink main terr
				if ((con.time % 1000) >= 500) {
					// rect frame on tile
					render_inline(win,
						vmul(coords, ly.terr_dim) + pos,
						ly.terr_dim,
						{255,255,255,255},
						ly.line
					);
				}


			}
		}


	}




	void render_bar(backend::Back &win,
			Env const& env,
			Console const& con,
			Vector2<int> const& pos,
			Vector2<int> const& dim
		) {
		// pos - left top pix
		// dim - size

		render_area(win, pos, dim, res(win, "WOODTILE_SS", 1));

	}





	void render_panel(backend::Back & win,
			Env const& env,
			Console & con,
			v2i const& pos,
			v2i const& dim
		) {

		//backend::Image img = res("COLONIZE/WOODTILE_SS", 1);  32x24

		render_area(win, pos, dim, res(win, "WOODTILE_SS", 1));

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

			info += format("\nmoves: %||", get_moves(*u));


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

		//print("-->%||<--", info);
		render_text_at2(win,
			pos,
			FontTiny, ColIndFont, ColorNone,
			info
		);

		// top right: nation indicator
		if (env.in_progress()) {
			render_pixel(win, pos + v2i(dim[0]-1,0), get_nation_color(env.get_current_nation()));
		}

		auto ren = Renderer(win, env);



		if (auto t = con.get_sel_terr()) {
			v2i pos = ly.pan.pos + v2i(0,ly.S(100));
			v2i dim = v2i(ly.pan.dim[0], ly.terr_dim[1]*3);

			render_fill(win, pos, dim,
				backend::Color(0,0,0,64)
			);

			render_units(win, con, pos, dim,
				env,
				*t
			);
		}

		
		// Idle Unit		
		// Idle Factory
		{
			if (env.in_progress()) {
				auto& p = env.get_current_nation();
			
				auto* terr = logic::get_idle_colony(env, p);
				
				auto colind = ColIndGray;
				
				// show button
				auto text_box = render_text(win,
					ly.pan.pos, ly.pan.dim - v2i(0, ly.font_tiny), v2f(0.5, 1.0),
					FontTiny, colind, ColorNone,
					"Idle Factory"
				);


				// left click -> select idle
				con.on(Event::Press, Button::Left, text_box.pos, text_box.dim,
					[&con,terr](){ con.select_terr(terr); }
				);
				
			}
		}
		// End of turn/Start game
		// nothing to move -> enter to ready turn
		{
			auto lab = string();
			auto cmd = string();
			auto colind = ColIndRed;

			if (env.in_progress()) {
				auto& p = env.get_current_nation();
				
				auto & mem = con.mem;

				if (mem.get_next_to_repeat(env, p)) {
					lab = "Move all";
					cmd = "move-all";
					colind = ColIndWhite;

					// Enter -> move all
					con.on(Event::Press, Key::Enter,
						[&con](){ con.repeat_all(); }
					);
				}
				else {
					lab = "End of turn";
					cmd = "ready";
					colind = (mem.get_next_to_order(env, p)) ? ColIndGray : ColIndWhite;

					// Enter -> end turn
					con.on(Event::Press, Key::Enter,
						[&con](){ con.command("ready"); }
					);
				}
			}
			else {
				lab = "Start game";
				cmd = "start";				
				colind = (env.nations.size() < 1) ? ColIndGray : ColIndWhite;
			}
			
			

			// label
			auto text_box = render_text(win,
				ly.pan.pos, ly.pan.dim, v2f(0.5, 1.0),
				FontTiny, colind, ColorNone,
				lab
			);

			// left click -> start/ready
			con.on(Event::Press, Button::Left, text_box.pos, text_box.dim,
				[&con,cmd](){ con.command(cmd); }
			);

		}



	}









	void render_cmd(backend::Back & app, col::Console & con) {
		if (con.is_active()) {

			auto ln = con.output.size();
			v2i pos = v2i(ly.map.pos[0], ly.map.pos[1]);

			for (auto& line: boost::adaptors::reverse(con.output)) {


				auto text_box = render_text(app,
					pos, {0,0}, {0,0},
					FontTiny, ColIndFont, {0,0,0,128},
					line
				);

				pos[1] += text_box.dim[1];
			}

			// command line with cursor
			render_text(app,
				ly.scr.pos, {0,0}, {0,0},
				FontTiny, ColIndFont, ColorNone,
				con.buffer + "_"
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

			// enter
			con.on(Event::Press, Key::Return, [&con](){
				con.handle_char(u'\r');
			});

			// backspace
			con.on(Event::Press, Key::Backspace, [&con](){
				con.handle_char(u'\b');
			});





		}
		else {
			// activate
			con.on(Event::Char, u'`', [&con](){
				con.set_active(true);
			});
		}

	}


	void render_cursor(backend::Back & win, Console const& con) {
		if (con.drag_item) {

			auto mouse = backend::get_mouse();
			auto pos = v2i(get_logical_pos(win, mouse.pos));
			//print("cursor: %||, %||\n", mp.x, mp.y);

			render_sprite(win, pos, res(win, ICON, get_item_icon_id(con.drag_item)));
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



	void render(backend::Back & app, col::Env & env, col::Console & con, int verbose) {

		if (verbose >= 2) print("render: {\n");

		app.clear();

		con.reset_hotspots();

		int w = app.get_logical_dim()[0];
		int h = app.get_logical_dim()[1];


		render_drag_clear(con);


		// bar top
		render_bar(app, env, con, ly.bar.pos, ly.bar.dim);

		// hline under the bar
		render_fill(app,
			{ly.bar.pos[0], ly.bar.end[1]},
			{ly.bar.dim[0], ly.line},
			{0,0,0,255}
		);


		if (con.mode == Console::Mode::COLONY) {
			render_city(app, env, con);
		}
		else {

			if (verbose >= 2) print("render: render_map\n");

			// map
			render_map(app, env, con, ly.map.pos, Coords(0,0));

			// panel right
			render_panel(app, env, con,
				ly.pan.pos,
				ly.pan.dim
			);

			// vline left of the panel
			render_fill(app,
				{ly.pan.pos[0] - ly.line, ly.pan.pos[1]},
				{ly.line, ly.pan.dim[1]},
				{0,0,0,255}
			);

		}

		//if (env.curr_nation) {
		//	render_nationind(app, 0, 0, env.curr_nation->color);
		//}

		// BACKGROUND

		if (verbose >= 2) print("render: render_cmd\n");

		render_cmd(app, con);

		render_cursor(app, con);

		if (verbose >= 2) print("render: app.flip\n");

		app.flip();

		if (verbose >= 2) print("render: }\n");

	}





}
