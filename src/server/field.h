#ifndef FIELD_H_349068896
#define FIELD_H_349068896

#include "terr.h"
#include "workplace.h"

namespace col{



	struct Field: Workplace{
		using Id = int8;
		PlaceType::type place_type() { return PlaceType::Field; }

		Terr *terr{nullptr};

		Terr const& get_terr() const { return *terr; }

		Field & set_terr(Terr & terr) { this->terr = &terr; return *this; }

		Field() {}
		Field(Terr & t): terr(&t) {}

		int get_slots() const { return 1; }

	
		virtual Amount get_prod(Item const& item, bool const& is_expert) const;
		

	};

	bool operator==(Field const& self, Field const& other);

}


#endif
