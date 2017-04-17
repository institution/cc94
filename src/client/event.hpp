#pragma once

#include "base.hpp"
#include "../front/front.hpp"
#include "../front/input.hpp"
#include <vector>
#include <functional>

namespace col {

	using namespace front::input;

	// event, pattern, matching, rules
	
	struct Widget;
	
	struct Event	
	{
		
		uint8_t 
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
		
		bool has_type() const { return type != EventNone; }
		bool has_key() const { return type != KeyNone; }
		bool has_pos() const { return has_pos_; }    
		bool has_dim() const { return has_dim_; }  
		bool has_chr() const { return has_chr_; }  
		bool has_box() const { return has_pos_ and has_dim_; }  
		bool has_fkb() const { return fkb != nullptr; }  
			 
		void set_type(EventType x) { type = x; }	
		void set_key(Key x) { key = x; }
		void set_pos(v2s x) { pos = x; has_pos_ = 1; }
		void set_dim(v2s x) { dim = x; has_dim_ = 1; }
		void set_chr(char x) { chr = x; has_chr_ = 1; }
		void set_box(b2s x) { set_pos(x.pos); set_dim(x.dim); }
		void set_fkb(Widget * x) { fkb = x; }		
		void set_mod(Mod x) { mod = x; }
		
		void unset_mod() { mod = 0; }
		void unset_pos() { has_pos_ = 0; }
		void unset_dim() { has_dim_ = 0; }
		void unset_chr() { has_chr_ = 0; }
		
			
		EventType get_type() const { return type; }
		Key get_key() const { return key; }
		Mod get_mod() const { return mod; }  	
		v2s get_pos() const { return pos; }    
		v2s get_dim() const { return dim; }  
		char get_chr() const { return chr; }  
		b2s get_box() const { return b2s(pos,dim); }  
		Widget * get_fkb() const { return fkb; }
		
		
		void unset() {
			unset_mod();
			unset_pos();
			unset_dim();
			unset_chr();			
		}
		
		
		
		// cc
		Event(Event const& o) = default;
		
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

		// char entered
		Event(EventType t, char c) {
			set_type(t);			
			unset_mod();
			unset_pos();
			unset_dim();
			set_chr(c);
		}

		// char+focus entered
		Event(Widget * fkb, EventType t) {
			set_type(t);
			set_fkb(fkb);
			
			unset_mod();
			unset_pos();
			unset_dim();
			unset_chr();
		}

		// key+focus press/release	
		Event(Widget * fkb, EventType t, Key k) {
			set_type(t);
			set_key(k);
			set_fkb(fkb);
			
			unset_mod();
			unset_pos();
			unset_dim();
			unset_chr();
		}
		
		// key+mod+focus press/release	
		Event(Widget * fkb, EventType t, Key k, Mod m) {
			set_type(t);
			set_key(k);
			set_mod(m);
			set_fkb(fkb);
			
			unset_pos();
			unset_dim();
			unset_chr();
		}

		// key press/release	
		Event(EventType t, Key k) {
			set_type(t);
			set_key(k);
			
			unset_mod();
			unset_pos();
			unset_dim();
			unset_chr();
		}
		
		// key+mod press/release	
		Event(EventType t, Key k, Mod m) {
			unset();
			set_type(t);
			set_key(k);
			set_mod(m);
			
			
		}

		// key press/release over area	
		Event(EventType t, Key k, v2s p, v2s d) {
			unset();
			set_type(t);
			set_key(k);		
			set_pos(p);
			set_dim(d);		
		}

		// key+mod press/release over area	
		Event(EventType t, Key k, Mod m, v2s p, v2s d) {
			unset();
			set_type(t);
			set_key(k);
			set_mod(m);
			set_pos(p);
			set_dim(d);		
		}
		
		// over area
		Event(EventType t, v2s p, v2s d) {
			unset();
			set_type(t);
			set_pos(p);
			set_dim(d);		
		}

		// mod over area
		Event(EventType t, Mod m, v2s p, v2s d) {
			unset();
			set_type(t);
			set_mod(m);
			set_pos(p);
			set_dim(d);		
		}

		// mod
		Event(EventType t, Mod m) {
			unset();
			set_type(t);
			set_mod(m);
		}	
		
		// event
		Event(EventType t) {
			unset();
			set_type(t);		
		}


		
		bool match_type(EventType t) const {
			return has_type() and t == get_type();
		}
		
		bool match_key(Key k) const {
			return has_key() and k == get_key();			
		}
		
		bool match_chr(char c) const {
			return has_chr() and c == get_chr();			
		}
				
		bool match_mod(Mod m) const {
		
			// pattern m
			// 1 -- require pressed
			// 0 -- don't care
			
			// event m
			// 1 -- mod set
			// 0 -- mod not set
			
			// true if all required pressed
			
			return (get_mod() & m) == m;
		}
		
		bool match_box(b2s const& box) const {
			return has_pos() and geo2::overlap(box, get_pos());
		}
		
	};


	std::ostream & operator<<(std::ostream & o, Event const& x);



	/// Match event 'e to pattern 'p
	bool match(Event const& e, Event const& p, bool verbose = 0);


	struct Rules 
	{
		using Cb0 = std::function<void()>;
		using Cb1 = std::function<void(Event&)>;
			
		struct Rule{
			Event pattern;
			
			Cb0 cb0{nullptr};
			Cb1 cb1{nullptr};
			
			Rule() = default;
			Rule(Event const& p): pattern(p) {}
			Rule(Event const& p, Cb0 cb0): pattern(p), cb0(cb0) {}
			Rule(Event const& p, Cb1 cb1): pattern(p), cb1(cb1) {}
			
		};
		
		
		std::vector<Rule> xs;
				
		void on(Event const& p) { xs.emplace_back(p); }
		void on(Event const& p, Cb0 cb0) { xs.emplace_back(p, cb0); }
		void on(Event const& p, Cb1 cb1) { xs.emplace_back(p, cb1); }
						
		bool handle_event(Event & e, bool verbose=0);
		
		void clear() { xs.clear(); }
	};
		
	
	
	
}


