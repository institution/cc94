#include "halo.h"


namespace halo{
	
	bool handle_event(Patterns const& ps, Pattern const& q) {
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
