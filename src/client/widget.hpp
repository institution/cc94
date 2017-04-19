#pragma once

//#include "console.hpp"
//#include "renderer.hpp"
//#include "layout.hpp"
//#include "coldef.hpp"

namespace col {

	int16_t const ex = 5;
	int16_t const em = 7;

	inline int16_t S(int16_t x) { return ly.S(x); }

	struct Widget; 

	// Widget * g_focus{nullptr};

	struct Console;
	
	
	struct IWidget 
	{
		virtual void render(Console & con) {}
				
		virtual bool handle(Console & con, Event e) { return 0; }
		
		virtual ~IWidget() {}
	};
	
	
	
	/// return minimal dim
	//virtual v2s get_dim() { return v2s(11,11); }
	
	/// set position
	//virtual void set_pos(v2s pos) { }
	
	
	struct Node
	{
		Node * up{nullptr};
		Node * down{nullptr};
		Node * right{nullptr};
		IWidget * self{nullptr};

		Node() = default;
		Node(IWidget * w): self(w) {}
	};
	
	struct Iter
	{
		Node * n{nullptr};
		
		Iter up() const { return Iter(n->up); }
		Iter down() const { return Iter(n->down); }
		Iter right() const { return Iter(n->right); }
		
		bool has_up() const { return n->up != nullptr; }
		bool has_down() const { return n->down != nullptr; }
		bool has_right() const { return n->right != nullptr; }
		
		//bool has() const { return bool(n); }
		//IWidget & get() { return *n; }
		//IWidget const& get() const { return *n; }
		
		explicit operator bool() const { return bool(n); }
		
		Iter() = default;
		Iter(Node * n): n(n) {}		
	};
	
	struct Widgets 
	{
		//insert
		//remove
		
	
		Node * root{nullptr};
	};
	
	
	
	struct Widget: IWidget
	{
		v2s pos{0,0};
		v2s dim{0,0};

		//bool has_pos{false};
		//bool has_dim{false};
			
		virtual ~Widget() {
			/*if (down) {
				delete down;
			}
			if (next) {
				delete next;
			}*/
		}
		
		Widget * last() {
			/*auto * z = this;
			while (z->next) {
				z->next = z->next;
			}
			return z;*/
			return nullptr;
		}
		
		void add(Widget * w) 
		{
			/*if (down) {
				down->last()->next = w;
			}
			else {
				down = w;
			}*/		
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
		
		virtual void render(Console & con) override {}
		virtual v2s get_dim() { return dim; }
		virtual void set_pos(v2s pos) { this->pos = pos; }
		
		//void grab_focus() { g_focus = this; }	
		//void clear_focus() { g_focus = nullptr; }
		//bool has_focus() { return g_focus == this; }
		
		void layout_0(v2s pos) {
			// phase 1: down->up get_dim, dim = min_dim
			// phase 2: up->down set_pos 
			
			get_dim();
			set_pos(pos);
		
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
			
			
}
