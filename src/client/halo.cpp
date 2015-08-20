#include "halo.h"


namespace halo{
	
	
	std::ostream & operator<<(std::ostream & o, Pattern const& p) {
		o << "Pattern(";
		if (p.dev) o << "dev=" << static_cast<int>(*p.dev) << ",";
		if (p.event) o << "event=" << static_cast<int>(*p.event) << ",";
		if (p.button) o << "button=" << static_cast<int>(*p.button) << ",";
		if (p.key) o << "key=" << static_cast<int>(*p.key) << ",";
		if (p.mod) o << "mod=" << static_cast<int>(*p.mod) << ",";
		if (p.unicode) o << "unicode=" << static_cast<int>(*p.unicode) << ",";
		if (p.area) {
			o << "pos=" << (*p.area).pos << ",";
			o << "dim=" << (*p.area).dim << ",";
		}
		o << ")";
		return o;
	}
	
	
	bool handle_event(Patterns const& ps, Pattern const& q, bool verbose) {
		
		//print("event = %||\n", q);
		
		for (int i = ps.size(); 0 < i; --i) {
			auto& p = ps[i-1];
			
			bool t = true;
			t = t and (!p.dev or (q.dev and *p.dev == *q.dev));
			t = t and (!p.event or (q.event and *p.event == *q.event));
			t = t and (!p.button or (q.button and *p.button == *q.button));
			t = t and (!p.key or (q.key and *p.key == *q.key));
			t = t and (!p.mod or (q.mod and (*p.mod == *q.mod)));
			t = t and (!p.unicode or (q.unicode and *p.unicode == *q.unicode));
			t = t and (!p.area or (q.area and col::overlap(*p.area, (*q.area).pos)));
			
			/*if (verbose and q.event and *q.event != Event::Hover) {
				print("halo event: %||: %||\n", p, t);
			}*/
			
			if (t) {
				if (p.callback) {
					p.callback();
				}
				else if (p.callback2) {
					p.callback2(q);
				}
				return true;
			}
			// continue
		}
		return false;
	}
	
	
	
}
