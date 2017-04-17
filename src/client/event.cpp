#include "event.hpp"

namespace col {

	
	std::ostream & operator<<(std::ostream & o, Event const& x) 
	{
		// something like: Event(EventPress,KeyA,ModLCtrl,ModLShift)
		
		print(o, "Event(");
		if (x.has_type()) {
			print(o, "%||,", get_eventtype_name(x.get_type()));
		}
		if (x.has_key()) {
			print(o, "%||,", get_key_name(x.get_key()));
		}
		{
			print(o, "[");
		
			for (uint32_t i=0; i<32; ++i) {
				auto f = uint32_t(1 << i);				
				if (x.get_mod() & f) {
					print(o, "%||,", get_mod_name(f));
				}
			}			
			
			print(o, "],");
		}
		if (x.has_pos()) {
			print(o, "pos=%||,", x.get_pos());
		}
		if (x.has_dim()) {
			print(o, "dim=%||,", x.get_dim());
		}
		if (x.has_chr()) {
			print(o, "chr=%||,", x.get_chr());
		}
		if (x.has_fkb()) {
			print(o, "fkb=%||", x.get_fkb());
		}
		print(o, ")");
		return o;
	}
	


	/// Match event 'e to pattern 'p
	bool match(Event const& e, Event const& p, bool verbose) {
		
		if (verbose) print("matching %||\n", e);
		
		if (p.has_type() and not e.match_type(p.get_type())) {
			if (verbose) print("bad type\n");
			return false;		
		}
		
		if (p.has_key() and not e.match_key(p.get_key())) {
			if (verbose) print("bad key\n");
			return false;			
		}
		
		if (not e.match_mod(p.get_mod())) {
			if (verbose) print("bad mod\n");
			return false;		
		}
		
		if (p.has_chr() and not e.match_chr(p.get_chr())) {
			if (verbose) print("bad chr\n");
			return false;		
		}
		
		if (p.has_box() and not e.match_box(p.get_box())) {
			if (verbose) print("bad box\n");
			return false;			
		}
		
		if (verbose) print("match!\n");
		return true;
	}
	
	
	
	bool Rules::handle_event(Event & e, bool verbose) {
		
		if (verbose and e.get_type() != EventMotion) {
			print("event is %||\n", e);
		}
		
		for (int i = xs.size(); 0 < i; --i) 
		{
			auto & rule = xs[i-1];
			auto & p = rule.pattern;
			
			if (verbose and e.get_type() != EventMotion) {
				//print("pattern is %||\n", p);
			}
				
			
			
			if (match(e,p)) 
			{
				if (verbose) {
					
					print("match\n");
				}
			
				if (rule.cb0) {
					rule.cb0();
				}
				else if (rule.cb1) {
					rule.cb1(e);
				}
				
				// stop after match
				return true;
			}			
		}
		return false;
	}


}

