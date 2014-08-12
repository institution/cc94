#ifndef PHYS_H
#define PHYS_H

#include "enum.hpp"

namespace col {

	struct Phys: Enum<> {
		using Enum<>::Enum;

		using Id = int;
	};


	constexpr
	Phys const
		PhysNone = 0,
		// unused = 1,
		PhysSeaLane = 2,   // in original probably was alternative with road or smth?; and 2 bits were reserved for height
		PhysRoad = 4,
		PhysForest = 8,
		PhysMinorRiver = 16,
		PhysMajorRiver = 32,
		PhysPlow = 64,
		PhysWorker = 128;

	Phys const
		PhysRiver = PhysMinorRiver | PhysMajorRiver;

	Phys const
		PhysAlterable = PhysRoad|PhysForest|PhysPlow;
}

#endif