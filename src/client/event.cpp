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
		if (x.has_mod()) {
			for (uint32_t i=0; i<16; ++i) {
				auto m = x.get_mod() & uint32_t(0 << i);
				if (m) {
					print(o, "%||,", get_mod_name(m));
				}
			}			
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
	bool match(Event const& e, Event const& p) {
		if (not (not p.has_fkb() or (e.has_fkb() and p.get_fkb() == e.get_fkb()))) return false;
		if (not (not p.has_type() or (e.has_type() and p.get_type() == e.get_type()))) return false;
		if (not (not p.has_key() or (e.has_key() and p.get_key() == e.get_key()))) return false;
		if (not (not p.has_mod() or (e.has_mod() and p.get_mod() == e.get_mod()))) return false;
		if (not (not p.has_chr() or (e.has_chr() and p.get_chr() == e.get_chr()))) return false;
		if (not (not p.has_box() or (e.has_pos() and geo2::overlap(p.get_box(), e.get_pos())))) return false;
		return true;
	}
	
	
	
	bool handle_event(Rules const& rs, Event const& e, bool verbose) {
		
		if (verbose) {
			print("event is %||\n", e);
		}
		
		for (int i = rs.size(); 0 < i; --i) 
		{
			auto & rule = rs[i-1];
			auto & p = rule.pattern;
			
			if (verbose) {
				print("pattern is %||\n", p);
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

