#ifndef FIELD_H
#define FIELD_H

#include "workplace.h"

namespace col{

	struct Terr;



	struct Field: Workplace{
		PlaceType::type place_type() { return PlaceType::Field; }

		Terr *terr{nullptr};

		Terr const& get_terr() const { return *terr; }

		Field & set_terr(Terr & terr) { this->terr = &terr; return *this; }

		Field() {}
		Field(Terr & t): terr(&t) {}

		uint16 get_yield(bool const& is_expert) const;

		int get_slots() const { return 1; }



	};

	inline bool operator==(Field const& self, Field const& other) {
		return self.terr == other.terr;
	}

}


#endif
