

int16_t const ex = 5;
int16_t const em = 7;

int16_t S(int16_t x) { return 4*s; }

struct Widget; 

Widget * g_focus{nullptr};

struct Widget 
{
	v2s pos{0,0};
	v2s dim{0,0};

	//bool has_pos{false};
	//bool has_dim{false};
		
	Widget * next{nullptr};
	Widget * down{nullptr};
	
	Widget * last() {
		auto * z = this;
		while (z->next) {
			z->next = z->next;
		}
		return z;
	}
	
	void add(Widget * w) 
	{
		if (down) {
			down->last()->next = w;
		}
		else {
			down = w;
		}		
	}
	
	
	Widget * adds(Widget * w) {
		add(w);
		return this;
	}
	
	template <class... Args>
	void adds(Widget * w, Args... args) {
		add(w);
		return adds(args...);
	}
	
	virtual void render(Front & win) {}
	virtual v2s get_dim() { return dim; }
	virtual void set_pos(v2s pos) { this->pos = pos; }
	
	void grab_focus() { g_focus = this; }	
	void clear_focus() { g_focus = nullptr; }
	void has_focus() { return g_focus == this; }
	
	void layout_0(v2s pos) {
		// phase 1: down->up get_dim, dim = min_dim
		// phase 2: up->down set_pos 
		
		get_dim();
		set_pos(pos);
	
	}
};








struct Frame: Widget 
{

	v2s get_dim() override {
		v2s dd{0,0};
		if (down) {
			dd = down->get_dim();
		}
		dim = v2s(2*S(2), 2*S(2)) + dd;
		return dim;
	}	
	
	void set_pos(v2s pos) override {
		this->pos = pos;
		if (down) {
			down->set_pos(pos + v2s(S(3), S(3)));
		}		
	}
	
	void render(Console & con) override 
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


struct Line: Widget {
	
	int16_t adv{0};
	
	v2s get_dim() override 
	{
		auto * w = down;
		while (w) {
			auto wdim = w->get_dim();
			
			dim[0] += wdim[0] + adv;
			if (dim[1] < wdim[1]) {
				dim[1] = wdim[1];
			}			
			w = w->next;
		}
		return dim;
	}
	
	void set_pos(v2s p) override 
	{
		pos = p;
		
		auto * w = down;
		auto c = pos[0];
		while (w) {
						
			w->set_pos(v2s(c, pos[1]))
			
			c += w->dim[0] + adv;
			
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


template <class T>
struct EditNum: Widget {
	
	T value;
	T min, max;
	
	int8_t len;
	
	v2s get_dim() override 
	{
		dim = v2s(2*S(2), 2*S(2)) + v2s(maxlen * ex, line_height);
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
			inner.pos, inner.dim, align, 
			StyleDefault.font, StyleDefault.fg,
			"123"
		);
		
		// render cursor
		if (has_focus()) {
		
			render_text_at(win,
				v2s(t.pos[0] + t.dim[0], t.pos(1))
				StyleDefault, 
				"_"
			)
		}
		
		// input
		con.on(this, EventPress, KeyBackspace, [this](Event & e){
			if (value.size() > 0) {
				value.pop_back();
			}
		});
	
		con.on(this, EventCharInput, [this](Event & e) {
			value += e.get_chr();			
		});
		
		con.on(EventPress, KeyLMB, pos, dim, [this](Event & e) {
			grab_focus();			
		});
		
		// if focus and p.match(EventPress, KeyA, ModCtrl)
		
	}
	
	
};

/*
Shortcuts/menu/link  unit orders
Mod+mouse tile/unit selection
menu/link/shortcuts button/hyperlink
Fx reports

Actions on main view
Actions on city view
Actions on europe view
*/







struct Cross: Widget {



};


struct Button: Widget {

};





struct FrameNum: Frame {

	Widget * edit;
	function<void(int)> onenter;
	
	FrameNum(function<void(int)> onenter): onenter(onenter) {
	
		edit = new EditInt(amt, 6).limit(0,999);
		
		adds(
			new Cross().adds(
				new Line().adds(
					new Label("Num", 5),
					edit,
				),
				new Button("OK", 8, [](){
					onenter(edit.get_value());
				})
			)
		);
		
	}
		
	void render() override {
		Frame::render();
		
		on(KeyEnter, [](){
			onenter(edit.get_value());
		});
		
		
	}

};




/*
b2s get_inner_box(Cell const& c) const {
	auto d = c.padding + c.border;
	return b2s(c.box.pos + d, c.box.dim - 2*d);
}
*/


/*
struct ShipDesign: Widget {

	int height{0};
	int width{0};

	
	
	auto f = Frame()
		Cross()
			Title("AAA", 9)		
			Line()
				Cross()
					Line()
						Label("height", 10)
						EditInt(width, 6).limit(0,20)						
					Line()
						Label("width", 10)
						EditInt(width, 6).limit(0,20)						
				Cross()
					Image()
			Button("OK", 8)
				
				
	f.layout();

};*/
		
		
