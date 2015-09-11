#ifndef WORKPLACE_H
#define WORKPLACE_H

#include "item.h"
#include "unit.h"
#include "base.h"

namespace col{

	struct Unit;

	// ItemSlot
	struct Slot {
		Amount amount;
	};

	// make unit/build
	// TODO: can be used to implement grazing animals
	struct Task {
		Amount progress{0};
		Makeable const* what{nullptr};
		
		void add(Amount num) { progress += num; } 
		Amount get() const { return progress; }
		Amount cap() const { return what->get_cost(); }
		
		void reset(Makeable const* m) { 
			progress = 0;
			what = m;
		}
		
		void reset() { reset(nullptr); }
		
		explicit operator bool() const { return what; }
		
		Task() = default;		
		Task(Makeable const* m) { 
			reset(m);
		}
		
	};
	

	struct Workplace{
		
		virtual PlaceType::type place_type() = 0;
		
		Task task;
		
		Item proditem{ItemNone};
				
		vector<Unit*> units;

		virtual Item const& get_proditem() const { return proditem; }
				
		virtual Item const& get_consitem() const { return ItemNone; }
			

		virtual int get_slots() const { return 1; }
		int get_space_left() const { return get_slots() - units.size(); }

		void add(Unit & u) { units.push_back(&u); }
		void sub(Unit & u) { units.erase(find(units.begin(), units.end(), &u)); }

		auto get_units() -> decltype(units)& { return units; }
		auto get_units() const -> decltype(units) const& { return units; }

		Workplace & set_proditem(Item const& item) { proditem = item; return *this; }

		void set_task(Makeable const& mk) { 
			task.reset(&mk); 
		}
		
		virtual Amount get_prod(Item const& item, Amount const& base) const { return 0; };
		virtual Amount get_cons(Item const& item, Amount const& base) const { return 0; };

	};


}


#endif
