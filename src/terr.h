#ifndef TERR34_H
#define TERR34_H

#include "objs.h"
#include "colony.h"

namespace col{


	struct TerrType{
		using Id = uint8;

		string name;
		Id id;
		uint8 movement_cost;
		uint8 movement_type;
		uint8 defensive;


		TerrType() {}

		TerrType(vector<string> const& xs) {
			assert(xs.size() >= 16);

			name = trim_copy(xs[0]);
			id = stoi(xs[1]);
			movement_cost = stoi(xs[2]) * SPACE_UNIT;
			defensive = stoi(xs[3]);
			movement_type = stoi(xs[15]);
		}

	};

	struct Terr: Place, Workplace {
		using Id = Coords;

		Biome::type biome;
		Phys::type phys;

		vector<Unit*> units;
		Colony* colony;

		Terr(): colony(nullptr) {}

		explicit Terr(Biome::type const& biome, Phys::type const& phys = Phys::None):
			biome(biome), phys(phys), colony(nullptr)
		{}

		// Place

		PlaceType::type place_type() {
			return PlaceType::Terr;
		}

		// Workplace

		uint16 get_yield(Item const& item, bool const& is_expert) const {
			return 3;
		}

		bool assign(bool const& exec=1) {
			if (has(Phys::Worker)) {
				return false;
			}
			if (exec) {
				add(Phys::Worker);
			}
			return true;
		}

		bool leave(bool const& exec=1) {
			if (!has(Phys::Worker)) {
				return false;
			}
			if (exec) {
				sub(Phys::Worker);
			}
			return true;
		}

		// Terr

		Colony& get_colony() {
			assert(colony != nullptr);
			return *colony;
		}

		Colony const& get_colony() const {
			assert(colony != nullptr);
			return *colony;
		}

		bool has_colony() const {
			return colony != nullptr;
		}


		Terr& set_colony(Colony &c) {
			colony = &c;
			return *this;
		}

		bool has_units() const {
			return units.size();
		}



		bool has(Phys::type const& p) const {
			return phys & p;
		}

		void add(Phys::type const& p) {
			phys |= p;
		}

		void sub(Phys::type const& p) {
			phys &= !p;
		}

		Travel get_travel();

		Terr& set_biome(Biome::type const& b) { biome = b; return *this; }

		int8 get_movement_cost(Travel const& movement_type);


		// max speed

		int8 get_land_movement_cost();
		int8 get_naval_movement_cost();

	};





}
#endif
