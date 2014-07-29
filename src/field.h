#ifndef FIELD_H
#define FIELD_H

#include "item.h"
#include "unit.h"

namespace col{

	struct Terr;

	struct Workplace2 {
		Item proditem{ItemNone};
		vector<Unit*> workers;

		Item const& get_proditem() const { return proditem; }

		int get_slots() const { return 1; }
		int get_space_left() const { return get_slots() - workers.size(); }

		void add_worker(Unit & u) { workers.push_back(&u); }
		void sub_worker(Unit & u) { workers.erase(find(workers.begin(), workers.end(), &u)); }

		Workplace2 & set_proditem(Item const& item) { proditem = item; return *this; }
	};

	struct Field: Workplace2{
		Terr *terr{nullptr};

		Terr const& get_terr() const { return *terr; }

		Field & set_terr(Terr & terr) { this->terr = &terr; return *this; }

		Field() {}
		Field(Terr & t): terr(&t) {}
	};

	inline bool operator==(Field const& self, Field const& other) {
		return self.terr == other.terr;
	}

}


#endif
