#ifndef WORKPLACE_H
#define WORKPLACE_H

#include "item.h"
#include "unit.h"
#include "base.h"

namespace col{

	struct Workplace: Place{

		Item proditem{ItemNone};
		vector<Unit*> units;

		Item const& get_proditem() const { return proditem; }

		int get_slots() const { return 1; }
		int get_space_left() const { return get_slots() - units.size(); }

		void add(Unit & u) { units.push_back(&u); }
		void sub(Unit & u) { units.erase(find(units.begin(), units.end(), &u)); }

		auto get_units() -> decltype(units)& { return units; }

		Workplace & set_proditem(Item const& item) { proditem = item; return *this; }

		virtual uint16 get_yield(Item const& item, bool const& is_expert) const { return 0; };

	};


}


#endif
