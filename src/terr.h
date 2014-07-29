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

	Alt const AltSea = 0;
	Alt const AltFlat = 1;
	Alt const AltHill = 2;
	Alt const AltMountain = 3;



	struct Terr: Place {
		using Id = Coords;

		Biome biome;
		Phys phys;
		Alt alt;

		vector<Unit*> units;
		Colony* colony;


		// Constructors
		//Terr(): biome(BiomePlains), phys(PhysNone), alt(SEA_LEVEL), colony(nullptr) {}

		explicit Terr(Alt const& alt = AltSea, Biome const& biome = BiomePlains, Phys const& phys = PhysNone):
			biome(biome), phys(phys), alt(alt), colony(nullptr)
		{}

		/*
		move_unit(Unit &u, Place &from) {

		}

		move_cargo(Cargo &c, Place &from) {

		}*/

		// Place
		PlaceType::type place_type() {
			return PlaceType::Terr;
		}

		// Workplace
		uint16 get_yield(Item const& item, bool const& is_expert) const;

		vector<Item> get_proditems() const {
			array<Item,9> field_items = {
				ItemFood,ItemSugar,ItemTobacco,
				ItemCotton,ItemFurs,ItemLumber,
				ItemOre,ItemSilver,ItemFish
			};

			vector<Item> xs;
			for (auto const& item: field_items) {
				if (get_yield(item, 0)) {
					xs.push_back(item);
				}
			}
			return xs;
		}

		int get_space_left() {
			return 999;
		}

		void add(Unit & u) {
			units.push_back(&u);
		}

		void sub(Unit & u) {
			units.erase(find(units.begin(), units.end(), &u));
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
				if (!exec) {
					throw Error("no worker to leave this place");
				}
				else {
					throw Critical("no worker to leave this place");
				}
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
			phys = phys & (~p);
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
