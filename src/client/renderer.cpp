#include "renderer.hpp"

//#include <functional>
#include <boost/range/adaptor/reversed.hpp>

#include "base.hpp"
#include "logic.hpp"
#include "props.hpp"
#include "conf.hpp"
#include "align.hpp"
#include "textrend.hpp"
#include "layout.hpp"
#include "simple_ai.hpp"
#include "colordefs.hpp"


//namespace Key = front::Key;

namespace col {


	using namespace logic;
	using namespace front::keys;


	using Event = Console::Event;
	using Button = Console::Button;
	using Key = front::Key;
	using Dev = Console::Dev;


	
	// frequent short literals lol :D
	int16_t const
		_0 = 0,
		_1 = 1,
		_2 = 2,
		_3 = 3,
		_4 = 4;



	Style const StyleMenu(ColorFont, ColorNone, ColorFontSelected, ColorFontDisabled);
	
	
	
	// Texture get dim
	v2s get_dim(Texture const& t) { return t.dim; }



	PixFont FontTiny;
	



	
	//ResMap const& get_unit_tex(Unit const& unit) {
	//	return res(win, ICON, get_unit_icon_id(unit));
	//}


	inline
	bool is_water(Terr const& t) {
		return t.alt == AltSea;
	}




	using ResKey = uint32_t;
	using ResCat = uint16_t;
	using ResNum = uint16_t;

	using ResMap = unordered_map<ResKey, Texture>;

	ResMap res_map;
	
	ResKey make_reskey(ResCat cat, ResNum num) {
		return (ResKey(cat) << 16) | ResKey(num);
	}

	ResCat get_rescat(ResKey k) {
		return k >> 16;
	}

	ResNum get_resnum(ResKey k) {
		return (k << 16) >> 16;
	}

	ResCat const
		TERR = 0,
		ICON = 1,
		PHYS = 2,
		ARCH = 3,
		COLONY = 4,
		BUILD = 5,
		WOODTILE = 6,
		DIFFUSE = 7,
		COAST = 8,
		ZZ = 9;
		

	Path get_res_dir(ResCat cat) {
		// categories, *xxx* -- generated to memory image
		switch (cat) {
			case TERR: return "TERRAIN_SS";
			case ICON: return "ICONS_SS";
			case PHYS: return "PHYS0_SS";
			case ARCH: return "PARCH_SS";
			case COLONY: return "COLONY_PIK";
			case BUILD: return "BUILDING_SS";
			case WOODTILE: return "WOODTILE_SS";
			case DIFFUSE: return "*DIFFUSE*";
			case COAST: return "*COAST*";
			case ZZ: return "ZZ";
		}
		fail("unknown resource category: %||\n", cat);
	}

	Path get_res_path(ResCat cat, ResNum num) {
		return conf.tile_path / get_res_dir(cat) / format("%|03|.png", num);
	}

	Path get_res_path_key(ResKey key) {		
		return get_res_path(get_rescat(key), get_resnum(key));
	}


	// Get resource from global cache (load on demand)
	Texture const& res(Front & fr, ResCat cat, ResNum num)
	{
		auto key = make_reskey(cat, num);		
		auto p = res_map.find(key);		
		if (p != res_map.end()) {
			return (*p).second;
		}
		else {
			auto path = get_res_path(cat, num);
			auto & img = res_map[key];
			img = fr.make_texture(path);
			return img;
		}
	}




	void preload(ResCat cat, ResNum num, Texture && tex) {
		auto key = make_reskey(cat, num);		
		res_map[key] = std::move(tex);
	}

	Image replace_black(Image const& inn, Image const& tex, v2s toff) {
		/* Create new surface

		mask colors meaning:
		 black -> take from tex
		 else -> take from mask

		 toff -- tex offset vector

		*/

		//print("replace_black: %|| + %|| <- %||\n", inn.dim, toff, tex.dim);

		auto dim = inn.get_dim();
		auto out = Image(dim);

		for (int j = 0; j < dim[1]; ++j) {
			for (int i = 0; i < dim[0]; ++i) {
				auto c = inn({i,j});
				auto pos = v2s(i,j);

				//print("c = %||\n", c);

				if (c.a == 0) {
					out(pos) = Color(0,0,0,0);
				}
				else if (c == Color(0,0,0,255)) {

					out(pos) = tex(toff + pos);
				}
				else {
					out(pos) = inn(pos);
				}

			}
		}

		return out;
	}

	//void render_fields(Front &win, Env const& env, Console const& con,
	//	v2s pix, Coords const& pos);


	v2s get_loffset(int l) {
		switch(l){
			case 0: return v2s(0,0) * ly.scale;
			case 1: return v2s(8,0) * ly.scale;
			case 2: return v2s(8,8) * ly.scale;
			case 3: return v2s(0,8) * ly.scale;
		}
		fail("invalid 'l'");
	}

	

	Texture make_combined_texture(
		Front & fr,
		std::pair<ResCat, ResNum> p,
		std::pair<ResCat, ResNum> b,
		v2s off
	) {
		auto p_img = front::load_png(
			get_res_path(p.first, p.second)
		);

		auto b_img = front::load_png(
			get_res_path(b.first, b.second)
		);

		auto surf = replace_black(p_img, b_img, off);

		return fr.make_texture(surf);
	}



	void preload_coast(Front & fr, Biome bio, int start) {
		for (int k = 0; k < 8; ++k) {
			for (int l = 0; l < 4; ++l) {
				int ind = (k<<2) + l;

				preload(COAST, start + ind,
					make_combined_texture(fr,
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

	void preload_coast(Front & back) {
		// 109 - 140

		preload_coast(back, TextureOceanId, 0);
		preload_coast(back, TextureSeaLaneId, 50);
	}




	void preload_terrain(Front & fr) {
		for (int i=1; i<13; ++i) {
			preload(DIFFUSE,  0+i,
				make_combined_texture(fr, {PHYS, 105}, {TERR, i}, {0,0})
			);
			preload(DIFFUSE, 50+i,
				make_combined_texture(fr, {PHYS, 106}, {TERR, i}, {0,0})
			);
			preload(DIFFUSE, 100+i,
				make_combined_texture(fr, {PHYS, 107}, {TERR, i}, {0,0})
			);
			preload(DIFFUSE, 150+i,
				make_combined_texture(fr, {PHYS, 108}, {TERR, i}, {0,0})
			);
		}

		preload_coast(fr);
	}

	
	
	void preload_renderer(Front & fr) {
		preload_terrain(fr);
		FontTiny.load(fr, conf.font_tiny_path, -1 * ly.scale);
	}

	ResNum get_prof_icon_id(Prof prof, ResNum def) {
		switch (prof) {
			case ProfFarmer: return 82;
			case ProfFisherman: return 90;
			case ProfSugarPlanter: return 83;
			case ProfTobaccoPlanter: return 84;
			case ProfCottonPlanter: return 85;
			case ProfFurTrapper: return 86;
			case ProfLumberjack: return 87;
			case ProfOreMiner: return 88;
			case ProfSilverMiner: return 89;
			case ProfDistiller: return 91;
			case ProfTobaconist: return 92;
			case ProfWeaver: return 93;
			case ProfFurTrader: return 94;
			case ProfCarpenter: return 95;
			case ProfScout: return 61;
			case ProfBlacksmith: return 96;
			case ProfGunsmith: return 97;
			case ProfSoldier: return 60;
			case ProfPioneer: return 59;
			case ProfPreacher: return 98;
			case ProfStatesman: return 99;
		}
		return def;
	}

	
	ResNum get_icon_id(Unit const& unit) {
		ResNum const IconErrorId = 56;
		
		auto prof = unit.get_prof();		
				
		if (prof == ProfNone) {
			auto icon_gray_id = unit.get_icon();		
			return icon_gray_id;
		}
		else {
			auto icon_expert_id = unit.get_icon_expert();
			
			if (icon_expert_id != 0) {
				return icon_expert_id;
			}			
		
			// colonist
			return get_prof_icon_id(unit.get_prof(), IconErrorId);
		}
	}
	


	void render_fill(Front & fr,
			v2s pos,
			v2s dim,
			Color c
	) {
		fr.render_fill({pos, dim}, c);
	}

	void render_inline(Front & win, v2s pos, v2s dim, Color c, int8_t t) {
		/* Draw inside border of pos,dim box; t -- thickness
		*/
		
		auto wh = dim - v2s(t,t);
		auto w = wh[0];
		auto h = wh[1];
		
		// top
		render_fill(win, pos + v2s{0,0}, {w,t}, c);
		// right
		render_fill(win, pos + v2s{w,0}, {t,h}, c);
		// bottom
		render_fill(win, pos + v2s{t,h}, {w,t}, c);
		// left
		render_fill(win, pos + v2s{0,t}, {t,h}, c);
	}

	void render_inline(Front & win, b2s box, Color c, int8_t t) {
		render_inline(win, box.pos, box.dim, c, t);
	}

	void render_outline(Front &win, v2s pos, v2s dim, Color c, int8_t t) {
		/* Draw 1px thick outline border of pos,dim box
		*/
		render_inline(win, pos - v2s(t,t), dim + v2s(t,t) * _2, c, t);
	}

	void render_outline(Front &win, b2s box, Color c, int8_t t) {
		render_outline(win, box.pos, box.dim, c, t);
	}


	void render_terr(Front &win,
			v2s pos,
			Env const& env,
			Terr const& terr);



	void render_shield(Front &win, v2s pos, Color const& color, char letter) {

		auto inner_pos = pos + v2s(1,1) * ly.line;
		auto inner_dim = v2s(ly.S(5), ly.font_tiny);

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
			pos + v2s(1,1) * ly.line,
			FontTiny, ColorBlack,
			string() + letter
		);
	}


	void render_pixel(Front &win, v2s pix, const Color &colr) {
		render_fill(win,
			pix,
			v2s(ly.S(1), ly.S(1)),
			colr
		);
	}



	void render_sprite(Front & win, v2s pix, Texture const& img) {
		win.render(img, pix);
	}

	void render_sprite(Front & win, 
		b2s box, v2f const& align,
		Texture const& img
	) {
		win.render(img, calc_align(box, align, get_dim(img)));
	}

	void render_sprite(Front & win, 
		v2s pos, v2s dim, v2f const& align,
		Texture const& img
	) {
		render_sprite(win, {pos, dim}, align, img);
	}



	void render_area(
		Front & win,
		v2s area_pos,
		v2s area_dim,
		Texture const& tex
	);




	void render_area(Front & win, v2s pos, v2s dim, Color const& color) {
		render_fill(win, pos, dim, color);
	}

	
	inline Color get_nation_color(Nation const& n) {
		auto c = n.color;
		return {c.r,c.g,c.b,255};
	}


	void render_area(
		Front & win,
		v2s area_pos,
		v2s area_dim,
		front::Texture const& tex
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
			auto src_box = b2s({0, 0}, {tile_dim[0], area_end[1] - ej});

			int i = area_pos[0], j = ej;
			while (i < area_end[0] - tile_dim[0]) {
				win.render(tex, {i, j}, src_box);
				i += tile_dim[0];
			}
			ei = i;
		}

		// right
		{
			auto src_box = b2s({0, 0}, {area_end[0] - ei, tile_dim[1]});
			int i = ei, j = area_pos[1];
			while (j < area_end[1] - tile_dim[1]) {
				win.render(tex, {i, j}, src_box);
				j += tile_dim[1];
			}
			ej = j;
		}

		// corner
		{
			auto src_box = b2s({0, 0}, {area_end[0] - ei, area_end[1] - ej});
			int i = ei, j = ej;
			win.render(tex, {i, j}, src_box);
		}

	}

	//Color Black = {0,0,0,255};
	//Color Transparent = {0,0,0,0};


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

	
	template <class T>
	T limit(T x, T a, T b) {
		if (x < a) return a;
		if (b < x) return b;
		return x;
	}
	
	
	

	
	
	
	struct Elem {
		vector<string> text;  // empty -> separator			
		Action onselect;  // empty -> disabled
		Key shortcut{KeyNone};
		
		Elem() {}
		Elem(vector<string> const& text, Action const& onselect, Key shortcut):
			text(text),
			onselect(onselect),
			shortcut(shortcut)				
		{}		
	};

	struct Select: Widget {
			
		v2s pos;
		v2s dim;
		
		//b2s box;
		
		vector<Col> cols;
		vector<Elem> rows;
		
		Action oncancel;
		Action onselect;
				
		size_t cursor{0};
		size_t highlight;
		
		Select() { highlight = -1; }
		
		void add(vector<string> const& text, Action const& onselect, Key shortcut=KeyNone) {
			rows.push_back(Elem(text, onselect, shortcut));
		}
		
		void add(vector<string> const& text, bool hl, Action const& onselect, Key shortcut=KeyNone) {
			if (hl) { set_highlight(); }
			rows.push_back(Elem(text, onselect, shortcut));
		}
		
		/*void add(string const& text, Action const& onselect, Key shortcut=KeyNone) {
			rows.push_back(Elem({text}, onselect, shortcut));
		}*/
		
		void render(Front & win, Console & con) override;
		
		void move(size_t i) {
			cursor = i;
			if (cursor <= 0) {
				cursor = rows.size();
			}
			if (cursor >= rows.size()) {
				cursor = 0;
			}
		}

		void move_up() {
			if (cursor <= 0) {
				cursor = rows.size();
			}
			cursor -= 1;			
		}

		void move_down() {
			cursor += 1;
			if (cursor >= rows.size()) {
				cursor = 0;
			}
		}
		
		void select() {
			if (cursor < rows.size()) {
				rows.at(cursor).onselect();
			}
			onselect();				
		}
		
		void set_highlight() {
			highlight = rows.size();
			cursor = rows.size();
		}
		
		void set_geo(b2s box, v2f align) 
		{
			// default single column
			if (cols.size() == 0) {
				cols.push_back(Col("", 400, 0.0f));			
			}		
		
			auto line_height = ly.font_tiny;
			auto sep = ly.sep;   // cell sep
			
			int width = 0;
			for (auto & c: cols) {
				width += c.width;
			}
			width += get_elems_stretch(cols.size(), 0, sep[0]);

			int height = get_elems_stretch(rows.size(), line_height, sep[1]);
			
			dim = v2s(width, height);			
			pos = calc_align(box, align, dim);
		}
	};

	void Select::render(Front & win, Console & con) {
		
		auto line_height = ly.font_tiny;
		auto sep = ly.sep;   // cell sep

		// any event -- ignore
		con.on();


		// click anywhere -- cancel
		con.on(Event::Press, oncancel);

		// esc -- cancel
		con.on(Event::Press, KeyEsc, oncancel);

		// background & outline border
		render_area(win, pos, dim, res(win, WOODTILE, 1));
		render_outline(win, pos, dim, ColorDarkBrown, ly.line);

		// click on dialog -- do nothing
		con.on(Event::Press, pos, dim, [](){});

		// enter -- select under cursor
		con.on(Event::Press, KeyEnter, 
			[this](){ this->select(); }
		);

		// up arrow -- move cursor
		con.on(Event::Press, KeyUp, 
			[this](){ this->move_up(); }
		);

		// down arrow -- move cursor
		con.on(Event::Press, KeyDown, 
			[this](){ this->move_down(); }
		);
		
		auto row_dim = v2s(dim[0] - sep[0]*2, line_height);
		auto row_pos = pos + sep;

		for (size_t j = 0; j < rows.size(); ++j) {
			auto & row = rows[j];

			auto cpos = row_pos;

			if (j == cursor) {
				render_fill(win,
					row_pos, row_dim, ColorSelectedBG
				);
			}
			
			// left click on row -- move cursor
			con.on(Event::Press, Button::Left, row_pos, row_dim,
				[this, j](){ 
					this->move(j); 
				}
			);
			
			// left release on row -- select
			con.on(Event::Release, Button::Left, row_pos, row_dim,
				[this](){ this->select(); }
			);

			
			//if (row.text.size() == 0) {
			//	// separator
			//}
			
			auto fg = ColorFont;
			if (not row.onselect) {
				// disabled
				fg = ColorFontDisabled;
			}
			else if (j == highlight) {
				// highlighted
				fg = ColorFontSelected;
			}

			// render row by cell
			for (size_t i = 0; i < cols.size(); ++i) {
				auto & col = cols[i];
				auto & cell = row.text[i];

				auto cdim = v2s(col.width, line_height);

				render_text(win,
					cpos, cdim, v2f(col.align, 0.5),
					FontTiny, fg,
					cell
				);

				cpos[0] += col.width + sep[0];
			}

			row_pos[1] += line_height + sep[1];
		}

	}


	


	
	template<class Key>
	void render_select_f(Front & win, Console & con,
		v2s g_pos,
		v2s g_dim,
		v2f const& align,

		vector<Col> const& cols,
		vector<Key> const& keys,
		vector<vector<string>> const& rows,

		Key & selected,
		std::function<void()> onselect,
		std::function<void()> oncancel

		//int & cursor
	) {

		auto line_height = ly.font_tiny;
		auto sep = v2s(ly.S(1), ly.S(1));   // cell sep

		int width = 0;
		for (auto& col: cols) {
			width += col.width;
		}
		width += get_elems_stretch(cols.size(), 0, sep[0]);

		int height = get_elems_stretch(rows.size(), line_height, sep[1]);

		// init cursor position if not specified
		/*int cursor = limit(cursor, 0, keys.size() - 1);
		
		if (cursor < 0 or cursor_max < cursor) {
			cursor = 0;
			for (size_t i=0; i < keys.size(); ++i) {
				if (keys[i] == selected) {
					cursor = i;
					break;
				}
			}
		}*/		

		v2s dim = v2s(width, height);
		auto pos = calc_align({g_pos, g_dim}, dim, align);


		// click anywhere -- cancel
		con.on(Event::Press, oncancel);

		// esc -- cancel
		con.on(Event::Press, KeyEsc, oncancel);

		// background & outline border
		render_area(win, pos, dim, res(win, WOODTILE, 1));
		render_outline(win, pos, dim, ColorDarkBrown, ly.line);

		// click on dialog -- do nothing
		con.on(Event::Press, pos, dim, [](){});

		// enter -- select under cursor
		/*con.on(Event::Press, KeyEnter, 
			[&selected,cursor,&keys,onselect](){ selected = keys.at(cursor); onselect(); }
		);*/

		// up arrow -- move cursor
		/*con.on(Event::Press, KeyEnter, 
			[cursor,onselect](){ cursor; onselect(); }
		);*/

		// down arrow -- move cursor
		
		auto row_dim = v2s(dim[0] - sep[0]*2, line_height);
		auto row_pos = pos + sep;

		for (size_t j = 0; j < rows.size(); ++j) {
			auto & row = rows[j];
			auto & key = keys[j];

			auto cpos = row_pos;

			// selected bg and font fg
			auto fgind = ColorFont;

			if (key == selected) {
				render_fill(win,
					row_pos, row_dim, ColorSelectedBG
				);
			}
			
			if (key == selected) {
				fgind = ColorFontSelected;
			}

			// left click on row -- select
			con.on(Event::Press, Button::Left, row_pos, row_dim,
				[&selected,key,onselect](){ selected = key; onselect(); }
			);

			// render row by cell
			for (size_t i = 0; i < cols.size(); ++i) {
				auto & col = cols[i];
				auto & cell = row[i];

				auto cdim = v2s(col.width, line_height);

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
	void render_select(Front & win, Console & con,
		std::function<v2s(v2s dim)> cpos,
		//Box const& par, v2f const& align,
		vector<pair<K, string>> const& kvs,
		K & selected,
		std::function<void()> onselect,
		std::function<void()> oncancel
	) {

		int const LINE_HEIGHT = ly.font_tiny;

		v2s const dim = v2s(ly.scr.dim[0] * 0.5, (kvs.size()+1) * LINE_HEIGHT);
		v2s pos = cpos(dim);

		// click anywhere -- cancel
		con.on(Event::Press, oncancel);

		// background
		render_area(win, pos, dim, res(win, WOODTILE, 1));
		render_outline(win, pos, dim, ColorDarkBrown, ly.line);


		// click on dialog -- do nothing
		con.on(Event::Press, pos, dim, [](){});

		// entries
		for (size_t i = 0; i < kvs.size(); ++i) {

			auto p = pos + v2s(0, i * LINE_HEIGHT);
			auto d = v2s(dim[0], LINE_HEIGHT);

			auto & key = kvs[i].first;

			auto fg = ColorFont;

			// selected bg
			if (key == selected) {
				render_fill(win,
					p, d,
					ColorSelectedBG
				);

				fg = ColorFontSelected;
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


	void render_unit_cargo(Front &win, Console & con, 
		v2s pos, 
		v2s dim, 
		Unit const& unit
	) {
		// render unit cargo


		// render unit items
		auto cpos = pos;
		auto item_dim = v2s(ly.S(12), dim[1]);		
		auto text_dim = v2s(ly.S(4) * 3 + ly.S(2), dim[1]);
		auto tile_dim = v2s(item_dim[0] + text_dim[0], dim[1]);
		
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
					cpos + v2s(item_dim[0], 0), text_dim, v2f(0, 0.5),
					FontTiny, ColorWhite, ColorNone,
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
					cpos + v2s(item_dim[0], 0), text_dim, v2f(0, 0.5),
					FontTiny, ColorWhite, ColorNone,
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






	void show_select_makeable(Env const& env, Console & con, Colony & colony, Build & factory, b2s box, v2f const& align) {
		
		auto & select = con.replace_widget<Select>();
					
		// cols format			
		select.cols.push_back({"Name", ly.S(1) * 24 * 4, 0.0f});
		select.cols.push_back({"Cost", ly.S(1) * 7 * 4, 1.0f});

		auto gen_row = [&factory](Makeable const& mk) -> vector<string>
		{
			string cost;
			string name;
			
			if (&mk == factory.task.what) {
				name = format("%||", get_name(mk));
				cost = format("%||", mk.get_cost() - factory.task.get());
				
			}
			else {
				name = format("%||", get_name(mk));
				cost = format("%||", mk.get_cost());	
			}
			
			vector<string> row;
			row.push_back(name);
			row.push_back(cost);
			return row;
		};


		if (factory.task.what == nullptr) {
			select.set_highlight();
		}
		
		select.add({"(no production)", ""}, [&factory](){
			factory.task.reset(nullptr);
		});
		
		// construct building
		for (auto& p: *env.bts) {
			auto & mk = p.second;
			if (env.can_make(factory, mk) and colony.select_place_for(mk)) {
				
				if (factory.task.what == &mk) {
					select.set_highlight();
				}
				
				select.add(gen_row(mk), [&factory, &mk](){
					factory.task.reset(&mk);
				});
				
			}
		}

		// construct unit
		for (auto& p: *env.uts) {
			auto & mk = p.second;
			if (env.can_make(factory, mk)) {
				
				if (factory.task.what == &mk) {
					select.set_highlight();
				}
				
				select.add(gen_row(mk), [&factory, &mk](){
					factory.task.reset(&mk);
				});
				
			}
		}
		
		select.oncancel = [&con](){
			con.clear_widget();
		};   // oncancel
		
		select.onselect = [&con](){
			con.clear_widget();
		};   // onselect
		
		

		select.set_geo(box, align);

		
	}


	void render_city_store(Front & win, Console & con, v2s pos, v2s dim, Terr const& terr) {
		auto const& env = con.env; // get_render_env

		auto const& supp_reg = env.get_store(terr);

		Register prod_reg, cons_reg;
		fill_colony_regs(env, terr, prod_reg, cons_reg);

		// render supply nums bg
		render_area(win,
			ly.city_supply_nums.pos, ly.city_supply_nums.dim,
			{76,100,172,255}
		);

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
		auto cell_dim = v2s(tile_width, line_height);
		auto up = v2s(0, -line_height);
		for (auto item: COLONY_ITEMS) {

			auto supp_num = supp_reg.get(item);
			auto prod_num = prod_reg.get(item);
			auto cons_num = cons_reg.get(item);			
			auto delta = prod_num - cons_num;
			
			auto tile_pos = cpos;
			auto tile_dim = v2s(tile_width, ly.S(12));

			// render item
			render_sprite(win, {tile_pos, tile_dim}, {0.5f, 0.5f},
				res(win, ICON, get_item_icon_id(item))
			);

			// left click on item -- start drag cargo load
			con.on(Event::Press, Button::Left, tile_pos, tile_dim,
				[item,supp_num,&con](){ con.drag_cargo(item, supp_num, +1); }
			);

			// render supp num
			{
				auto pos = cpos + v2s(0, tile_dim[1]);
				auto dim = v2s(tile_width, line_height);
					
				auto proj = supp_num - cons_num + prod_num;
				if (proj < 0) {
					render_text(win,
						pos, cell_dim, v2f(0.5f, 0),
						FontTiny, ColorYellow, ColorNone,
						format("%||", supp_num)
					);
				}
				else {
					render_text(win,
						pos, cell_dim, v2f(0.5f, 0),				
						FontTiny, ColorWhite, ColorNone,
						std::to_string(supp_num)
					);
				}
			}
		
			// render prod num
			if (prod_num) {
				render_text(win,
					cpos + up * _3, cell_dim, v2f(1, 0),
					FontTiny, ColorWhite, ColorNone,
					format("+%||", prod_num)
				);
			}

			// render cons num
			if (cons_num) {
				render_text(win,
					cpos + up * _2, cell_dim, v2f(1, 0),
					FontTiny, ColorWhite, ColorNone,
					format("-%||", cons_num)
				);
			}
		
			// render delta
			if (delta > 0) {
				render_text(win,
					cpos + up, cell_dim, v2f(1, 0),
					FontTiny, ColorGreen, ColorNone,
					format("+%||", delta)
				);
			}
			else if (delta < 0) {
				render_text(win,
					cpos + up, cell_dim, v2f(1, 0),
					FontTiny, ColorYellow, ColorNone,
					format("%||", delta)
				);
			}
		
			
			cpos[0] += tile_width;
		}
	}

	

	
	void render_unit_tile(Front & win, b2s box, Unit const& unit)
	{		
		/// Render unit icon with terrain icon in background
		
		auto & unit_tex = res(win, ICON, get_icon_id(unit));
		auto & bgs_tex = res(win, COLONY, 1);

		// determine terrain icon: land or water
		b2s src_box;
		if (unit.get_travel() == TravelSea) {
			// water src box			
			src_box = b2s({141, 29}, {16, 3}) * ly.scale;
		}
		else {
			// land src box
			src_box = b2s({66, 27}, {16, 3}) * ly.scale;
		}
		// TODO: transported unit

		// render terrain icon
		win.render(bgs_tex,
			calc_align(box, {0.5f, 1.0f}, src_box.dim),
			src_box
		);
		
		// render unit
		render_sprite(win, box, {0.5f, 0.5f}, unit_tex);
	}

	void render_city(Front &win, Env const& env, Console & con) {
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

		//auto render = Renderer{win,env};



		// render area
		auto & sand_tex = res(win, ARCH, 1);
		render_area(win, ly.city_builds.pos, ly.city_builds.dim, sand_tex);

		auto & pixs = ly.pixs;
		auto & dims = ly.dims;


		auto& terr = *con.get_sel_terr();
		auto& colony = terr.get_colony();

		// render buildings
		{
			// hover anywhere -- clear label on building
			con.on(Event::Hover,
				[&con]() { con.sel_colony_slot_id = -1; }
			);

			// render in reverse order to make sure that docks clickarea will not obscure other builds
			int i = colony.builds.size();
			while (i > 0) {
				i = i - 1;

				auto workplace_id = i;

				auto & build = colony.builds.at(i);
				
				auto& build_tex = res(win, BUILD, build.get_type_id());
				auto build_pos = ly.city_builds.pos + pixs.at(i);
				auto build_dim = get_dim(build_tex);

				render_sprite(win, build_pos, build_tex);

				int nominal_prod = logic::get_nominal_prod(env, build, build.get_proditem());

				// production button under the building
				if (build.get_proditem() == ItemHammers)
				{

					auto button_pos = build_pos + v2s(0, build_dim[1]);
					auto button_dim = v2s(build_dim[0], ly.font_tiny);

					
					int blink = 0;
					string label, progress;
					if (build.task) {
						label = get_name(*build.task.what);
						label = label.substr(0,11);

						progress = format("%||/%||", build.task.get(), build.task.cap());
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
							FontTiny, ColorGray, ColorWhite,
							label
						);
					}
					else {
						render_text(win,
							button_pos, button_dim, {0.5,0.5},
							FontTiny, ColorWhite, ColorNone,
							label
						);
					}

					render_outline(win,
						button_pos, button_dim, ColorWhite, ly.S(1)
					);

					// progress ind
					render_text(win,
						build_pos, build_dim, {1, 0},
						FontTiny, ColorWhite, ColorBlack,
						progress
					);


					

					// left click on button -- select production
					con.on(Event::Press, Button::Left, button_pos, button_dim,
						[&env, &con, &colony, &build]() {
							show_select_makeable(env, con, colony, build,
								{ly.scr.pos, ly.scr.dim}, v2f{0.5f, 0.5f}
							);
						}
					);


				}



				// render build name
				if (con.sel_colony_slot_id == i) {

					render_text(win,
						build_pos, build_dim, {0.5, 0.5},
						FontTiny, ColorWhite, ColorBlack,
						build.get_name()
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
					auto& item_tex = res(win, ICON, get_item_icon_id(build.get_proditem()));
					auto item_dim = get_dim(item_tex);

					// item icon
					render_sprite(win, build_pos, item_tex);

					// number
					render_text(win,

						build_pos + v2s(item_dim[0], 0),
						v2s(0, item_dim[1]),
						{0, 0.5},

						FontTiny, ColorWhite, ColorBlack,
						std::to_string(y)
					);
				}


				// hover on building -- show label with building name (select buildplace)
				con.on(Event::Hover, build_pos, build_dim,
					[&con, i]() { con.sel_colony_slot_id = i; }
				);

				v2s units_frame = {ly.S(25), ly.S(16)};

				// units on build
				int n = build.units.size();
				int i = 0;
				for (auto& unit_ptr: build.units) {
					auto& unit = *unit_ptr;
					auto& unit_tex = res(win, ICON, get_icon_id(unit));

					v2s unit_pos = calc_align(
						{
							build_pos + build_dim - units_frame + ly.S(v2s(1,1)), units_frame
						},
						get_dim(unit_tex),
						v2f(float(i+1)/float(n+1), 1)
					);

					v2s unit_dim = get_dim(unit_tex);

					v2s sel_pos = unit_pos;
					v2s sel_dim = unit_dim;

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

			auto& tex_wood = res(win, WOODTILE, 1);
			render_area(win, ly.city_fields.pos, ly.city_fields.dim, tex_wood);

			auto city_terr_pos = calc_align(ly.city_fields.pos, ly.city_fields.dim, ly.terr_dim);

			auto& city_terr = terr;

			auto pos = env.get_coords(terr);
			auto city_coords = Coords(pos[0]-1, pos[1]-1);


			for(size_t field_id = 0; field_id < colony.fields.size(); ++field_id) {
				auto& field = colony.fields.at(field_id);

				auto& field_terr = field.get_terr();

				v2s relc = v2s(env.get_coords(*field.terr) - env.get_coords(city_terr));
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
						v2s unit_dim = v2s(field_dim[0]/2, field_dim[1]);
						v2s unit_pos = field_pos + v2s(field_dim[0]/2, 0);

						v2s item_dim = field_dim; //v2s(field_dim[0]/2, field_dim[1]);
						v2s item_pos = field_pos;

						v2s sel_pos = field_pos;
						v2s sel_dim = field_dim;

						auto& unit_tex = res(win, ICON, get_icon_id(unit));

						// field production
						if (auto proditem = field.get_proditem()) {
							// render produced item
							auto& item_tex = res(win, ICON, get_item_icon_id(proditem));
							render_sprite(win,
								calc_align({item_pos, item_dim}, get_dim(item_tex)),
								item_tex
							);

							// render produced item amount
							auto text = to_string(logic::get_nominal_prod(env, field, field.get_proditem()));

							render_text_at(win,
								item_pos,
								FontTiny, Style(ColorWhite, ColorBlack),
								text
							);
						}

						// render unit on field
						render_sprite(win,
							calc_align({unit_pos, unit_dim}, get_dim(unit_tex)),
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
									con.selected_id = field.get_proditem();
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



		// render city terr units

		// right click anywhere - unselect selected unit
		if (con.get_sel_unit()) {
			con.on(Event::Press, Button::Right,
				[&con]() {
					con.unselect_unit();
				}
			);
		}

		
		// left click on city terrain area with selected unit -- unwork that unit
		if (con.get_sel_unit()) {
			con.on2(Event::Press, Button::Left, ly.city_units.pos, ly.city_units.dim,
				"work-none"
			);
		}

		// render city terr units area bg
		render_fill(win,
			ly.city_units.pos, ly.city_units.dim,
			ColorSkyBlue
		);

		// render unit cargo area bg
		render_fill(win,
			ly.city_unit_cargo.pos, ly.city_unit_cargo.dim,
			ColorLightBrown
		);
		
		// render standing units
		int i = 0;
		for (auto& p: terr.units) {
			auto& unit = *p;

			if (unit.in_game and !unit.is_working()) {

				v2s unit_pos = ly.city_units.pos + v2s(ly.terr_dim[0] * i, 0);
				v2s unit_dim = ly.terr_dim;

				v2s sel_pos = unit_pos;
				v2s sel_dim = unit_dim;

				render_unit_tile(win, {unit_pos, unit_dim}, unit);
				
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

					// unit cargo
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
			ly.city_exit.pos + v2s(1,1),
			{0,0}, {0,0},
			FontTiny, ColorFont, ColorNone,
			"RET"
		);

		// left click on exit button -- exit city screen
		con.on(Event::Press, Button::Left, ly.city_exit.pos, ly.city_exit.dim,
			[&con, &terr](){ con.command("exit"); }
		);

		// render fields
		//render_fields(win, env, con, ly.city_fields.pos, );


		if (con.equip_to_unit_id) {
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

				auto prod = logic::get_nominal_prod(env, wp, item);

				if (prod) {
					keys.push_back(item);

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


	void render_units(Front &win, Console & con,
		v2s pos,
		v2s dim,
		Env const& env,
		Terr const& terr)
	{
		int i = 0;
		int j = 0;
		v2s unit_dim = ly.terr_dim;

		for (auto& p: terr.units) {
			auto& unit = *p;
			if (unit.in_game and !unit.is_working()) {

				auto& unit_tex = res(win, ICON, get_icon_id(unit));
				v2s unit_pos = pos + vmul(unit_dim, v2s(i,j));

				v2s sel_pos = unit_pos;
				v2s sel_dim = unit_dim;

				// render unit
				render_unit_tile(win, {unit_pos, unit_dim}, unit);


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
		for (auto dir: AllDirs) {
			arr[(dir)] = &get_terr_ext(env, p + dir2vec(dir));
		}
		return arr;
	}

	Terr const& get(array<Terr const*, 9> const& loc, Dir const& dir) {
		return *loc[(dir)];
	}

	int get_wxad_index(LocalArea const& loc, Phys const& phys) {
		int idx = 0;
		if ( get(loc, DirW).has_phys(phys) ) idx |= 8;
		if ( get(loc, DirX).has_phys(phys) ) idx |= 4;
		if ( get(loc, DirA).has_phys(phys) ) idx |= 2;
		if ( get(loc, DirD).has_phys(phys) ) idx |= 1;
		return idx;
	}

	int get_wxad_index_level(LocalArea const& loc, Alt const& lvl) {
		int idx = 0;
		if ( get(loc, DirW).get_alt() == lvl ) idx |= 8;
		if ( get(loc, DirX).get_alt() == lvl ) idx |= 4;
		if ( get(loc, DirA).get_alt() == lvl ) idx |= 2;
		if ( get(loc, DirD).get_alt() == lvl ) idx |= 1;
		return idx;
	}


	array<Dir,8> CLOCKWISE_DIRS = {
		DirW, DirE, DirD, DirC, DirX, DirZ, DirA, DirQ
	};

	bool looks_like_sea(Terr const& t) {
		return t.get_alt() == AltSea;
	}

	int get_coast_index(uint8_t const& l, Terr const& t0, Terr const& t1, Terr const& t2) {
		uint8 k =
			(!looks_like_sea(t2) << 2) +
			(!looks_like_sea(t1) << 1) +
			(!looks_like_sea(t0) << 0);
		return (k<<2) + l;
	}

	/*
	Biome get_render_biome(LocalArea const& loc) {
		auto b = get(loc, DirS).biome;

		if (is_water_biome(b)) {
			auto c = get(loc, DirA);
			if (!is_water_biome(c)) return c;

			c = get(loc, DirX);
			if (!is_water_biome(c)) return c;

			c = get(loc, DirD);
			if (!is_water_biome(c)) return c;

			c = get(loc, DirW);
			if (!is_water_biome(c)) return c;
		}
		return b;
	}*/

	void render_diffuse_from(Front &win,
		v2s pix,
		LocalArea const& loc,
		Dir d,
		int offset
	) {
		auto b = get(loc, d).biome;		
		if (b != BiomeNone) {
			render_sprite(win, pix, res(win, DIFFUSE, get_biome_icon_id(b) + offset) );	
		}
		else {
			// no diffuse from unexplored terrain (TODO: looks bad)
		}
	}



	//int const TextureMountainId;
	//int const TextureHillId;






	/// Render terrain: altitude, biome and physical features
	void render_terr(Front &win,
			v2s pos,
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
		render_sprite(win, pix, res(win, TERR, get_biome_icon_id(biome)));

		// render neighs pattern
		render_diffuse_from(win, pix, loc, DirW, 0);
		render_diffuse_from(win, pix, loc, DirD, 50);
		render_diffuse_from(win, pix, loc, DirX, 100);
		render_diffuse_from(win, pix, loc, DirA, 150);




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

			render_sprite(win, pix + v2s(0,0), res(win, COAST,
				base + get_coast_index(0, get(loc,DirA), get(loc,DirQ), get(loc,DirW))
			));

			render_sprite(win, pix + v2s(h,0), res(win, COAST,
				base + get_coast_index(1, get(loc,DirW), get(loc,DirE), get(loc,DirD))
			));

			render_sprite(win, pix + v2s(h,h), res(win, COAST,
				base + get_coast_index(2, get(loc,DirD), get(loc,DirC), get(loc,DirX))
			));

			render_sprite(win, pix + v2s(0,h), res(win, COAST,
				base + get_coast_index(3, get(loc,DirX), get(loc,DirZ), get(loc,DirA))
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

	void render_selected_unit(Front & win, Console & con, v2s pos, Unit & unit);



	void render_terr(Front &win,
			Coords const& pos,
			Env const& env,
			Terr const& terr,
			Vector2<int> const& map_pix,
			Coords const& delta)
	{

		int x = (pos[0] - delta[0]) * conf.terr_w + map_pix[0];
		int y = (pos[1] - delta[1]) * conf.terr_h + map_pix[1];

		render_terr(win, v2s(x,y), env, terr);

	}



	Color get_unit_color(Unit const& u) {
		auto c = get_nation_color(u.get_nation());
		if (u.get_time_left() == 0) {
			return Color(c.r/2, c.g/2, c.b/2, c.a);
		}
		return {c.r, c.g, c.b, c.a};
	}

	void render_stack(Front &win,
			Console & con,
			v2s pos,
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
			render_sprite(win, pos, res(win, ICON, 4));

			// left click on colony (on map) -- enter colony screen
			con.on(Event::Press, Button::Left, pos, v2s(conf.tile_dim, conf.tile_dim),
				[&con, coords](){
					con.select_terr(coords);
					con.command("enter");
				}
			);

			// colony flag
			if (auto p = env.get_control(terr)) {
				// render owner flag over colony (unit in garrison)
				render_sprite(win, {pos[0] + ly.S(5), pos[1]}, res(win, ICON, p->get_flag_id()));
			}

		}
		else if (terr.has_units() and !sel_unit_here) {
			// unit
			auto & unit = env.get_defender(terr);

			auto & icon = res(win, ICON, get_icon_id(unit));
			// render unit in field
			render_shield(win, pos, get_unit_color(unit), con.get_letter(unit));
			render_sprite(win,
				calc_align({pos, ly.terr_dim}, get_dim(icon)),
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


	/// Return res num assigned to this dir
	ResNum dir_res_num(Dir dir) {
		switch (dir) {
			case DirQ: return 8;
			case DirW: return 1;
			case DirE: return 2;
			case DirA: return 7;
			
			case DirD: return 3;
			case DirZ: return 6;
			case DirX: return 5;
			case DirC: return 4;
		}
		assert(false);
	}

	void handle_selected_unit(Front & win, Console & con, Unit & unit) 
	{
		auto unit_id = unit.id;
	
		// selected unit keyboard shortcuts 
		
		// numpad move
		con.on(Event::Press, KeyNumpad2,
			[&con,unit_id](){ con.command("move 0 1"); }
		);
		con.on(Event::Press, KeyNumpad8,
			[&con,unit_id](){ con.command("move 0 -1"); }
		);
		con.on(Event::Press, KeyNumpad6,
			[&con,unit_id](){ con.command("move 1 0"); }
		);
		con.on(Event::Press, KeyNumpad4,
			[&con,unit_id](){ con.command("move -1 0"); }
		);
		con.on(Event::Press, KeyNumpad3,
			[&con,unit_id](){ con.command("move 1 1"); }
		);
		con.on(Event::Press, KeyNumpad7,
			[&con,unit_id](){ con.command("move -1 -1"); }
		);
		con.on(Event::Press, KeyNumpad1,
			[&con,unit_id](){ con.command("move -1 1"); }
		);
		con.on(Event::Press, KeyNumpad9,
			[&con,unit_id](){ con.command("move 1 -1"); }
		);

		// alpha move
		con.on(Event::Press, KeyX,
			[&con,unit_id](){ con.command("move 0 1"); }
		);
		con.on(Event::Press, KeyW,
			[&con,unit_id](){ con.command("move 0 -1"); }
		);
		con.on(Event::Press, KeyD,
			[&con,unit_id](){ con.command("move 1 0"); }
		);
		con.on(Event::Press, KeyA,
			[&con,unit_id](){ con.command("move -1 0"); }
		);
		con.on(Event::Press, KeyC,
			[&con,unit_id](){ con.command("move 1 1"); }
		);
		con.on(Event::Press, KeyQ,
			[&con,unit_id](){ con.command("move -1 -1"); }
		);
		con.on(Event::Press, KeyZ,
			[&con,unit_id](){ con.command("move -1 1"); }
		);
		con.on(Event::Press, KeyE,
			[&con,unit_id](){ con.command("move 1 -1"); }
		);

		// build colony
		con.on(Event::Press, KeyB,
			[&con](){ con.command("build-colony Unnamed"); }
		);
		// build road
		con.on(Event::Press, KeyR,
			[&con,unit_id](){ con.cmd_unit(unit_id, Cmd(InsRoad)); }
		);
		// plow fields
		con.on(Event::Press, KeyP,
			[&con,unit_id](){ con.cmd_unit(unit_id, Cmd(InsPlow)); }
		);
		// clear forest
		con.on(Event::Press, KeyO,
			[&con,unit_id](){ con.cmd_unit(unit_id, Cmd(InsClear)); }
		);
		// space - skip unit
		con.on(Event::Press, KeySpace,
			[&con,unit_id](){ con.cmd_unit(unit_id, Cmd(InsWait, 1)); }
		);
		
		// esc - clear orders
		con.on(Event::Press, KeyEsc,
			[&con,unit_id](){ 
				con.clear_unit_cmds(unit_id); 
				con.set_input_mode(con.InputModeDefault);
			}
		);
		
		// press enter -- execute current orders
		con.on(Event::Press, KeyEnter,
			[&con](){
				if (auto * u = con.get_sel_unit_ext()) {
					con.exec_unit_cmds(*u);
					con.select_next_unit();
				}
				con.set_input_mode(con.InputModeDefault);
			}
		);
		
		// M - enter move mode
		con.on(Event::Press, KeyM,
			[&con](){ con.set_input_mode(con.InputModeMove); }
		);
		
		

	}

	void render_selected_unit(Front & win, Console & con, v2s pos, Unit & unit) {
		auto & icon = res(win, ICON, get_icon_id(unit));
		auto unit_id = unit.id;

		// render blinking shield
		if (con.blink()) {
			render_shield(win, pos, get_unit_color(unit), con.get_letter(unit));
		}

		// render unit in field
		render_sprite(win,
			calc_align({pos, ly.terr_dim}, get_dim(icon)),
			icon
		);


		
	
	}

	//void select_area()

	void render_fog(Front & win, v2s pos) {
		win.render_fill({pos,ly.terr_dim}, Color(64,64,64,64));
	}

	void render_unknown(Front & win, v2s pos) {
		render_sprite(win, pos, res(win, PHYS, 149));
	}



	
	
	bool is_tile_on_screen(Console & con, Coords xy) 
	{
		return overlap(con.get_view_box(), xy);			
	}
	
	b2s get_tile_on_screen(Console & con, Coords xy)
	{
		auto rpos = ly.map.pos + vmul(ly.terr_dim, xy - con.get_view_pos());
		return b2s(rpos, ly.terr_dim);
	}
	
	void render_on_tile(Front & win, Console & con, Coords xy, Texture const& tex) 
	{
		if (is_tile_on_screen(con, xy)) {
			auto rpos = get_tile_on_screen(con, xy).pos;
			win.render(tex, rpos);
		}
	}
	
	
	
	
	void render_cmds(Console & con, v2c unit_pos, Cmds const& cmds) 
	{		
		auto pos = unit_pos;
		
		for (auto i = cmds.size(); i > 0; --i) {
			auto cmd = cmds.at(i-1);
		
			switch (cmd.ins) {
				case InsMove: {
					Dir dir = cmd.arg;
				
					render_on_tile(con.win, con, pos, 
						res(con.win, ZZ, dir_res_num(dir))
					);
					
					pos += dir2vec(dir);
					
					render_on_tile(con.win, con, pos, 
						res(con.win, ZZ, dir_res_num(turnabout(dir)))
					);
				
					break;
				}					
			}
		}
	
	}
	
	
	void render_marks(Console & con, Marks const& marks) {
		for (auto & m: marks.list()) {
			if (is_tile_on_screen(con, m.pos)) 
			{
				auto box = get_tile_on_screen(con, m.pos);
				
				render_text(con.win,
					box.pos, box.dim, v2f(0.5f, 0.5f),
					FontTiny, ColorBlue, ColorBlack,
					m.label	
				);
			}
		}
	}
	
	void render_debug_ai(Console & con) {
		if (con.runner) 
		{
			auto & win = con.win;
			
			for (Agent * a: con.runner->agents) 
			{
				auto cls = a->get_class();
				if (cls == simple_ai::ClassSimpleAi) 
				{
					auto * b = static_cast<simple_ai::SimpleAi*>(a);
					render_marks(con, b->marks);
				}			
			}
		}
	}
	
	
	

	
	void render_map(Front & win, Env const& env, Console & con, v2s pos,
			Coords const& delta)
	{
		auto w = con.view_dim[0];
		auto h = con.view_dim[1];
		
		// left arrow -- scroll map
		con.on(Event::Press, KeyLeft, 
			[&con](){
				con.move_view({-1,0});
			}
		);

		// right arrow -- scroll map
		con.on(Event::Press, KeyRight, 
			[&con](){
				con.move_view({+1,0});
			}
		);

		// up arrow -- scroll map
		con.on(Event::Press, KeyUp, 
			[&con](){
				con.move_view({0,-1});
			}
		);

		// down arrow -- scroll map
		con.on(Event::Press, KeyDown, 
			[&con](){
				con.move_view({0,+1});
			}
		);

		auto vpos = con.view_pos;
		auto vdim = v2s(w,h);

		auto view_box = ext::b2<Coord>(con.view_pos, con.view_dim);
		
		
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
		
				auto coords = Coords(i,j) + vpos;

				if (env.in_bounds(coords)) {
				
					auto pos = ly.map.pos + vmul(ly.terr_dim, v2s(i,j));

					auto& terr = env.get_terr(coords);


					if (con.is_discovered(terr)) {
						render_terr(win, pos, env, terr);
					}
					else {
						render_unknown(win, pos);
					}

					auto dim = ly.terr_dim;

					// right click on terr -- center view
					con.on(Event::Press, Button::Right, pos, dim,
						[&con,coords](){
							con.center_view_on(coords);
						}
					);

					if (con.get_input_mode() == con.InputModeDefault) {						
						// left press on terr -- select terr					
						con.on(Event::Press, Button::Left, pos, ly.terr_dim,
							[&con,coords](){
								con.select_terr(coords);
							}
						);

						// left press on terr with shift -- add select terr
						con.on(Event::Press, Button::Left, halo::ModButtonLeft|halo::ModShift, pos, ly.terr_dim,
							[&con,coords](){
								con.select_terr(coords, +1);
							}
						);

						// left press on terr with ctrl -- sub select terr
						con.on(Event::Press, Button::Left, halo::ModButtonLeft|halo::ModCtrl, pos, ly.terr_dim,
							[&con,coords](){
								con.select_terr(coords, -1);
							}
						);

						// hover with button down over terr -- select set
						con.on(Event::Hover, halo::ModButtonLeft, pos, ly.terr_dim,
							[&con,coords](){
								con.select_terr(coords);
							}
						);

						// hover with button down with shift over terr -- select add
						con.on(Event::Hover, halo::ModButtonLeft|halo::ModShift, pos, ly.terr_dim,
							[&con,coords](){
								con.select_terr(coords, +1);
							}
						);

						// hover with button down with ctrl over terr -- select sub
						con.on(Event::Hover, halo::ModButtonLeft|halo::ModCtrl, pos, ly.terr_dim,
							[&con,coords](){
								con.select_terr(coords, -1);
							}
						);
					}
					else if (con.get_input_mode() == con.InputModeMove) 
					{
						// left press on terr -- show path
						con.on(Event::Press, Button::Left, pos, ly.terr_dim,
							[&con,coords](){
								if (auto * u = con.get_sel_unit_ext()) {																		
									auto path = con.find_path(con.get_coords(*u), coords, *u);
									u->cmds.clear();
									if (path.exists()) {
										u->cmds.adds(path.cmds);
									}				
								}
							}
						);
						
					}


				}


			}
		}

		// render unit path/commands
		if (auto * u = con.get_sel_unit_ext()) 
		{			
			// unit is controlled by human or AI
			// editing mode -> show unit command
			// playing mode -> show only if ours
			
			/*auto * agn = con.get_agent(u->id);
			
			if (con.editing or agn == &con) {
				auto const& cmds = get_unit_cmds(agn, u->id);
			
				render_cmds(con, con.get_coords(*u), cmds);
			}*/
			render_cmds(con, con.get_coords(*u), u->cmds);
		}
		
		// stacks on map
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				auto coords = Coords(i,j) + vpos;
				if (env.in_bounds(coords)) {

					auto pos = ly.map.pos + vmul(ly.terr_dim, v2s(i,j));

					auto& terr = env.get_terr(coords);

					if (con.has_vision(terr)) {
						render_stack(win, con, pos, env, terr);
					}
					else {
						if (con.is_discovered(terr)) {
							render_fog(win, pos);
						}
					}

				}
			}
		}
	
		if (auto *u = con.get_sel_unit()) 
		{
			handle_selected_unit(win, con, *u);
		}

		// render cursor on selected tile
		if (not con.get_sel_unit()) {
			if (Terr* tp = con.get_sel_terr()) {

				for (auto tp: con.get_sel_terrs()) {
					auto coords = env.get_coords(*tp);
					
				
					if (overlap(view_box, coords)) 
					{
						render_inline(win,
							vmul(coords - vpos, ly.terr_dim) + pos,
							ly.terr_dim,
							{128,128,128,255},
							ly.line
						);
					}
				}

				auto coords = env.get_coords(*tp);
				
				// blink main terr
				if ((con.time % 1000) >= 500) {
					// rect frame on tile
					if (overlap(view_box, coords)) {
						render_inline(win,
							vmul(coords - vpos, ly.terr_dim) + pos,
							ly.terr_dim,
							{255,255,255,255},
							ly.line
						);
					}
				}


			}
		}
		
		if (con.editing) {
			render_debug_ai(con);
		}
		
		
		
	}
	
	

	void render_bar(Front &win,
		Env const& env,
		Console const& con,
		v2s pos,
		v2s dim
	) {
		// pos - left top pix
		// dim - size

		
		

		render_area(win, pos, dim, res(win, WOODTILE, 1));

	}

	void finish_popup(Console & con, Select & s) 
	{
		s.oncancel = [&con](){
			con.clear_widget();
		};   // oncancel
		
		s.onselect = [&con](){
			con.clear_widget();
		};   // onselect
				
		s.set_geo({ly.scr.pos, ly.scr.dim}, v2f(0.5f, 0.5f));
	}
	
	void finish_popup_at(Console & con, Select & s, v2s pos) 
	{
		s.oncancel = [&con](){
			con.clear_widget();
		};   // oncancel
		
		s.onselect = [&con](){
			con.clear_widget();
		};   // onselect
				
		s.set_geo({pos, v2s(0,0)}, v2f(0.0f, 0.0f));
	}

	void show_select_biome(Env const& env, Console & con, Terr const& terr) {
		auto current = terr.get_biome();
		auto & s = con.replace_widget<Select>();
		
		for (auto i = BiomeNone+1; i < BiomeEnd; ++i)
		{
			s.add({get_biome_name(i)}, i == current, [&con, i](){
				con.set_biome(i);
			});			
		}
		
		finish_popup(con, s);
	}
	
	void show_select_alt(Env const& env, Console & con, Terr const& terr) {
		auto current = terr.get_alt();
		auto & s = con.replace_widget<Select>();
		
		for (auto i = AltNone+1; i < AltEnd; ++i)
		{
			s.add({get_alt_name(i)}, i == current, [&con, i](){
				con.set_alt(i);
			});			
		}
		
		finish_popup(con, s);
	}
	
	void show_select_river(Console & con, Terr & terr) {
		auto current = terr.get_phys(PhysRiver);
		auto & s = con.replace_widget<Select>();
		
		auto lst = std::array<Phys,3>{PhysNone, PhysMinorRiver, PhysMajorRiver};
		
		for (auto i: lst)
		{
			s.add({get_phys_name(i)}, i == current, [&con, i](){
				con.set_phys(PhysRiver, i);
			});			
		}
		
		finish_popup(con, s);
	}
	
	void show_select_flora(Console & con, Terr & terr) {
		auto current = terr.get_phys(PhysForestPlow);
		auto & s = con.replace_widget<Select>();
		
		auto lst = std::array<Phys,3>{PhysNone, PhysForest, PhysPlow};
		
		for (auto i: lst)
		{
			s.add({get_phys_name(i)}, i == current, [&con, i](){
				con.set_phys(PhysForestPlow, i);
			});			
		}
		
		finish_popup(con, s);
	}



	void show_select_unit_type(Env const& env, Console & con) {
		auto & s = con.replace_widget<Select>();
		auto * u = con.get_sel_unit();
		
		
		for (auto i = UnitTypeNone + 1; i < UnitTypeEnd; ++i) 
		{
			// TODO remove this - select should auto page
			// skip to save space; menu too long with this
			if (i == UnitTypePioneers20) {
				i = UnitTypePioneers100;
			}
		
			auto & type = env.get<UnitType>(i);
			
			s.add({type.get_name()}, i == u->get_type().id, [u, &type](){ 
				u->set_type(type);
			});
		}
		
		finish_popup(con, s);
	}

	void show_select_speciality(Console & con) {
		
		auto & s = con.replace_widget<Select>();
		
		auto * u = con.get_sel_unit();
		
		for (int i = ProfNone; i < ProfEnd; ++i)
		{
			s.add({get_prof_name(i)}, i == u->get_prof(), [u, i](){ 
				u->set_prof(i);				
			});
		}
		
		finish_popup(con, s);
	}


	/*struct Node 
	{		
		virtual v2s get_dim() const {}
		virtual bool is_selectable() const { return false; }
		
		virtual Node * get_right() {}
		virtual Node * get_up() {}
		virtual Node * get_down() {}
		
		virtual void set_right(Node *) {}
		virtual void set_up(Node *) {}
		virtual void set_down(Node *) {}
		
		virtual void render(Front & win, Console & con) {}
		
		virtual ~Node() {}
	};
	
	//Node * right_{nullptr}, 
	//Node * up_{nullptr}, 
	//Node * down_{nullptr};
	
	struct Cell: Node {
	
	}
	
	struct Text: Node {
		
	
	
		string text;
	}*/
	



	struct Menu : Widget {
		int cursor{-1};
		void render(Front & win, Console & con) override;
			
		int size() { return 2; }
				
		void move(size_t i) {
			cursor = i;
			if (cursor <= 0) { cursor = size(); }
			if (cursor >= size()) { cursor = 0; }
		}

		void move_up() {
			if (cursor <= 0) { cursor = size(); }
			cursor -= 1;
		}

		void move_down() {
			cursor += 1;
			if (cursor >= size()) { cursor = 0; }
		}		
	};

	void Menu::render(Front & win, Console & con) {
		/*TextRend2 cur(win, con, ly.bar, PixFont, StyleMenu);
		
		cur.link("Reports")
		
		Game   Unit   Reports   BlaBla
		*/
		
		
		
		
		
		/*
		
		auto cind = 0;
		
		auto box = ly.bar;		
		
		v2s cpos = box.pos;
		v2s cdim = {120, box.dim[1]};
		
		if (cind == cursor) { win.render_fill(win, cpos, cdim, ColorSelectedBG); }		
		render_text(win, cpos, cdim, {0,0}, PixFont, ColorFont, ColorNone, "Game");		
		con.on
		cpos[0] += cdim[0];
		
		if (cind == cursor) { win.render_fill(win, cpos, cdim, ColorSelectedBG); }
		render_text(win, cpos, cdim, {0,0}, PixFont, ColorFont, ColorNone, "Reports");
		cpos[0] += cdim[0];
		*/
		
		
		
		
	}

	void render_nations_table() {
		/*
		cur.set_tabs(160, 100, 40);
		cur.set_align(0, 0, 0);
		cur.text("Name").td().text("Power").td().text("Bla").tr();
		
		cur.set_align(-1, +1, 0);
		cur.link("England", con.editing, [](){ }).td();
		cur.text("88").td();
		cur.text("bla").tr();
		
		cur.link("England", con.editing, [](){ }).tab();
		*/
		
		
	
	}

	void show_select_nation(Console & con) {
		auto & s = con.replace_widget<Select>();
		
		for (auto & nation: list_values(con.env.nations)) 
		{			
			auto id = nation.id;
			s.add({nation.get_name()}, id == con.nation_id, [&con, id](){
				con.set_nation_id(id);
			});			
		}		
		finish_popup(con, s);	
	}

	void render_panel(Front & win,
			Env const& env,
			Console & con,
			v2s pos,
			v2s dim
		) {

		//front::Image img = res("COLONIZE/WOODTILE_SS", 1);  32x24

		con.on(Event::Press, KeyF12,
			[&con](){ 
				con.editing = !con.editing; 
				con.put("EDITING = %||", int(con.editing));
			}
		);
	
		

		render_area(win, pos, dim, res(win, WOODTILE, 1));

		// terrain info
		/*
		 * Biome: Plains
		 * road
		 */

		

		string nation_name = "EDITING";
		if (env.in_progress()) {
			nation_name = env.get_current_nation().name;
		}

		auto cur = TextRend2(win, con, ly.pan, FontTiny, StyleMenu);

		// Turn 5, England
		cur.text("Turn " + to_string(env.get_turn_no()) + ", " + nation_name + "\n");

		if (con.nation_id) 
		{
			cur.text("You are: ");
			cur.link(env.get_nation_name(con.nation_id), con.editing, [&con](){
				show_select_nation(con);
			});
			cur.text("\n");			
		}



		if (Terr * tp = con.get_sel_terr()) 
		{
			auto& t = *tp;
			
			if (con.is_discovered(t))
			{				
				string phys_info;
				for (auto const& item: phys_names) {
					auto const& phys = item.first;
					auto const& name = string(item.second);

					if (t.has_phys(phys)) phys_info += name + ",";
				}
				
				
				cur.text("Biome: ");
				cur.link(get_biome_name(t.biome), con.editing, 
					[&con, &env, &t](){
						show_select_biome(env, con, t);
					}
				);
				cur.text("\n");
				
				cur.text("Alt: ");
				cur.link(get_alt_name(t.get_alt()), con.editing, 
					[&con, &env, &t](){
						show_select_alt(env, con, t);
					}
				);
				cur.text("\n");
				
				
				//cur.render(Textformat("[%||]\n", phys_info));
				
				auto river = t.get_phys(PhysRiver);
				auto forest = t.get_phys(PhysForest);
				auto plow = t.get_phys(PhysPlow);
				auto road = t.get_phys(PhysRoad);
				
				auto get_river_name = [](Phys river){
					switch (river) {						
						case PhysMinorRiver: return "MiRi";
						case PhysMajorRiver: return "MaRi";
					}
					return "NoRi";
				};
				
				cur.text("(");
				cur.link(get_river_name(river), con.editing,
					[&con,river](){
						con.set_phys(PhysRiver, next_phys(PhysRiver, river));
					}
				);				
				cur.text(",");
				
				cur.link((forest ? "Fore" : "NoFo"), con.editing,
					[&con,forest](){
						con.set_phys(PhysForest, next_phys(PhysForest, forest));
					}
				);				
				cur.text(",");
				
				cur.link((plow ? "Plow" : "NoPl"), con.editing,
					[&con,plow](){
						con.set_phys(PhysPlow, next_phys(PhysPlow, plow));
					}
				);			
				cur.text(",");
				
				
				cur.link((road ? "Road" : "NoRo"), con.editing,
					[&con, road](){
						con.set_phys(PhysRoad, next_phys(PhysRoad, road));
					}
				);
				cur.text(")\n");
			
			
			}
			else {
				cur.text(format("Biome: %||\n", "Unknown"));			
				cur.text(format("Alt: %||\n", "Unknown"));
				cur.text(format("(%||)\n", "Unknown"));				
			}
			
		}

		cur.text("\n");

		if (auto u = con.get_sel_unit()) {
			
			cur.link(u->get_name(), con.editing, 
			[&env, &con](){
				show_select_unit_type(env, con);
			});
			
			cur.text("(");			
			cur.link(get_prof_name(u->get_prof()), con.editing,
			[&con](){
				show_select_speciality(con);
			});						
			cur.text(")\n");
			
			cur.text(format("Time left: %||/%||\n",
				int(u->time_left),
				int(TIME_UNIT)
			));

			cur.text(format("moves: %||\n", get_moves(*u)));
			cur.text(format("transported: %||\n", u->get_transported()));
			cur.text(format("space left: %||\n", u->get_space_left()));

			if (u->get_item1() == ItemTools) {
				cur.text(format("tools: %||\n", u->get_num1()));
			}

		}

		//
		// altitude
		// biome
		// feats


		// top right: nation indicator
		if (env.in_progress()) {
			
			render_fill(win,
				pos + v2s(dim[0]-ly.S(1),0),
				v2s(ly.S(1), ly.S(1)),
				get_nation_color(env.get_current_nation())
			);
		}

		//auto ren = Renderer(win, env);



		if (auto t = con.get_sel_terr()) {
			v2s pos = ly.pan.pos + v2s(0,ly.S(100));
			v2s dim = v2s(ly.pan.dim[0], ly.terr_dim[1]*3);

			render_fill(win, pos, dim,
				Color(0,0,0,64)
			);

			render_units(win, con, pos, dim,
				env,
				*t
			);
		}

		
		// Idle Unit
		{
			if (env.in_progress()) {
				
				auto& p = env.get_current_nation();
				
				auto colind = (con.get_next_to_order(nullptr)) ? ColorGray : ColorWhite;
				
				// show button
				auto text_box = render_text(win,
					ly.pan.pos, ly.pan.dim - v2s(0, ly.font_tiny * 2), v2f(0.5, 1.0),
					FontTiny, colind, ColorNone,
					"Idle Unit"
				);

				// left click -> select next idle
				con.on(Event::Press, Button::Left, text_box.pos, text_box.dim,
					[&con](){ con.select_next_unit(); }
				);
				
			}
		}
		
		
		// Idle Factory
		{
			if (env.in_progress()) {
				
				auto& p = env.get_current_nation();
			
				auto colind = ColorGray;
			
				auto* terr = logic::get_idle_colony(env, p);
				if (terr) {
					colind = ColorWhite;
				}
				
				// show button
				auto text_box = render_text(win,
					ly.pan.pos, ly.pan.dim - v2s(0, ly.font_tiny), v2f(0.5, 1.0),
					FontTiny, colind, ColorNone,
					"Idle Factory"
				);


				// left click -> select idle
				con.on(Event::Press, Button::Left, text_box.pos, text_box.dim,
					[&con,terr](){ con.select_terr(terr); con.mode = Console::Mode::COLONY; }
				);
				
			}
		}
		
		// End of turn/Start game
		// nothing to move -> enter to ready turn
		{
			auto lab = string();
			auto cmd = string();
			auto colind = ColorRed;

			if (env.in_progress()) {
				auto& p = env.get_current_nation();

				if (con.get_next_to_repeat(nullptr)) {
					lab = "Move all";
					cmd = "move-all";
					colind = ColorWhite;

					
					if (con.get_input_mode() == con.InputModeDefault) 
					{
						// Ctrl + Enter -> move all
						/*con.on(Event::Press, halo::ModCtrl, KeyEnter,
							[&con](){ 						
								print("to repeat: %||\n", con.get_next_to_repeat(nullptr)->id);
								con.repeat_all(); 
							}
						);*/
					}
				}
				else {
					lab = "End of turn";
					cmd = "ready";
					colind = (con.get_next_to_order(nullptr)) ? ColorGray : ColorWhite;

					
					if (con.get_input_mode() == con.InputModeDefault) 
					{
						// Ctrl + Enter -> end turn	
						/*con.on(Event::Press, halo::ModCtrl, KeyEnter,
							[&con](){ con.command("ready"); }
						);*/
					}
				}
			}
			else {
				lab = "Start game";
				cmd = "start";				
				colind = (env.nations.size() < 1) ? ColorGray : ColorWhite;
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









	void render_cmd(Front & app, col::Console & con) {

		auto ln = con.output.size();
		v2s pos = v2s(ly.map.pos[0], ly.map.pos[1]);

		for (auto& line: boost::adaptors::reverse(con.output)) {


			auto text_box = render_text(app,
				pos, {0,0}, {0,0},
				FontTiny, ColorFont, {0,0,0,128},
				line
			);

			pos[1] += text_box.dim[1];
		}

		// command line with cursor
		render_text(app,
			ly.scr.pos, {0,0}, {0,0},
			FontTiny, ColorFont, ColorNone,
			con.buffer + "_"
		);

		// trap all keyboard
		con.on(Dev::Keyboard);

		// char entered
		con.on(Event::Char, [&con](halo::Pattern const& p){
			con.handle_char(*p.unicode);
		});

		// history up
		con.on(Event::Press, KeyUp, [&con](){
			con.history_up();
		});

		// history down
		con.on(Event::Press, KeyDown, [&con](){
			con.history_down();
		});

		// enter
		con.on(Event::Press, KeyReturn, [&con](){
			con.handle_char(u'\r');
		});

		// backspace
		con.on(Event::Press, KeyBackspace, [&con](){
			con.handle_char(u'\b');
		});




		
	}




	void render_cursor(Front & win, Console const& con) {
		if (con.drag_item) {

			auto mouse = front::get_mouse();
			auto pos = v2s(get_logical_pos(win, mouse.pos));
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

	void show_order_menu(Console & con, v2s pos) 
	{	
		auto & s = con.replace_widget<Select>();

		
		s.add({"Move (M)"}, [&con](){
			if (con.has_sel_unit()) {
				con.set_input_mode(con.InputModeMove);
			}
		});
		
		s.add({"Exec (Enter)"}, [&con](){
			if (auto * u = con.get_sel_unit_ext()) {
				con.exec_unit_cmds(*u);
				con.select_next_unit();
			}
		});
		
		s.add({"Clear (Esc)"}, [&con](){
			if (con.has_sel_unit()) {
				con.clear_unit_cmds(con.get_sel_unit_id());
			}
		});
		
		
		finish_popup_at(con, s, pos);	
	}
	
	
	void render_menu(Console & con) 
	{
		auto cur = TextRend2(con.win, con, ly.bar, FontTiny, StyleMenu);

		auto pos = cur.get_pos() + v2s(0, cur.get_height() + ly.line);		

		cur.link("Order", true, [&con, pos](){
			show_order_menu(con, pos);
		});

	}
	


	void render(Front & app, col::Env & env, col::Console & con, int verbose) {
	
		con.sync_widget();
	
		if (verbose >= 2) print("render: {\n");

		app.clear();

		con.reset_hotspots();

		int w = app.get_ctx_dim()[0];
		int h = app.get_ctx_dim()[1];


		render_drag_clear(con);


		// bar top
		render_bar(app, env, con, ly.bar.pos, ly.bar.dim);

		// hline under the bar
		render_fill(app,
			{ly.bar.pos[0], ly.bar.end()[1]},
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


		if (con.is_active()) {

			render_cmd(app, con);

			// deactivate terminal
			con.on(Event::Char, u'`', [&con](){
				con.set_active(false);
			});

		}
		else {
			render_menu(con);
		
			// activate terminal
			con.on(Event::Char, u'`', [&con](){
				con.set_active(true);
			});
		}

		
		

		

		render_cursor(app, con);


		// render current widget
		if (con.widget) {
			con.widget->render(app, con);
		}

		//app.render_line(ly.scr.pos, ly.scr.end(), ColorRed, 5);


		if (verbose >= 2) print("render: app.flip\n");

		app.flip();

		if (verbose >= 2) print("render: }\n");
	}





}
