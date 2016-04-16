#pragma once

#include "terr.hpp"
#include "workplace.hpp"

namespace col{



	struct Field: Workplace{
		using Id = int8;
		PlaceType::type place_type() override { return PlaceType::Field; }

		Terr *terr{nullptr};

		Terr const& get_terr() const { return *terr; }

		Field & set_terr(Terr & terr) { this->terr = &terr; return *this; }

		Field() {}
		Field(Terr & t): terr(&t) {}

		int get_slots() const override { return 1; }

	
		virtual Amount get_prod(Env const& env, Unit const& unit, Item const& item) const override;
		

	};

	bool operator==(Field const& self, Field const& other);

}

