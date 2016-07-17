#pragma once

#include <vector>
#include <functional>

namespace col {

	// event, pattern, matching, rules
	
	struct Widget;
	
	struct Event	
	{
		uint8_t 
			has_mod_:1,
			has_pos_:1,
			has_dim_:1,
			has_chr_:1
		;
		
		Widget * fkb{nullptr};
		EventType type{EventNone};
		Key key{KeyNone};
		Mod mod{0};	
		v2s pos{0,0}; 
		v2s dim{0,0};
		char chr{'?'};
		
		bool has_type() const { return type != TypeNone; }
		bool has_key() const { return type != KeyNone; }
		bool has_mod() const { return has_mod_; }  	
		bool has_pos() const { return has_pos_; }    
		bool has_dim() const { return has_dim_; }  
		bool has_chr() const { return has_chr_; }  
		bool has_box() const { return has_pos_ and has_dim_; }  
		bool has_fkb() const { return fid != nullptr; }  
			 
		void set_type(EventType x) { type = x; }	
		void set_key(Key x) { key = x; }
		void set_mod(Mod x) { mod = x; has_mod_ = 1; }
		void set_pos(v2s x) { pos = x; has_pos_ = 1; }
		void set_dim(v2s x) { dim = x; has_dim_ = 1; }
		void set_chr(char x) { chr = x; has_chr_ = 1; }
		void set_box(b2s x) { set_pos(x.pos); set_dim(x.dim); }
		void set_fkb(Widget * x) { fkb = x; }
			
		EventType get_type() const { return type; }
		Key get_key() const { return key; }
		Mod get_mod() const { return mod; }  	
		v2s get_pos() const { return pos; }    
		v2s get_dim() const { return dim; }  
		char get_chr() const { return chr; }  
		b2s get_box() const { return b2s(pos,dim); }  
		Widget * get_fkb() const { return fkb; }
		
		// any
		Event() {}
		
		// non
		Event(EventType t, Key k, Mod m, v2s p, v2s d, char c) {
			set_type(t);
			set_key(k);
			set_mod(m);
			set_pos(p);
			set_dim(d);
			set_chr(c);
		}

		// character entered
		Event(EventType t, char c) {
			set_type(t);
			set_chr(c);
		}

		// key+focus press/release	
		Event(Widget * fkb, EventType t, Key k) {
			set_type(t);
			set_key(k);
			set_fkb(fkb);
		}
		
		// key+mod+focus press/release	
		Event(Widget * fkb, EventType t, Key k, Mod m) {
			set_type(t);
			set_key(k);
			set_mod(m);
			set_fkb(fkb);
		}

		// key press/release	
		Event(EventType t, Key k) {
			set_type(t);
			set_key(k);
		}
		
		// key+mod press/release	
		Event(EventType t, Key k, Mod m) {
			set_type(t);
			set_key(k);
			set_mod(m);
		}

		// key press/release over area	
		Event(EventType t, Key k, v2s p, v2s d) {
			set_type(t);
			set_key(k);		
			set_pos(p);
			set_dim(d);		
		}

		// key+mod press/release over area	
		Event(EventType t, Key k, Mod m, v2s p, v2s d) {
			set_type(t);
			set_key(k);
			set_mod(m);
			set_pos(p);
			set_dim(d);		
		}
		
		// over area
		Event(EventType t, v2s p, v2s d) {
			set_type(t);
			set_pos(p);
			set_dim(d);		
		}

		// mod over area
		Event(EventType t, Mod m, v2s p, v2s d) {
			set_type(t);
			set_mod(m);
			set_pos(p);
			set_dim(d);		
		}

		// mod
		Event(EventType t, Mod m) {
			set_type(t);
			set_mod(m);
		}	
		
		// event
		Event(EventType t) {
			set_type(t);		
		}

	};


	std::ostream & operator<<(std::ostream & o, Event const& x);



	/// Match event 'e to pattern 'p
	bool match(Event const& e, Event const& p);
	

	struct Rule{
		Event pattern;
		std::function<void()> cb0;
		std::function<void(Event&)> cb1;
		
		Rule() = default;
		Rule(Event e, std::function<void()> cb0): e(e), cb0(cb0) {}
		Rule(Event e, std::function<void(Event&)> cb1): e(e), cb1(cb1) {}
	};

	using Rules = std::vector<Rule>;
		
	bool handle_event(Rules const& rs, Event const& e, bool verbose=0);
	
	
}


