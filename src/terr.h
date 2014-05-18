#ifndef TERR34_H
#define TERR34_H

#include "phys.h"
#include "biome.h"
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

	using Alt = uint8;

	Alt const SEA_LEVEL = 0;
	Alt const FLATLAND_LEVEL = 1;
	Alt const HILL_LEVEL = 2;
	Alt const MOUNTAIN_LEVEL = 3;


	struct Terr: Place, Workplace {
		using Id = Coords;

		Biome biome;
		Phys phys;
		Alt alt;

		vector<Unit*> units;
		Colony* colony;


		// Constructors
		//Terr(): biome(BiomePlains), phys(PhysNone), alt(SEA_LEVEL), colony(nullptr) {}

		explicit Terr(Biome const& biome=BiomePlains, Phys const& phys = PhysNone, Alt const& alt = SEA_LEVEL):
			biome(biome), phys(phys), alt(alt), colony(nullptr)
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
			if (has(PhysWorker)) {
				return false;
			}
			if (exec) {
				add(PhysWorker);
			}
			return true;
		}

		bool leave(bool const& exec=1) {
			if (!has(PhysWorker)) {
				return false;
			}
			if (exec) {
				sub(PhysWorker);
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

		bool has(Phys const& p) const {
			return phys & p;
		}

		void add(Phys const& p) {
			phys = phys | p;
		}

		void sub(Phys const& p) {
			phys = phys & (!p);
		}

		Biome const& get_biome() const { return biome; }
		Terr & set_biome(Biome const& biome) { this->biome = biome; return *this; }

		Alt const& get_alt() const { return alt; }
		Terr & set_alt(Alt const& alt) { this->alt = alt; return *this; }

		Travel get_travel();
		int8 get_movement_cost(Travel const& t);

		bool is_water_tile() const {
			return alt < 1;
		}



	// private
		// max speed
		int8 get_land_movement_cost();
		int8 get_naval_movement_cost();

	};





}
#endif
