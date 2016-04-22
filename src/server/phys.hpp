#pragma once

#include "col.hpp"
#include <unordered_map>

namespace col {

/*
	namespace detail {
		struct Phys;
	}

	using Phys = distinct::Enum<detail::Phys,uint16>;
*/

	using Phys = uint16_t;
	Phys const
		PhysTransit{1 << 0},      // Road for land tiles, SeaLane for water
		PhysForest{1 << 1},
		PhysPlow{1 << 2},		
		PhysMinorRiver{1 << 3},
		PhysMajorRiver{1 << 4},		
		PhysWorker{1 << 5};
	
	Phys const PhysNone = 0;	
	Phys const PhysRoad = PhysTransit;
	Phys const PhysSeaLane = PhysTransit;
	Phys const PhysRiver = PhysMinorRiver | PhysMajorRiver;
	Phys const PhysForestPlow = PhysPlow | PhysForest;
	Phys const PhysAlterable = PhysRoad | PhysForest | PhysPlow;
				
	// plow xor forest
	// majorriver xor minorriver
	// road xor sealane
	
	
	inline Phys next_phys(Phys mask, Phys p) {
		p = mask & p;
		if (p == 0) {
			return mask & ~(mask - 1);   //  00111100 -> 00000100
		}
		else {
			return mask & (p << 1);
		}
	}
		
	unordered_map<Phys, char const*> const phys_names {
		{PhysSeaLane, "SeaLane"},
		{PhysRoad, "Road"},
		{PhysForest, "Forest"},
		{PhysMinorRiver, "MinorRiver"},
		{PhysMajorRiver, "MajorRiver"},
		{PhysPlow, "Plow"}
	};

	inline char const* get_phys_name(Phys phys) {
		for (auto& item: phys_names) {
			if (item.first == phys) {
				return item.second;
			}
		}
		return "None";
	}
	

	inline Phys get_phys_by_name(char const* name) {
		for (auto& item: phys_names) {
			if (strcmp(item.second, name) == 0) {
				return item.first;
			}
		}
		return PhysNone;
	}
	
	inline Phys get_phys_by_name(string const& name) {
		return get_phys_by_name(name.c_str());
	}
	
}
