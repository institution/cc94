#ifndef PHYS_H
#define PHYS_H

#include "enum.hpp"

namespace col {

	struct Phys: Enum<> {
		using Enum<>::Enum;
	};


	constexpr
	Phys const
		PhysNone = 0,
		// bits 0 and 1 reserved for height
		// Unused = 1,
		PhysSeaLane = 2,   
		PhysRoad = 4,
		PhysForest = 8,
		PhysMinorRiver = 16,
		PhysMajorRiver = 32,
		PhysPlow = 64,
		PhysWorker = 128;

}

#endif