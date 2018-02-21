#include "renderer.hpp"

//#include <functional>
#include <boost/range/adaptor/reversed.hpp>

#include "base.hpp"
#include "logic.hpp"
#include "props.hpp"
#include "conf.hpp"
#include "align.hpp"
#include "hypertext.hpp"
#include "layout.hpp"
#include "simple_ai.hpp"
#include "coldef.hpp"
#include "widget.hpp"

namespace col {
	using Con = Console;



	Style const StyleMenu(ColorFont, ColorNone, ColorFontSelected, ColorFontDisabled);


	bool const VERBOSE = 1;




	ResId get_prof_icon_id(Prof prof, ResId def)
	{
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


	ResId get_icon_id(Unit const& unit) {
		ResId const IconErrorId = 56;

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


	/// Render border inside box
	/// t -- thickness
	void render_inline(b2s box, RGBA8 color, int8_t t)
	{
		auto wh = box.dim - v2s(t,t);
		auto w = wh[0];
		auto h = wh[1];

		// top
		render_fill({box.pos + v2s{0,0}, {w,t}}, color);
		// right
		render_fill({box.pos + v2s{w,0}, {t,h}}, color);
		// bottom
		render_fill({box.pos + v2s{t,h}, {w,t}}, color);
		// left
		render_fill({box.pos + v2s{0,t}, {t,h}}, color);
	}

	void render_outline(b2s box, RGBA8 color, int8_t t) {
		/* Draw 1px thick outline border of pos,dim box
		*/
		render_inline({box.pos - v2s(t,t), box.dim + v2s(t,t) * _2}, color, t);
	}

	Sprite make_subsprite(Sprite const& o, b2s sub)
	{
		auto u = ext::vdiv(o.box.dim(), v2f(o.dim));
		Sprite s;
		s.dim = sub.dim;
		s.box.pos = o.box.pos + v2s(ext::vmul(u, v2f(sub.pos)));
		s.box.end = s.box.pos + v2s(ext::vmul(u, v2f(sub.dim)));
		return s;
	}

	/// Make quarter tile
	Sprite make_subtile(int8_t sub_id, ResId terrain_id)
	{
		auto const& o = res(terrain_id);
		
		auto h = (o.box.end[0] - o.box.pos[0]) / 2.0f;
		auto x = o.box.pos[0];
		auto y = o.box.pos[1];

		// subtile numbering: 0=NW, 1=NE, 2=SE, 3=SW
		
		v2f p;
		switch (sub_id)
		{
		case 0:			
			p = v2f(x,y);
			break;
		case 1:
			p = v2f(x+h,y);
			break;
		case 2:
			p = v2f(x+h,y+h);
			break;
		case 3:
			p = v2f(x,y+h);			
			break;
		default:
			ext::fail("invalid subtile_id\n");
		}

		Sprite s;		
		s.dim = v2s(o.dim[0]/2, o.dim[1]/2); // pixel units
		s.box.pos = p; // relative float units
		s.box.end = p + v2f(h,h); // relative float units
		return s;
	}


	inline bool is_water(Terr const& t) {
		return t.alt == AltSea;
	}


	RGBA8 get_control_color(Control cc)
	{
		switch (cc) {
			case ControlEnglandCol: return ColorRed;
			case ControlFranceCol: return ColorBlue;
			case ControlSpainCol: return ColorYellow;
			case ControlNetherlandsCol: return ColorOrange;
		}
		return ColorBrown;
	}

	void render_terr(
			v2s pos,
			Env const& env,
			Terr const& terr);



	void render_shield(v2s pos, RGBA8 color, char letter)
	{
		auto inner_pos = pos + v2s(1,1) * ly.line;
		auto inner_dim = v2s(ly.S(5), ly.font_tiny);

		render_outline(
			{inner_pos, inner_dim},
			{0, 0, 0, 255},
			ly.line
		);

		render_fill(
			{inner_pos, inner_dim},
			color
		);

		render_text(
			pos + v2s(1,1) * ly.line,
			font_tiny(), ColorBlack,
			string() + letter
		);
	}


	void render_pixel(v2s pos, RGBA8 color)
	{
		render_fill(
			{pos, v2s(ly.S(1), ly.S(1))},
			color
		);
	}

	
	void render_sprite(b2s box, v2f const& align, Sprite const& spr)
	{
		render_sprite(calc_align(box, spr.dim, align), spr);
	}



	void render_area(b2s box, Sprite const& spr)
	{
		auto tile_dim = spr.dim;
		auto area_pos = box.pos;
		auto area_dim = box.dim;
		auto area_end = box.end();

		int16_t ei,ej;

		assert(tile_dim[0] > 0);
		assert(tile_dim[1] > 0);

		// center
		{
			int16_t i,j;
			j = area_pos[1];
			while (j < area_end[1] - tile_dim[1])
			{
				i = area_pos[0];
				while (i < area_end[0] - tile_dim[0])
				{
					//print("render_sprite i j %|| %|| d %|| %||\n", i, j, tile_dim[0], tile_dim[1]);
					render_sprite({i,j}, spr);
					i += tile_dim[0];
				}
				j += tile_dim[1];
			}

			ei = i;
			ej = j;
		}


		// bottom
		{
			auto src_box = b2s({0, 0}, v2s(tile_dim[0], area_end[1] - ej));

			int16_t i = area_pos[0], j = ej;
			while (i < area_end[0] - tile_dim[0]) {
				render_sprite({i, j}, make_subsprite(spr, src_box));
				i += tile_dim[0];
			}
			ei = i;
		}

		// right
		{
			auto src_box = b2s({0, 0}, {int16_t(area_end[0] - ei), tile_dim[1]});
			int16_t i = ei, j = area_pos[1];
			while (j < area_end[1] - tile_dim[1]) {
				render_sprite({i, j}, make_subsprite(spr, src_box));
				j += tile_dim[1];
			}
			ej = j;
		}

		// corner
		{
			auto src_box = b2s({0, 0}, {int16_t(area_end[0] - ei), int16_t(area_end[1] - ej)});
			int16_t i = ei, j = ej;
			render_sprite({i, j}, make_subsprite(spr, src_box));
		}

	}

	//Color Black = {0,0,0,255};
	//Color Transparent = {0,0,0,0};


	/*
	static Res::mapped_type const* g_cursor_tex = nullptr;
	void set_cursor(Res::mapped_type const* tex = nullptr) {

		g_cursor_tex = tex;
	}*/

	struct Col
	{
		float align{0};    // 0 - left, 1 - right, 0.5 - center
		int width{0};
		string name{""};

		Col(float a, int w, string n):
			name(n), width(w), align(a) {}

		Col(float a, int w):
			width(w), align(a) {}

		Col(float a):
			align(a) {}

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
		uint8_t flags; // bit 0 set -> highlight

		Elem() {}
		Elem(vector<string> const& text, Action const& onselect, Key shortcut):
			text(text),
			onselect(onselect),
			shortcut(shortcut)
		{}
	};







	/*struct Frame: Widget
	{
		Frame() = default;

		virtual v2s get_dim() override {
			v2s dd{0,0};
			if (down) {
				dd = down->get_dim();
			}
			dim = v2s(2*S(2), 2*S(2)) + dd;
			return dim;
		}

		virtual void set_pos(v2s pos) override {
			this->pos = pos;
			if (down) {
				down->set_pos(pos + v2s(S(3), S(3)));
			}
		}

		virtual void render(Console & con) override
		{
			render_inline(con.win, pos, dim, ColorWhite, 1);

			if (down) {
				down->render(con);
			}
		}

	};


	struct Absolute: Widget
	{

		v2s get_dim() override {
			return dim;
		}

		void set_pos(v2s pos) override {
			this->pos = pos;
		}

		void render(Console & con) override
		{
			render_inline(con.win, pos, dim, ColorWhite, 1);

			if (down) {
				down->render(con);
			}
		}

	};


	template <int A, int B>
	struct Axis: Widget {

		int16_t adv{0};

		v2s get_dim() override
		{
			auto * w = down;
			while (w) {
				auto wdim = w->get_dim();

				dim[A] += wdim[A] + adv;
				if (dim[B] < wdim[B]) {
					dim[B] = wdim[B];
				}
				w = w->next;
			}
			return dim;
		}

		void set_pos(v2s p) override
		{
			pos = p;

			auto * w = down;
			auto c = pos;
			while (w) {

				w->set_pos(c);

				c[A] += w->dim[A] + adv;

				w = w->next;
			}
		}

		void render(Console & con) override
		{
			auto * x = down;
			while (x) {
				x->render(con.win);
				x = x->next;
			}
		}

	};


	using Line = Axis<0,1>;
	using Cross = Axis<1,0>;
	*/



	/*
	Shortcuts/menu/link  unit orders
	Mod+mouse tile/unit selection
	menu/link/shortcuts button/hyperlink
	Fx reports

	Actions on main view
	Actions on city view
	Actions on europe view
	*/




	/*
	struct Label: Widget {

		string text{""};
		int16_t maxlen{0};
		v2f align{0.5f, 0.5f};

		Label(string const& text, int16_t maxlen):
			text(text), maxlen(maxlen)
		{
		}

		PixFont const& get_font() const {
			//assert(font);
			return font_tiny();
		}

		v2s get_dim() override
		{
			dim = v2s(2*S(2), 2*S(2)) + v2s(maxlen * get_font().get_width(), get_font().height);
			return dim;
		}

		void set_pos(v2s p) override
		{
			pos = p;
		}

		void render(Console & con) override
		{
			auto & win = con.win;

			// output
			render_inline(win, pos, dim, ColorWhite, 1);

			auto b = v2s(S(2), S(2));

			b2s inner(pos + b, dim - b - b);

			auto t = render_text(win,
				inner.pos, inner.dim, {0.0f, 0.0f},
				font_tiny(), ColorFont,
				text
			);

		}
	};

	struct Button: Label {

		using Action = std::function<void()>;

		Action onpress{nullptr};

		Button() = default;
		Button(string const& text, int16_t maxlen, Action onpress):
			Label(text, maxlen), onpress(onpress)
		{
		}

		void render(Console & con) override
		{
			Label::render(con);

			// input
			con.on({EventPress, KeyLMB, pos, dim}, [this](Event & e) {
				onpress();
			});

		}
	};


	struct EditStr: Label {

		EditStr(string s, int16_t w): Label(s,w) {}

		string const& get_value() const { return text; }

		void render(Console & con) override
		{
			auto & win = con.win;

			// output
			render_inline(win, pos, dim, ColorWhite, 1);

			auto b = v2s(S(2), S(2));

			b2s inner(pos + b, dim - b - b);

			auto t = render_text(win,
				inner.pos, inner.dim, align,
				get_font(), ColorFont,
				"123"
			);

			// render cursor
			if (has_focus()) {

				render_text_at(win,
					v2s(t.pos[0] + t.dim[0], t.pos[1]),
					get_font(), ColorFont,
					"_"
				);
			}

			// input
			con.on({this, EventPress, KeyBackspace}, [this](Event & e){
				if (text.size() > 0) {
					text.pop_back();
				}
			});

			con.on({this, EventChar}, [this](Event & e) {
				text += e.get_chr();
			});

			con.on({EventPress, KeyLMB, pos, dim}, [this](Event & e) {
				grab_focus();
			});

			// if focus and p.match(EventPress, KeyA, ModCtrl)

		}


	};
*/



/*
	struct FrameNum: Frame {

		Widget * edit;
		function<void(int)> onenter;

		FrameNum(function<void(int)> onenter): onenter(onenter) {

			edit = new EditStr("", 6);

			adds(
				(new Cross())->adds(
					(new Line())->adds(
						new Label("Num", 5),
						edit
					),
					new Button("OK", 8, [onenter](){
						onenter(edit.get_value());
					})
				)
			);

		}

		void render(Console & con) override {
			Frame::render();

			con.on({EventPress, KeyEnter}, [this](){
				// close ?
				onenter(edit.get_value());
			});

			con.on({EventPress, KeyEsc}, [](){
				// close ?
			});


		}

	};*/







	struct Select: IWidget {

		v2s pos;
		v2s dim;

		vector<Col> cols;
		vector<Elem> rows;

		// close on select
		using Flag = uint8_t;
		Flag flags;
		Flag const
			FlagCloseOnSelect = 1,
			FlagFrame = 2;

		//virtual void onselect() {}

		Action oncancel;
		Action onselect;

		size_t cursor{0};
		size_t highlight{size_t(-1)};


		Select(int ncol)
		{
			for (auto i = 0; i < ncol; ++i)
			{
				cols.push_back(Col{});
			}
		}

		Select() {
			cols.push_back(Col{});
		}

		void add_col(string name, int width, float align) {
			cols.push_back(Col{align,width,name});
		}

		void set_cols(Col c0) {
			assert(cols.size() >= 1);
			cols[0] = c0;
		}

		void set_cols(Col c0, Col c1) {
			assert(cols.size() >= 2);
			cols[0] = c0;
			cols[1] = c1;
		}

		void add(vector<string> const& text, Action const& onselect, Key shortcut=KeyNone)
		{
			for (size_t i = 0; i < cols.size(); ++i)
			{
				auto w = get_text_dim(font_tiny(), text.at(i)) [0];
				if (w > cols.at(i).width) {
					cols.at(i).width = w;
				}
			}

			rows.push_back(Elem(text, onselect, shortcut));
		}

		void add(vector<string> const& text, bool hl, Action const& onselect, Key shortcut=KeyNone) {
			if (hl) { set_highlight(); }
			add(text, onselect, shortcut);
		}

		/*void add(string const& text, Action const& onselect, Key shortcut=KeyNone) {
			rows.push_back(Elem({text}, onselect, shortcut));
		}*/

		void render(Console & con) override;


		int16_t calc_width() const
		{
			auto sep = ly.sep;   // cell sep
			int width = 0;
			for (auto & c: cols) {
				width += c.width;
			}
			width += get_elems_stretch(cols.size(), 0, sep[0]);
			return width;
		}

		v2s calc_dim() const
		{
			auto line_height = ly.font_tiny;
			auto sep = ly.sep;   // cell sep

			auto width = calc_width();
			int height = get_elems_stretch(rows.size(), line_height, sep[1]);

			return v2s(width, height);
		}

		v2s get_dim()
		{
			return calc_dim();
		}

		void set_pos(v2s pos)
		{
			// default single column
			if (cols.size() == 0) {
				cols.push_back(Col{});
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
			auto box = b2s{pos, {0,0}};
			pos = calc_align(box, dim, {0.5f, 0.5f});
		}

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
		}

		v2s get_row_dim() const {
			return {calc_width(), font_tiny().height};
		}


		virtual bool handle(Console & con, Event e) override;
	};


	bool Select::handle(Console & con, Event e)
	{
		// KEYBOARD

		// esc -- cancel
		if (match(e, {EventPress, KeyEsc})) {
			oncancel();
			return 1;
		}

		// enter -- select under cursor
		if (match(e, {EventPress, KeyEnter})) {
			select();
			return 1;
		}

		// up arrow -- move cursor
		if (match(e, {EventPress, KeyUp})) {
			move_up();
			return 1;
		}

		// down arrow -- move cursor
		if (match(e, {EventPress, KeyDown})) {
			move_down();
			return 1;
		}

		// MOUSE

		auto row_dim = get_row_dim();
		auto row_pos = pos + ly.sep;

		for (size_t j = 0; j < rows.size(); ++j)
		{
			// left click on row -- move cursor
			if (match(e, {EventPress, KeyLMB, row_pos, row_dim}))
			{
				move(j);
				return 1;
			}

			// left release on row -- select
			if (match(e, {EventRelease, KeyLMB, row_pos, row_dim}))
			{
				select();
				return 1;
			}

			row_pos[1] += font_tiny().height + ly.sep[1];
		}

		// click anywhere -- cancel
		if (match(e, {EventPress, KeyLMB})) { oncancel(); return 1;}
		if (match(e, {EventPress, KeyRMB})) { oncancel(); return 1;}

		// click on dialog -- do nothing
		if (match(e, {EventPress, KeyLMB, pos, dim})) {
			return 1;
		}

		if (match(e, {EventPress, KeyRMB, pos, dim})) {
			return 1;
		}


		// any event -- catch
		return 1;
	}




	void Select::render(Console & con)
	{
		auto line_height = ly.font_tiny;
		auto sep = ly.sep;   // cell sep

		// background & outline border
		render_area({pos, dim}, res(WOODTILE));
		render_outline({pos, dim}, ColorDarkBrown, ly.line);

		auto row_dim = get_row_dim(); // dim[0] - sep[0]*2
		auto row_pos = pos + sep;

		for (size_t j = 0; j < rows.size(); ++j)
		{
			auto & row = rows[j];

			auto cpos = row_pos;

			if (j == cursor) {
				render_fill({row_pos, row_dim}, ColorSelectedBG);
			}

			// left click on row -- move cursor
			con.on({EventPress, KeyLMB, row_pos, row_dim},
				[this, j](){
					this->move(j);
				}
			);

			// left release on row -- select
			con.on({EventRelease, KeyLMB, row_pos, row_dim},
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

				render_text(cpos, cdim, v2f(col.align, 0.5),
					font_tiny(), fg, ColorNone,
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
		con.on({EventPress}, oncancel);

		// esc -- cancel
		con.on({EventPress, KeyEsc}, oncancel);

		// background & outline border
		render_area({pos, dim}, res(WOODTILE));
		render_outline({pos, dim}, ColorDarkBrown, ly.line);

		// click on dialog -- do nothing
		con.on({EventPress, pos, dim});

		// enter -- select under cursor
		/*con.on(EventPress, KeyEnter,
			[&selected,cursor,&keys,onselect](){ selected = keys.at(cursor); onselect(); }
		);*/

		// up arrow -- move cursor
		/*con.on(EventPress, KeyEnter,
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
				render_fill({row_pos, row_dim}, ColorSelectedBG);
			}

			if (key == selected) {
				fgind = ColorFontSelected;
			}

			// left click on row -- select
			con.on({EventPress, KeyLMB, row_pos, row_dim},
				[&selected,key,onselect](){ selected = key; onselect(); }
			);

			// render row by cell
			for (size_t i = 0; i < cols.size(); ++i) {
				auto & col = cols[i];
				auto & cell = row[i];

				auto cdim = v2s(col.width, line_height);

				render_text(
					cpos, cdim, v2f(col.align, 0.5),
					font_tiny(), fgind, ColorNone,
					cell
				);

				cpos[0] += col.width + sep[0];
			}

			row_pos[1] += line_height + sep[1];
		}

	}


	/*
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
		con.on(EventPress, oncancel);

		// background
		render_area(win, pos, dim, res(WOODTILE));
		render_outline(win, pos, dim, ColorDarkBrown, ly.line);


		// click on dialog -- do nothing
		con.on(EventPress, pos, dim, [](){});

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
				font_tiny(), fg, ColorNone,
				kvs[i].second
			);

			// left click on entry -- select
			con.on(EventPress, KeyLMB, p, d,
				[&selected,key,onselect](){ selected = key; onselect(); }
			);
		}

	}*/


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

		auto & box_tex = res(ICON, 123);

		// empty space
		{
			int num = unit.get_space_left();
			if (num) {

				// frame
				render_fill({cpos, tile_dim}, ColorDarkWoodBrown);
				render_inline({cpos, tile_dim}, ColorWoodBrown, ly.line);

				render_sprite(
					{cpos, item_dim}, v2f(0.5, 0.5),
					box_tex
				);

				render_text(
					cpos + v2s(item_dim[0], 0), text_dim, v2f(0, 0.5),
					font_tiny(), ColorWhite, ColorNone,
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
				render_fill({cpos, tile_dim}, ColorDarkWoodBrown);
				render_inline({cpos, tile_dim}, ColorWoodBrown, ly.line);

				// left click on cargo -- start drag cargo unload
				con.on({EventPress, KeyLMB, cpos, tile_dim},
					[item,num,&con](){ con.drag_cargo(item, num, -1); }
				);

				render_sprite({cpos, item_dim}, v2f(0.5, 0.5), box_tex);

				// item icon
				auto & item_tex = res(ICON, get_item_icon_id(item));

				render_sprite({cpos, item_dim}, v2f(0.5, 0.5), item_tex);

				// amount
				render_text(cpos + v2s(item_dim[0], 0), text_dim, v2f(0, 0.5),
					font_tiny(), ColorWhite, ColorNone,
					to_string(num)
				);

				cpos[0] += tile_dim[0] - ly.S(1);;
			}
		}



		// unit cargo area frame
		if (con.drag_item) {
			// left release -- drop cargo
			con.on({EventRelease, KeyLMB, pos, dim},
				[&con](){ con.drop_cargo(+1); }
			);
		}

	} // render_unit_cargo






	void show_select_makeable(Env const& env, Console & con, Colony & colony, Build & factory, b2s box, v2f const& align)
	{

		//auto & select = con.replace_widget<Select>();



		auto & select = con.push_widget<Select>(2);

		// cols format
		select.set_cols(
			{0, ly.S(1) * 24 * 4, "Name"},
			{1, ly.S(1) * 7 * 4,  "Cost"}
		);

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
		for (auto& mk: *env.bts)
		{
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
		for (auto & mk: *env.uts) {
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
			con.pop_widget();
		};   // oncancel

		select.onselect = [&con](){
			con.pop_widget();
		};   // onselect



		select.dim = select.get_dim();

		select.pos = calc_align(ly.scr, select.dim);



	}


	void render_city_store(Front & win, Console & con, v2s pos, v2s dim, Terr const& terr) {
		auto const& env = con.env; // get_render_env

		auto const& supp_reg = env.get_store(terr);

		Register prod_reg, cons_reg;
		logic::fill_colony_regs(env, terr, prod_reg, cons_reg);

		// render supply nums bg
		render_fill(
			{ly.city_supply_nums.pos, ly.city_supply_nums.dim},
			{76,100,172,255}
		);

		// render storage area
		render_fill({pos, dim}, {76,100,172,255});

		if (con.drag_item) {
			// left release city store -- unload cargo
			con.on({EventRelease, KeyLMB, pos, dim},
				[&con](){ con.drop_cargo(-1); }
			);
		}

		int tile_width = ly.S(16);
		int line_height = font_tiny().height;

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
			render_sprite({tile_pos, tile_dim}, {0.5f, 0.5f},
				res(ICON, get_item_icon_id(item))
			);

			// left click on item -- start drag cargo load
			con.on({EventPress, KeyLMB, tile_pos, tile_dim},
				[item,supp_num,&con](){ con.drag_cargo(item, supp_num, +1); }
			);

			// render supp num
			{
				auto pos = cpos + v2s(0, tile_dim[1]);
				auto dim = v2s(tile_width, line_height);

				auto proj = supp_num - cons_num + prod_num;
				if (proj < 0) {
					render_text(
						pos, cell_dim, v2f(0.5f, 0),
						font_tiny(), ColorYellow, ColorNone,
						format("%||", supp_num)
					);
				}
				else {
					render_text(
						pos, cell_dim, v2f(0.5f, 0),
						font_tiny(), ColorWhite, ColorNone,
						std::to_string(supp_num)
					);
				}
			}

			// render prod num
			if (prod_num) {
				render_text(
					cpos + up * _3, cell_dim, v2f(1, 0),
					font_tiny(), ColorWhite, ColorNone,
					format("+%||", prod_num)
				);
			}

			// render cons num
			if (cons_num) {
				render_text(
					cpos + up * _2, cell_dim, v2f(1, 0),
					font_tiny(), ColorWhite, ColorNone,
					format("-%||", cons_num)
				);
			}

			// render delta
			if (delta > 0) {
				render_text(
					cpos + up, cell_dim, v2f(1, 0),
					font_tiny(), ColorGreen, ColorNone,
					format("+%||", delta)
				);
			}
			else if (delta < 0) {
				render_text(
					cpos + up, cell_dim, v2f(1, 0),
					font_tiny(), ColorYellow, ColorNone,
					format("%||", delta)
				);
			}


			cpos[0] += tile_width;
		}
	}




	/// Render unit icon with terrain icon in background
	void render_unit_tile(Front & win, b2s box, Unit const& unit)
	{
		auto & unit_tex = res(ICON, get_icon_id(unit));

		// render terrain icon
		if (unit.get_travel() == TravelSea) {
			// water
			auto const& s = res(ZZ, 9);
			render_sprite(calc_align(box, s.dim, {0.5f, 1.0f}), s);
		}
		else {
			// grass
			auto const& s = res(ZZ, 10);
			render_sprite(calc_align(box, s.dim, {0.5f, 1.0f}), s);
		}
		// TODO: transported unit, wooden tex
		
		// render unit
		render_sprite(box, {0.5f, 0.5f}, unit_tex);
	}

	struct CityView: IWidget
	{
		void render(Con & con) override;
		bool handle(Con & con, Event e) override;
	};



	bool CityView::handle(Con & con, Event e)
	{
		return 0;
	}


	void CityView::render(Con & con)
	{
		auto const& env = con.env;


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
		auto & sand_tex = res(SANDTILE);
		render_area(ly.city_builds, sand_tex);

		auto & pixs = ly.pixs;
		auto & dims = ly.dims;


		auto& terr = *con.get_sel_terr();
		auto& colony = terr.get_colony();

		// render buildings
		{
			// hover anywhere -- clear label on building
			con.on({EventMotion},
				[&con]() { con.sel_colony_slot_id = -1; }
			);

			// render in reverse order to make sure that docks clickarea will not obscure other builds
			int i = colony.builds.size();
			while (i > 0) {
				i = i - 1;

				auto workplace_id = i;

				auto & build = colony.builds.at(i);

				auto const& build_tex = res(BUILDING, build.get_type_id());
				auto build_pos = ly.city_builds.pos + pixs.at(i);
				auto build_dim = build_tex.dim;

				render_sprite(build_pos, build_tex);

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
						render_text(
							button_pos, button_dim, {0.5,0.5},
							font_tiny(), ColorGray, ColorWhite,
							label
						);
					}
					else {
						render_text(
							button_pos, button_dim, {0.5,0.5},
							font_tiny(), ColorWhite, ColorNone,
							label
						);
					}

					render_outline(
						{button_pos, button_dim}, ColorWhite, ly.S(1)
					);

					// progress ind
					render_text(
						build_pos, build_dim, {1, 0},
						font_tiny(), ColorWhite, ColorBlack,
						progress
					);




					// left click on button -- select production
					con.on({EventPress, KeyLMB, button_pos, button_dim},
						[&env, &con, &colony, &build]() {
							show_select_makeable(env, con, colony, build,
								{ly.scr.pos, ly.scr.dim}, v2f{0.5f, 0.5f}
							);
						}
					);


				}



				// render build name
				if (con.sel_colony_slot_id == i) {

					render_text(
						build_pos, build_dim, {0.5, 0.5},
						font_tiny(), ColorWhite, ColorBlack,
						build.get_name()
					);

				}

				// left click on building -- assign worker
				con.on({EventPress, KeyLMB, build_pos, build_dim},
					[&con, workplace_id]() {
						if (con.get_sel_unit()) {
							con.command(format("work-build %||", workplace_id));
						}
					}
				);

				// number of produced items
				if (int y = nominal_prod) {
					auto const& item_tex = res(ICON, get_item_icon_id(build.get_proditem()));
					auto item_dim = item_tex.dim;

					// item icon
					render_sprite(build_pos, item_tex);

					// number
					render_text(

						build_pos + v2s(item_dim[0], 0),
						v2s(0, item_dim[1]),
						{0, 0.5},

						font_tiny(), ColorWhite, ColorBlack,
						std::to_string(y)
					);
				}


				// hover on building -- show label with building name (select buildplace)
				con.on({EventMotion, build_pos, build_dim},
					[&con, i]() { con.sel_colony_slot_id = i; }
				);

				v2s units_frame = {ly.S(25), ly.S(16)};

				// units on build
				int n = build.units.size();
				int i = 0;
				for (auto& unit_ptr: build.units) {
					auto& unit = *unit_ptr;
					auto& unit_tex = res(ICON, get_icon_id(unit));

					v2s unit_pos = calc_align(
						{
							build_pos + build_dim - units_frame + ly.S(v2s(1,1)), units_frame
						},
						unit_tex.dim,
						v2f(float(i+1)/float(n+1), 1)
					);

					v2s unit_dim = unit_tex.dim;

					v2s sel_pos = unit_pos;
					v2s sel_dim = unit_dim;

					// render unit on build
					render_sprite(unit_pos, unit_tex);

					auto unit_id = unit.id;

					if (unit_id == con.get_sel_unit_id()) {
						// render selection frame
						render_inline({sel_pos, sel_dim}, {255,100,100,255}, ly.line_sel);
					}
					else {
						// left click on unit -- select this unit
						con.on({EventPress, KeyLMB, sel_pos, sel_dim},
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

			auto& tex_wood = res(WOODTILE);
			render_area({ly.city_fields.pos, ly.city_fields.dim}, tex_wood);

			auto city_terr_pos = calc_align({ly.city_fields.pos, ly.city_fields.dim}, ly.terr_dim);

			auto& city_terr = terr;

			auto pos = env.get_coords(terr);
			auto city_coords = Coords(pos[0]-1, pos[1]-1);


			for(size_t field_id = 0; field_id < colony.fields.size(); ++field_id) {
				auto& field = colony.fields.at(field_id);

				auto& field_terr = field.get_terr();

				v2s relc = v2s(env.get_coords(*field.terr) - env.get_coords(city_terr));
				auto field_pos = city_terr_pos + vmul(relc, ly.terr_dim);
				auto field_dim = ly.terr_dim;


				render_terr(field_pos, env, field_terr);


				if (field.units.size() == 0) {
					// left click on field -- assign unit to work there
					string cmd = format("work-field %||", field_id);
					con.on({EventPress, KeyLMB, field_pos, field_dim},
						[&con,cmd](){ con.command(cmd); }
					);
				}



				// units on field
				for (auto& unit_p: field.units) {
					auto& unit = *unit_p;
					{
						v2s unit_dim = v2s(field_dim[0]/2, field_dim[1]);
						v2s unit_pos = field_pos + v2s(field_dim[0]/2, 0);

						v2s item_dim = field_dim; //v2s(field_dim[0]/2, field_dim[1]);
						v2s item_pos = field_pos;

						v2s sel_pos = field_pos;
						v2s sel_dim = field_dim;

						auto& unit_tex = res(ICON, get_icon_id(unit));

						// field production
						if (auto proditem = field.get_proditem()) {
							// render produced item
							auto& item_tex = res(ICON, get_item_icon_id(proditem));
							render_sprite(
								calc_align({item_pos, item_dim}, item_tex.dim),
								item_tex
							);

							// render produced item amount
							auto text = to_string(logic::get_nominal_prod(env, field, field.get_proditem()));

							render_text(
								item_pos, {0,0}, {0,0},
								font_tiny(), ColorWhite, ColorBlack,
								text
							);
						}

						// render unit on field
						render_sprite(
							calc_align({unit_pos, unit_dim}, unit_tex.dim),
							unit_tex
						);

						auto unit_id = unit.id;

						if (unit_id == con.get_sel_unit_id()) {
							// render selection frame
							render_inline({sel_pos, sel_dim}, {255,100,100,255}, ly.line);

							// left click on field with selected worker -- switch to next proditem
							/*con.on(EventPress, KeyLMB, sel_pos, sel_dim,
								[&con,field_id]() {
									con.command("prodnext-field " + to_string(field_id));
								}
							);*/



							// left click on field with selected worker -- show select proditem menu
							con.on({EventPress, KeyLMB, sel_pos, sel_dim},
								[&con, &field]() {
									con.prod_to_workplace = &field;
									con.selected_id = field.get_proditem();
								}
							);



						}
						else {
							// left click on field with worker -- select this worker
							con.on({EventPress, KeyLMB, sel_pos, sel_dim},
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
			con.on({EventPress, KeyRMB},
				[&con]() {
					con.unselect_unit();
				}
			);
		}


		// left click on city terrain area with selected unit -- unwork that unit
		if (con.get_sel_unit()) {
			con.on({EventPress, KeyLMB, ly.city_units.pos, ly.city_units.dim},
				[&con](){con.command("work-none");}
			);
		}

		// render city terr units area bg
		render_fill(
			{ly.city_units.pos, ly.city_units.dim},
			ColorSkyBlue
		);

		// render unit cargo area bg
		render_fill(
			{ly.city_unit_cargo.pos, ly.city_unit_cargo.dim},
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
					render_inline({sel_pos, sel_dim}, {255,100,100,255}, ly.line);

					// left click on selected unit (on terr) -- show equip select
					con.on({EventPress, KeyLMB, sel_pos, sel_dim},
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
					con.on({EventPress, KeyLMB, sel_pos, sel_dim},
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
		render_fill({ly.city_exit.pos, ly.city_exit.dim}, {140,0,140,255});

		render_text(
			ly.city_exit.pos + v2s(1,1),
			font_tiny(), ColorFont,
			"RET"
		);

		// left click on exit button -- exit city screen
		con.on({EventPress, KeyLMB, ly.city_exit.pos, ly.city_exit.dim},
			[&con, &terr](){ con.command("exit"); }
		);

		// render fields
		//render_fields(win, env, con, ly.city_fields.pos, );


		if (con.equip_to_unit_id) {
			vector<int> keys;
			vector<vector<string>> rows;

			for (auto const* ut: logic::equip_to_types(env, env.get<Unit>(con.equip_to_unit_id))) {
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
			cols.push_back({0.0f, ly.S(1) * 16 * 4, "Name"});
			cols.push_back({1.0f, ly.S(1) * 24 * 4, "Equip"});



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
			for (auto& item: logic::get_all_items(env)) {

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
			cols.push_back({0.0f, 14 * 4 * ly.S(1), "Item"});
			cols.push_back({1.0f, 5 * 4 * ly.S(1),  "Prod"});

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

				auto& unit_tex = res(ICON, get_icon_id(unit));
				v2s unit_pos = pos + vmul(unit_dim, v2s(i,j));

				v2s sel_pos = unit_pos;
				v2s sel_dim = unit_dim;

				// render unit
				render_unit_tile(win, {unit_pos, unit_dim}, unit);


				// select
				auto unit_id = unit.id;

				if (unit_id == con.get_sel_unit_id()) {
					// render selection frame
					render_inline({sel_pos, sel_dim}, {255,100,100,255}, ly.line);
				}
				else {
					// left click on unselected unit -- select unit
					con.on({EventPress, KeyLMB, sel_pos, sel_dim},
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

	array<Terr const*, 9> make_terr_ext(Env const& env, Coords const& p)
	{
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


	// phys coast encoding
	//
	// 00000000
	//       ^^ subtile
	//
	// 00000000
	//    ^^^ clockwise neigh
	//
	// subtiles
	// 01
	// 32
	//
	// example
	//
	//  ab
	// 01c
	// 32
	//
	// so subtile 1 with neighs a=water, b=water, c=~water
	// is 00011001
	//
	// base num for phys coast sprites is 109
	//


	int8_t get_coast_index(uint8_t const& l, Terr const& t0, Terr const& t1, Terr const& t2)
	{
		uint8_t k =
			(!looks_like_sea(t2) << 2) +
			(!looks_like_sea(t1) << 1) +
			(!looks_like_sea(t0) << 0);
		return (k<<2) + l;
	}

	Sprite const& get_coast_subtile(uint8_t const& l, Terr const& t0, Terr const& t1, Terr const& t2)
	{
		return res(PHYS, 109 + get_coast_index(l, t0, t1, t2));	
	}


	/// Render diffused biome
	void render_diffuse(v2s pos, Env const& env, Coords c, ResId blend_id)
	{
		if (env.has_coords(c))
		{
			auto biome = env.get_terr(c).biome;
			render_sprite_replace(pos,
				res(PHYS, blend_id),
				res(TERRAIN, get_biome_icon_id(biome))
			);
		}
	}

	


	/// Render terrain: altitude, biome and physical features
	void render_terr(v2s pos, Env const& env, Terr const& terr)
	{

		auto& pix = pos;

		auto coords = env.get_coords(terr);

		auto loc = make_terr_ext(env, coords);

		auto biome = terr.biome;
		auto forest = terr.has_phys(PhysForest);

		auto hill = (terr.get_alt() == AltHill);
		auto mountain = (terr.get_alt() == AltMountain);



		// render biome
		render_sprite(pix, res(TERRAIN, get_biome_icon_id(biome)));

		// render neighs pattern
		auto N = Coords(0,-1);
		auto E = Coords(+1,0);
		auto S = Coords(0,+1);
		auto W = Coords(-1,0);

		render_diffuse(pix, env, coords + N, 105);
		render_diffuse(pix, env, coords + E, 106);
		render_diffuse(pix, env, coords + S, 107);
		render_diffuse(pix, env, coords + W, 108);

		
		if (is_water(terr))
		{
			// base water tile
			int base;
			if (terr.has_phys(PhysSeaLane)) {
				base = 12;
			}
			else {
				base = 11;
			}

			// half-tile dim
			int16_t const h = conf.tile_dim >> 1;

			// one tile is composed of 4 subtiles
			//
			// subtile numbering
			// 01
			// 32

			render_sprite_replace(pix + v2s(0,0),
				get_coast_subtile(0, get(loc,DirA), get(loc,DirQ), get(loc,DirW)),
				make_subtile(0, TERRAIN + base)
			);

			render_sprite_replace(pix + v2s(h,0),
				get_coast_subtile(1, get(loc,DirW), get(loc,DirE), get(loc,DirD)),
				make_subtile(1, TERRAIN + base)
			);

			render_sprite_replace(pix + v2s(h,h),
				get_coast_subtile(2, get(loc,DirD), get(loc,DirC), get(loc,DirX)),
				make_subtile(2, TERRAIN + base)
			);

			render_sprite_replace(pix + v2s(0,h),
				get_coast_subtile(3, get(loc,DirX), get(loc,DirZ), get(loc,DirA)),
				make_subtile(3, TERRAIN + base)
			);

		}


		// level
		if (terr.get_alt() == AltMountain) {
			//render_sprite(win, pix, res(PHYS, 33 + get_wxad_index_level(loc, terr.get_alt())));
			render_sprite(pix, res(PHYS, 33));
		}

		if (terr.get_alt() == AltHill) {
			// render_sprite(win, pix, res(PHYS, 49 + get_wxad_index_level(loc, terr.get_alt())));
			render_sprite(pix, res(PHYS, 49));
		}

		// phys feats & improvments
		if (terr.has_phys(PhysPlow)) {
			render_sprite(pix, res(PHYS, 150));
		}

		if (terr.has_phys(PhysForest)) {
			render_sprite(pix, res(PHYS, 65 + get_wxad_index(loc, PhysForest)));
		}

		if (terr.has_phys(PhysMajorRiver)) {
			if (!is_water(terr)) {
				auto ind = get_wxad_index(loc, PhysMajorRiver|PhysMinorRiver);
				if (ind) {
					render_sprite(pix, res(PHYS, 1 + ind));
				}
				else {
					render_sprite(pix, res(PHYS, 16));
				}
			}
			else {
				for (int i = 0; i < 8; i += 2) {
					if ( get(loc, CLOCKWISE_DIRS[i]).has_phys(PhysMajorRiver) ) {
						render_sprite(pix, res(PHYS, 141 + (i >> 1)));
					}
				}
			}

		}
		else if (terr.has_phys(PhysMinorRiver)) {
			if (!is_water(terr)) {
				auto ind = get_wxad_index(loc, PhysMajorRiver|PhysMinorRiver);
				if (ind) {
					render_sprite(pix, res(PHYS, 17 + ind));
				}
				else {
					render_sprite(pix, res(PHYS, 32));
				}
			}
			else {
				for (int i = 0; i < 8; i += 2) {
					if ( get(loc, CLOCKWISE_DIRS[i]).has_phys(PhysMinorRiver) ) {
						render_sprite(pix, res(PHYS, 145 + (i >> 1)));
					}
				}
			}
		}

		if (terr.has_phys(PhysRoad)) {
			bool r = false;
			for (int i=0; i<8; ++i) {
				if ( get(loc, CLOCKWISE_DIRS[i]).has_phys(PhysRoad) )
				{
					if (conf.hires) {
						render_sprite(pix-v2s(4,4), res(PHYS, 82 + i));
					}
					else {
						render_sprite(pix, res(PHYS, 82 + i));
					}
					r = true;
				}
			}
			if (!r) {
				render_sprite(pix, res(PHYS, 81));
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

		int x = (pos[0] - delta[0]) * ly.terr_dim[0] + map_pix[0];
		int y = (pos[1] - delta[1]) * ly.terr_dim[1] + map_pix[1];

		render_terr(v2s(x,y), env, terr);

	}



	RGBA8 get_unit_color(Unit const& u) {

		auto c = get_control_color(u.get_control());

		if (u.get_time_left() == 0) {
			return RGBA8(c.r/2, c.g/2, c.b/2, c.a);
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
			render_sprite(pos, res(ICON, 4));

			// colony flag
			{
				auto p = env.get_control(terr);
				if (p != ControlNone) {
					// render owner flag over colony (unit in garrison)
					render_sprite(v2s(pos[0] + ly.S(5), pos[1]), res(ICON, get_control_flagicon(p)));
				}
			}

		}
		else if (terr.has_units() and !sel_unit_here) {
			// unit
			auto & unit = env.get_defender(terr);

			auto & icon = res(ICON, get_icon_id(unit));
			// render unit in field
			render_shield(pos, get_unit_color(unit), con.get_letter(unit));
			render_sprite(
				calc_align({pos, ly.terr_dim}, icon.dim),
				icon
			);

		}


		// now render selected unit
		if (sel_unit_here) {
			render_selected_unit(win, con, pos, *con.get_sel_unit());
		}

	}


	/// Return res num assigned to this dir
	ResId dir_res_num(Dir dir) {
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
		con.on({EventPress, KeyNumpad2},
			[&con,unit_id](){ con.command("move 0 1"); }
		);
		con.on({EventPress, KeyNumpad8},
			[&con,unit_id](){ con.command("move 0 -1"); }
		);
		con.on({EventPress, KeyNumpad6},
			[&con,unit_id](){ con.command("move 1 0"); }
		);
		con.on({EventPress, KeyNumpad4},
			[&con,unit_id](){ con.command("move -1 0"); }
		);
		con.on({EventPress, KeyNumpad3},
			[&con,unit_id](){ con.command("move 1 1"); }
		);
		con.on({EventPress, KeyNumpad7},
			[&con,unit_id](){ con.command("move -1 -1"); }
		);
		con.on({EventPress, KeyNumpad1},
			[&con,unit_id](){ con.command("move -1 1"); }
		);
		con.on({EventPress, KeyNumpad9},
			[&con,unit_id](){ con.command("move 1 -1"); }
		);

		// alpha move
		con.on({EventPress, KeyX},
			[&con,unit_id](){ con.command("move 0 1"); }
		);
		con.on({EventPress, KeyW},
			[&con,unit_id](){ con.command("move 0 -1"); }
		);
		con.on({EventPress, KeyD},
			[&con,unit_id](){ con.command("move 1 0"); }
		);
		con.on({EventPress, KeyA},
			[&con,unit_id](){ con.command("move -1 0"); }
		);
		con.on({EventPress, KeyC},
			[&con,unit_id](){ con.command("move 1 1"); }
		);
		con.on({EventPress, KeyQ},
			[&con,unit_id](){ con.command("move -1 -1"); }
		);
		con.on({EventPress, KeyZ},
			[&con,unit_id](){ con.command("move -1 1"); }
		);
		con.on({EventPress, KeyE},
			[&con,unit_id](){ con.command("move 1 -1"); }
		);

		// build colony
		con.on({EventPress, KeyB},
			[&con](){ con.command("build-colony Unnamed"); }
		);
		// build road
		con.on({EventPress, KeyR},
			[&con,unit_id](){ con.cmd_unit(unit_id, Cmd(InsRoad)); }
		);
		// plow fields
		con.on({EventPress, KeyP},
			[&con,unit_id](){ con.cmd_unit(unit_id, Cmd(InsPlow)); }
		);
		// clear forest
		con.on({EventPress, KeyO},
			[&con,unit_id](){ con.cmd_unit(unit_id, Cmd(InsClear)); }
		);
		// space - skip unit
		con.on({EventPress, KeySpace},
			[&con,unit_id](){ con.cmd_unit(unit_id, Cmd(InsWait, 1)); }
		);

		// esc - clear orders
		con.on({EventPress, KeyEsc},
			[&con,unit_id](){
				con.clear_unit_cmds(unit_id);
				con.set_input_mode(con.InputModeDefault);
			}
		);

		// press enter -- execute current orders
		con.on({EventPress, KeyEnter},
			[&con](){
				if (auto * u = con.get_sel_unit_ext()) {
					con.exec_unit_cmds(*u);
					con.select_next_unit();
				}
				con.set_input_mode(con.InputModeDefault);
			}
		);

		// M - enter move mode
		con.on({EventPress, KeyM},
			[&con](){ con.set_input_mode(con.InputModeMove); }
		);



	}

	void render_selected_unit(Front & win, Console & con, v2s pos, Unit & unit) {
		auto & icon = res(ICON, get_icon_id(unit));
		auto unit_id = unit.id;

		// render blinking shield
		if (con.blink()) {
			render_shield(pos, get_unit_color(unit), con.get_letter(unit));
		}

		// render unit in field
		render_sprite(
			calc_align({pos, ly.terr_dim}, icon.dim),
			icon
		);




	}

	//void select_area()

	void render_fog(v2s pos) {
		render_fill({pos, ly.terr_dim}, RGBA8(64,64,64,64));
	}

	void render_unknown(v2s pos) {
		render_sprite(pos, res(PHYS, 149));
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

	void render_on_tile(Console & con, Coords xy, Sprite const& tex)
	{
		if (is_tile_on_screen(con, xy)) {
			auto rpos = get_tile_on_screen(con, xy).pos;
			render_sprite(rpos, tex);
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

					render_on_tile(con, pos,
						res(ZZ, dir_res_num(dir))
					);

					pos += dir2vec(dir);

					render_on_tile(con, pos,
						res(ZZ, dir_res_num(turnabout(dir)))
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

				render_text(
					box.pos, box.dim, v2f(0.5f, 0.5f),
					font_tiny(), ColorBlue, ColorBlack,
					m.label
				);
			}
		}
	}

	void render_debug_ai(Console & con) {
		if (con.runner)
		{
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




	struct MapView: IWidget
	{
		void render(Console & con) override;
		bool handle(Console & con, Event e) override;

		bool handle_terr(Console & con, Coords coords, v2s pos, v2s dim, Event e);





	};





	void render_map(Front & win, Env const& env, Console & con, v2s pos,
			Coords const& delta)
	{
		auto w = con.view_dim[0];
		auto h = con.view_dim[1];

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
						render_terr(pos, env, terr);
					}
					else {
						render_unknown(pos);
					}

					auto dim = ly.terr_dim;

					// handle


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

					auto & terr = env.get_terr(coords);

					if (con.has_vision(terr)) {
						render_stack(win, con, pos, env, terr);
					}
					else {
						if (con.is_discovered(terr)) {
							render_fog(pos);
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
						render_inline(
							{vmul(coords - vpos, ly.terr_dim) + pos, ly.terr_dim},
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


						render_inline(
							{vmul(coords - vpos, ly.terr_dim) + pos, ly.terr_dim},
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



	void MapView::render(Console & con)
	{
		render_map(win, con.env, con, ly.map.pos, Coords(0,0));
	}

	bool MapView::handle_terr(Console & con, Coords coords, v2s pos, v2s dim, Event e)
	{
		// right click on terr -- center view
		if (match(e, {EventPress, KeyRMB, pos, dim})) {
			con.center_view_on(coords);
			return 1;
		}

		if (con.get_input_mode() == con.InputModeDefault)
		{
			if (e.match_box({pos,dim}))
			{
				auto & terr = con.env.get_terr(coords);

				// left press on terr with shift -- add select terr
				if (match(e, {EventPress, KeyLMB, ModLShift, pos, dim})) {
					con.select_terr(coords, +1);
					return 1;
				}

				// left press on terr with ctrl -- sub select terr
				if (match(e, {EventPress, KeyLMB, ModLCtrl, pos, dim})) {
					con.select_terr(coords, -1);
					return 1;
				}

				// hover with button down with shift over terr -- select add
				if (match(e, {EventMotion, ModLMB|ModLShift, pos, dim})) {
					con.select_terr(coords, +1);
					return 1;
				}

				// hover with button down with ctrl over terr -- select sub
				if (match(e, {EventMotion, ModLMB|ModLCtrl, pos, dim})) {
					con.select_terr(coords, -1);
					return 1;
				}

				// hover with button down over terr -- select set
				if (match(e, {EventMotion, ModLMB, pos, dim})) {
					con.select_terr(coords);
					return 1;
				}


				// left press -- city enter OR select unit OR select terr
				if (match(e, {EventPress, KeyLMB, pos, dim}))
				{
					if (terr.has_colony())
					{
						con.enter(coords);
						return 1;
					}
					else if (terr.has_units())
					{
						print("select unit\n");
						con.select_unit(terr.units.at(0));
						return 1;
					}
					else
					{
						print("select terr\n");
						con.select_terr(coords);
						return 1;
					}
				}



			}
		}
		else if (con.get_input_mode() == con.InputModeMove)
		{
			// left press on terr -- show path
			if (match(e, {EventPress, KeyLMB, pos, dim}))
			{
				if (auto * u = con.get_sel_unit_ext()) {
					auto path = con.find_path(con.get_coords(*u), coords, *u);
					u->cmds.clear();
					if (path.exists()) {
						u->cmds.adds(path.cmds);
					}
				}
				return 1;
			}

		}

		return 0;
	}

	bool MapView::handle(Console & con, Event e)
	{
		// left arrow -- scroll map
		if (match(e, {EventPress, KeyLeft})) {
			con.move_view({-1,0});
			return 1;
		}

		// right arrow -- scroll map
		if (match(e, {EventPress, KeyRight})) {
			con.move_view({+1,0});
			return 1;
		}

		// up arrow -- scroll map
		if (match(e, {EventPress, KeyUp})) {
			con.move_view({0,-1});
			return 1;
		}

		// down arrow -- scroll map
		if (match(e, {EventPress, KeyDown})) {
			con.move_view({0,+1});
			return 1;
		}


		auto w = con.view_dim[0];
		auto h = con.view_dim[1];

		auto vpos = con.view_pos;
		auto vdim = v2s(w,h);

		auto view_box = ext::b2<Coord>(con.view_pos, con.view_dim);

		auto & env = con.env;

		for (int j = 0; j < h; ++j)
		{
			for (int i = 0; i < w; ++i)
			{
				auto coords = Coords(i,j) + vpos;
				if (env.in_bounds(coords)) {

					auto pos = ly.map.pos + vmul(ly.terr_dim, v2s(i,j));
					auto dim = ly.terr_dim;

					// handle terr
					if (handle_terr(con, coords, pos, dim, e)) {
						return 1;
					}
				}
			}
		}
		return 0;
	}





	void finish_popup(Console & con, Select & s)
	{
		s.oncancel = [&con](){
			con.pop_widget();
		};   // oncancel

		s.onselect = [&con](){
			con.pop_widget();
		};   // onselect

		s.dim = s.get_dim();
		s.pos = calc_align(ly.scr, s.dim);



	}

	void finish_popup_at(Console & con, Select & s, v2s pos)
	{
		s.oncancel = [&con](){
			con.pop_widget();
		};   // oncancel

		s.onselect = [&con](){
			con.pop_widget();
		};   // onselect

		s.dim = s.get_dim();
		s.pos = pos;


	}

	void show_select_biome(Env const& env, Console & con, Terr const& terr) {
		auto current = terr.get_biome();

		auto & s = con.push_widget<Select>(1);
		//s.set_cols({0, 16 * font_tiny().get_width(), "Biome"});


		for (auto i = BiomeNone+1; i < BiomeEnd; ++i)
		{
			s.add({get_biome_name(i)}, i == current, [&con, i](){
				con.set_biome(i);
			});
		}

		s.oncancel = [&con](){
			con.pop_widget();
		};   // oncancel

		s.onselect = [&con](){
			con.pop_widget();
		};   // onselect

		s.dim = s.get_dim();
		s.pos = calc_align(ly.scr, s.dim);



	}

	void show_select_alt(Env const& env, Console & con, Terr const& terr)
	{

		auto current = con.get_alt();

		auto & s = con.push_widget<Select>(1);

		for (auto i = AltNone+1; i < AltEnd; ++i)
		{
			s.add({get_alt_name(i)}, i == current, [&con, i](){
				con.set_alt(i);
			});
		}

		s.oncancel = [&con](){
			con.pop_widget();
		};   // oncancel

		s.onselect = [&con](){
			con.pop_widget();
		};   // onselect

		s.dim = s.get_dim();
		s.pos = calc_align(ly.scr, s.dim);

	}

	void show_select_river(Console & con, Terr & terr) {
		auto current = terr.get_phys(PhysRiver);
		auto & s = con.push_widget<Select>(1);

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
		auto & s = con.push_widget<Select>(1);

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
		auto & s = con.push_widget<Select>(1);
		auto * u = con.get_sel_unit();


		for (auto i = UnitTypeNone + 1; i < UnitTypeEnd; ++i)
		{
			// TODO remove this - select should auto page
			// skip to save space; menu too long with this
			if (i == UnitTypePioneers20) {
				i = UnitTypePioneers100;
			}

			auto & type = env.get_unittype(i);

			s.add({type.get_name()}, i == u->get_type().id, [u, &type](){
				u->set_type(type);
			});
		}

		finish_popup(con, s);
	}

	void show_select_speciality(Console & con) {

		auto & s = con.push_widget<Select>(1);

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
		void render(Front & win, Console & con);

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

	struct SelectFaction
	{
		void render(Con & con);
		bool handle(Con & con, Event e);
	};

	void show_select_nation(Console & con) {

		auto & s = con.push_widget<Select>();

		for (auto & faction: list_values(con.env.factions))
		{
			auto id = faction.id;
			s.add({faction.get_name()}, id == con.get_control(), [&con, id](){
				con.set_control(id);
			});
		}

		s.oncancel = [&con](){
			con.pop_widget();
		};   // oncancel

		s.onselect = [&con](){
			con.pop_widget();
		};   // onselect

		s.dim = s.get_dim();
		s.pos = calc_align(ly.scr, s.dim);

	}

	struct SidePanel: IWidget
	{
		void render(Con & con);
		bool handle(Con & con, Event e);
	};

	void SidePanel::render(Con & con)
	{
		auto & env = con.env;
		auto pos = ly.pan.pos;
		auto dim = ly.pan.dim;

		//front::Image img = res("COLONIZE/WOODTILE_SS", 1);  32x24

		render_area({pos, dim}, res(WOODTILE));

		// terrain info
		/*
		 * Biome: Plains
		 * road
		 */


		string faction_name = "Nobody";
		if (env.has_current_control()) {
			auto & f = env.get<Faction>(env.get_current_control());
			faction_name = f.get_name();
		}




		auto cur = TextRend2(win, con, ly.pan, font_tiny(), StyleMenu);

		// Turn 5, England
		cur.text("Turn " + to_string(env.get_turn_no()) + ", " + faction_name + "\n");


		string my_name =
			(con.get_control() != ControlNone) ?
				env.get<Faction>(con.get_control()).get_name() :
				"Nobody";

		cur.text("You are: ");
		cur.link(my_name, con.editing, [&con](){
			show_select_nation(con);
		});
		cur.text("\n");




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
						case PhysMinorRiver: return "+R";
						case PhysMajorRiver: return "++R";
					}
					return "-R";
				};

				cur.text("(");
				cur.link(get_river_name(river), con.editing,
					[&con,river](){
						con.set_phys(PhysRiver, next_phys(PhysRiver, river));
					}
				);
				cur.text(",");

				cur.link((forest ? "+F" : "-F"), con.editing,
					[&con,forest](){
						con.set_phys(PhysForest, next_phys(PhysForest, forest));
					}
				);
				cur.text(",");

				cur.link((plow ? "+P" : "-P"), con.editing,
					[&con,plow](){
						con.set_phys(PhysPlow, next_phys(PhysPlow, plow));
					}
				);
				cur.text(",");


				cur.link((road ? "+R" : "-R"), con.editing,
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

			cur.text(format("moves: %||\n", logic::get_moves(*u)));
			// cur.text(format("transported: %||\n", u->get_transported()));
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
		if (env.has_current_control())
		{
			render_fill(
				{pos + v2s(dim[0]-ly.S(1),0), v2s(ly.S(1), ly.S(1))},
				get_control_color(env.get_current_control())
			);
		}

		// unit list
		if (auto t = con.get_sel_terr())
		{
			v2s pos = ly.pan.pos + v2s(0,ly.S(100));
			v2s dim = v2s(ly.pan.dim[0], ly.terr_dim[1]*3);

			render_fill({pos, dim},
				RGBA8(0,0,0,64)
			);

			render_units(win, con, pos, dim,
				env,
				*t
			);
		}


		// Idle Unit
		{
			if (env.has_current_control()) {

				//auto & p = env.get_current_control();

				auto colind = (con.get_next_to_order(nullptr)) ? ColorGray : ColorWhite;

				// show button
				auto text_box = render_text(
					ly.pan.pos, ly.pan.dim - v2s(0, ly.font_tiny * 2), v2f(0.5, 1.0),
					font_tiny(), colind, ColorNone,
					"Idle Unit"
				);

				// left click -> select next idle
				con.on({EventPress, KeyLMB, text_box.pos, text_box.dim},
					[&con](){ con.select_next_unit(); }
				);

			}
		}



		// Idle Factory
		{
			if (env.has_current_control()) {

				auto colind = ColorGray;

				auto * terr = con.get_idle_colony(env.get_current_control());
				if (terr) {
					colind = ColorWhite;
				}

				// show button
				auto text_box = render_text(
					ly.pan.pos, ly.pan.dim - v2s(0, ly.font_tiny), v2f(0.5, 1.0),
					font_tiny(), colind, ColorNone,
					"Idle Factory"
				);


				// left click -> select idle
				con.on({EventPress, KeyLMB, text_box.pos, text_box.dim},
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

			{
				auto p = env.get_current_control();

				if (con.get_next_to_repeat(nullptr)) {
					lab = "Move all";
					cmd = "move-all";
					colind = ColorWhite;


					if (con.get_input_mode() == con.InputModeDefault)
					{
						// Ctrl + Enter -> move all
						/*con.on(EventPress, halo::ModCtrl, KeyEnter,
							[&con](){
								print("to repeat: %||\n", con.get_next_to_repeat(nullptr)->id);
								con.repeat_all();
							}
						);*/
					}
				}
				else {
					lab = "Ready";
					cmd = "ready";
					colind = (con.get_next_to_order(nullptr)) ? ColorGray : ColorWhite;



				}
			}



			// label
			auto text_box = render_text(
				ly.pan.pos, ly.pan.dim, v2f(0.5, 1.0),
				font_tiny(), colind, ColorNone,
				lab
			);

			// left click -> start/ready
			con.on({EventPress, KeyLMB, text_box.pos, text_box.dim},
				[&con](){ con.env.ready(); }
			);

		}

	}

	bool SidePanel::handle(Con & con, Event e)
	{
		// F12 -- toogle edit mode
		if (match(e, {EventPress, KeyF12}))
		{
			con.editing = !con.editing;
			return 1;
			//con.put("EDITING = %||", int(con.editing));
		}


		//if (con.get_input_mode() == con.InputModeDefault)
		{

			// Ctrl+Enter -- ready
			//print("%||\n", e);
			if (match(e, {EventPress, ModLCtrl, KeyEnter}))
			{
				con.env.ready();
				return 1;
				//con.put("EDITING = %||", int(con.editing));
			}
		}



		return 0;
	}











	void render_cursor(Console const& con)
	{
		if (con.drag_item)
		{
			auto pos = v2s(get_logical_pos(win, con.mouse.pos));
			//print("cursor: %||, %||\n", mp.x, mp.y);

			render_sprite(pos, res(ICON, get_item_icon_id(con.drag_item)));
		}
	}



	void render_drag_clear(Console & con)
	{
		if (con.drag_item)
		{
			// left release anywhere -- end drag cargo
			con.on({EventRelease, KeyLMB},
				[&con](){ con.drop_cargo(0); }
			);
		}
	}



/*

	struct MenuEntry
	{
		string label{""};
		function<bool()> can_run;
		function<void()> run;
	};




		s.add({"Move (M)"}, [&con](){
			if (con.has_sel_unit()) {
				con.set_input_mode(con.InputModeMove);
			}
		});

*/







	struct EditMenu: IWidget{
		void render(Con & con) override;
		bool handle(Con & con, Event e) override;
		void init(Con & con, v2s pos);
		void showhide() { visible = not visible; }


		bool visible{0};
		Select s;
	};

	void EditMenu::render(Con & con) {
		if (visible) {
			s.render(con);
		}
	}

	bool EditMenu::handle(Con & con, Event e) {
		if (visible) {
			if (s.handle(con, e)) {
				return 1;
			}
		}
		return 0;
	}

	void EditMenu::init(Con & con, v2s pos)
	{
		s.add({"Add faction"}, [&con](){

		});

		s.add({"Ready"}, [&con](){
			con.env.ready();
		});

		s.add({"Turn"}, [&con](){
			con.env.turn();
		});

		s.oncancel = [this](){
			this->showhide();
		};   // oncancel

		s.onselect = [this](){
			this->showhide();
		};   // onselect

		s.dim = s.get_dim();
		s.pos = pos;
	}

	struct OrderMenu: IWidget{
		void render(Con & con) override;
		bool handle(Con & con, Event e) override;
		void init(Con & con, v2s pos);
		void showhide() { visible = not visible; }


		bool visible{0};
		Select s;
	};

	void OrderMenu::render(Con & con) {
		if (visible) {
			s.render(con);
		}
	}

	bool OrderMenu::handle(Con & con, Event e) {
		if (visible) {
			if (s.handle(con, e)) {
				return 1;
			}
		}
		return 0;
	}


	void OrderMenu::init(Con & con, v2s pos)
	{
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

		s.oncancel = [this](){
			this->showhide();
		};   // oncancel

		s.onselect = [this](){
			this->showhide();
		};   // onselect

		s.dim = s.get_dim();
		s.pos = pos;
	}


	struct Link: IWidget {
		void render(Console & con) override;
		bool handle(Console & con, Event e) override;

		string text;
		Action act;
		b2s box;

		Link() = default;
		Link(string const& text, Action act): text(text), act(act) {
			box.dim = get_text_dim(font_tiny(), text);
		}

		void set_text(string const& t) {
			text = t;
			box.dim = get_text_dim(font_tiny(), text);
		}

		v2s get_sub_pos() const {
			return {box.pos[0], box.end()[1]};
		}
	};

	void Link::render(Console & con)
	{
		render_text(box.pos, font_tiny(), StyleMenu.hl, text);
	}

	bool Link::handle(Console & con, Event e)
	{
		if (match(e, {EventPress, KeyLMB, box.pos, box.dim}))
		{
			act();
			return 1;
		}
		return 0;
	}

	struct MainMenu: IWidget{
		void render(Console & con) override;
		bool handle(Console & con, Event e) override;

		Link order;
		Link edit;

		OrderMenu order_menu;
		EditMenu edit_menu;


		MainMenu(Console & con) {

			auto pos = ly.bar.pos;

			order.set_text("Order");
			order.act = [this](){
				order_menu.showhide();
			};
			order.box.pos = pos;

			order_menu.init(con, order.get_sub_pos());


			pos[0] += 24 * ex;

			edit.set_text("Edit");
			edit.act = [this](){
				edit_menu.showhide();
			};
			edit.box.pos = pos;

			edit_menu.init(con, edit.get_sub_pos());

			pos[0] += 24 * ex;

		}
	};

	bool MainMenu::handle(Console & con, Event e) {

		// F9 -> focus

		// click -> open submenu
		if (order.handle(con, e)) {
			return 1;
		}

		if (edit.handle(con, e)) {
			return 1;
		}

		if (order_menu.handle(con, e)) {
			return 1;
		}

		if (edit_menu.handle(con, e)) {
			return 1;
		}

		// if focused
			// focused -- submenu is always opened when focused
			// arrow left/right

			// right click -- close menu


		return 0;
	}

	void MainMenu::render(Console & con)
	{
		order.render(con);
		edit.render(con);
		order_menu.render(con);
		edit_menu.render(con);
	}


	struct Command: IWidget{
		void render(Console & con) override;
		bool handle(Console & con, Event e) override;
	};

	bool Command::handle(Console & con, Event e)
	{

		// history up
		if (match(e, {EventPress, KeyUp})) {
			con.history_up();
			return 1;
		}

		// history down
		if (match(e, {EventPress, KeyDown})) {
			con.history_down();
			return 1;
		}

		// enter
		if (match(e, {EventPress, KeyEnter})) {
			con.handle_char(u'\r');
			return 1;
		}

		// backspace
		if (match(e, {EventPress, KeyBackspace})) {
			con.handle_char(u'\b');
			return 1;
		}

		// char entered
		if (match(e, {EventChar})) {
			con.handle_char(e.get_chr());
			return 1;
		}

		// trap all keyboard
		// ??? how to

		return 0;
	}


	void Command::render(Console & con)
	{
		auto ln = con.output.size();
		v2s pos = v2s(ly.map.pos[0], ly.map.pos[1]);

		for (auto& line: boost::adaptors::reverse(con.output))
		{
			
			render_fill(
				{pos, get_text_dim(font_tiny(), line)},
				{0,0,0,128}
			);
			render_text(
				pos,
				font_tiny(), ColorFont,
				line
			);

			pos[1] += font_tiny().height;
		}

		// command line with cursor
		render_text(
			ly.scr.pos,
			font_tiny(), ColorFont, 
			con.buffer + "_"
		);

	}



	struct MainFrame: IWidget
	{
		void render(Console & con) override;
		bool handle(Console & con, Event e) override;

		Command command;
		MainMenu main_menu;
		MapView map_view;
		SidePanel side_panel;
		CityView city_view;

		MainFrame(Console & con): main_menu(con) {}
	};

	void MainFrame::render(Console & con)
	{
		con.reg.clear();


		// int w = app.get_ctx_dim()[0];
		// int h = app.get_ctx_dim()[1];


		render_drag_clear(con);




		if (con.mode == Console::Mode::COLONY)
		{
			// city
			city_view.render(con);
		}
		else {

			// map-view
			map_view.render(con);


			// side panel
			side_panel.render(con);

			// vline left of the panel
			render_fill(
				{v2s(ly.pan.pos[0] - ly.line, ly.pan.pos[1]), {ly.line, ly.pan.dim[1]}},
				ColorBlack
			);


		}


		// top-bar
		{
			// top-bar background
			render_area({ly.bar.pos, ly.bar.dim}, res(WOODTILE));

			// black hline under the top-bar
			render_fill(
				{{ly.bar.pos[0], ly.bar.end()[1]}, {ly.bar.dim[0], ly.line}},
				{0,0,0,255}
			);

			// top-bar content: cmd or menu
			if (con.is_active())
			{
				command.render(con);
			}
			else {
				main_menu.render(con);
			}
		}



		render_cursor(con);

	}

	bool MainFrame::handle(Console & con, Event e)
	{

		// toogle terminal
		if (match(e, {EventChar, '`'}))
		{
			con.set_active(not con.is_active());
			return 1;
		}

		if (con.is_active()) {
			if (command.handle(con, e)) {
				return 1;
			}
		}
		else {
			if (main_menu.handle(con, e)) {
				return 1;
			}
		}

		if (con.mode == Console::Mode::COLONY)
		{
			if (city_view.handle(con, e)) {
				return 1;
			}
		}
		else {

			if (map_view.handle(con, e)) {
				return 1;
			}

			if (side_panel.handle(con, e)) {
				return 1;
			}
		}

		return 0;
	}




	void render(col::Env & env, col::Console & con, int verbose)
	{
		win.clear();
		// render widgets stack
		for (auto * w: con.widgets) {
			w->render(con);
		}
		win.flip();
	}




	v2s get_loffset(int l) {
		switch(l){
			case 0: return v2s(0,0) * ly.scale;
			case 1: return v2s(8,0) * ly.scale;
			case 2: return v2s(8,8) * ly.scale;
			case 3: return v2s(0,8) * ly.scale;
		}
		fail("invalid 'l'");
	}

	
	void start_gui(Console & con)
	{
		con.push_widget<MainFrame>(con);
	}

}
