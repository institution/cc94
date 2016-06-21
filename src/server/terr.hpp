#pragma once

#include <bitset>
#include "phys.hpp"
#include "biome.hpp"
#include "objs.hpp"
#include "unit.hpp"
#include "nation.hpp"

namespace col{



	using Alt = int8_t;
	
	
	Alt const 
		AltNone = 0,
		AltSea = 1,
		AltFlat = 2,
		AltHill = 3,
		AltMountain = 4,
		AltEnd = 5;
		
	inline char const* get_alt_name(Alt alt) {
		switch(alt) {
			case AltNone: return "<InvalidAlt:None>";
			case AltSea: return "Sea";
			case AltFlat: return "Flat";
			case AltHill: return "Hill";
			case AltMountain: return "Mountain";
			case AltEnd: return "<InvalidAlt:End>";
		}
		return "<InvalidAlt>";
	}
		
	Alt const AltDefault = AltSea;
	Biome const BiomeDefault = BiomeDesert;

	struct Colony;
	struct Unit;

	using Vision = uint32_t;

	struct Terr {
		using Id = Coords;

		Alt alt{AltDefault};
		Biome biome{BiomeDefault};
		Phys phys{PhysNone};
		
		vector<Unit*> units;
		Colony * colony{nullptr};
		
		Vision vision{0};
		Vision discovered{0};

		explicit Terr(Alt const& alt = AltDefault, Biome const& biome = BiomeDefault, Phys const& phys = PhysNone):
			biome(biome), phys(phys), alt(alt)
		{}

		~Terr() {
			assert(colony == nullptr);
			assert(units.size() == 0);
		}

		void clear_vision() {
			vision = 0;	
		}
		
		void mark_vision(Nation::Id id) {
			vision |= (Vision(1) << id);
			discovered |= (Vision(1) << id);
		}

		void set_vision(Nation::Id id, bool val) {
			vision |= (Vision(1) << id);
		}
		
		bool get_discovered(Nation::Id id) const {
			return discovered & (Vision(1) << id);
		}
				
		bool get_vision(Nation::Id id) const {
			return vision & (Vision(1) << id);
		}



		void add(Unit & u) {
			units.push_back(&u);
		}

		void sub(Unit & u) {
			units.erase(find(units.begin(), units.end(), &u));
		}

		bool assign(bool const& exec=1) {
			if (has_phys(PhysWorker)) {
				return false;
			}
			if (exec) {
				add_phys(PhysWorker);
			}
			return true;
		}

		bool leave(bool const& exec=1) {
			if (!has_phys(PhysWorker)) {
				if (!exec) {
					throw Error("no worker to leave this place");
				}
				else {
					throw Error("no worker to leave this place");
				}
			}
			if (exec) {
				sub_phys(PhysWorker);
			}
			return true;
		}

		// Terr
		Colony & get_colony() {
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

		// features
		bool has_phys(Phys const& p) const {
			return phys & p;
		}

		void add_phys(Phys const& p) {
			//print("add_phys: %|| | %|| -> %||\n", int(phys), int(p), int(phys | p));
			phys = phys | p;			
		}
		
		void sub_phys(Phys const& p) {
			phys = phys & (~p);
		}
		
		void set_phys(Phys mask, Phys value) {
			phys = (phys & (~mask)) | (value & mask);
		}
		
		void switch_phys(Phys mask) {
			auto p = get_phys(mask);
			Phys first = mask & ~(mask - 1);   //  00111100 -> 00000100
			set_phys(mask, (p == 0) ? first : (p << 1));	
		}

		Phys get_phys(Phys mask) const { return phys & mask; }

		Phys const& get_phys() const { return phys; }
		void set_phys(Phys const& phys) { this->phys = phys; }

		Biome const& get_biome() const { return biome; }
		Terr & set_biome(Biome const& biome) { this->biome = biome; return *this; }

		Alt const& get_alt() const { return alt; }
		Terr & set_alt(Alt const& alt) { this->alt = alt; return *this; }

		Travel get_travel() const;

	};


	inline Travel Terr::get_travel() const {
		if (alt == AltSea) {
			return TravelSea;
		}
		else {
			if (colony != nullptr) {
				return TravelSea | TravelLand;
			}
			else {
				return TravelLand;
			}
		}
		fail("ERROR: Terr::get_travel: Unknown travel type\n");		
	}






}
