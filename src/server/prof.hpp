#pragma once

#include "col.hpp"

namespace col {
	
	using Prof = uint8_t;
	Prof const
		ProfNone{0},
		
		// school
		ProfFarmer{1},
		ProfFisherman{2},
		ProfSugarPlanter{3},
		ProfTobaccoPlanter{4},
		ProfCottonPlanter{5},
		ProfFurTrapper{6},
		ProfLumberjack{7},
		ProfOreMiner{8},
		ProfSilverMiner{9},
		ProfDistiller{10},
		ProfTobaconist{11},
		ProfWeaver{12},
		ProfFurTrader{13},
		ProfCarpenter{14},
		ProfScout{15},
		
		// college		
		ProfBlacksmith{16},
		ProfGunsmith{17},
		ProfSoldier{18},		
		ProfPioneer{19},

		// university
		ProfPreacher{20},
		ProfStatesman{21},
		
		ProfEnd{22};    


	char const* get_prof_name(Prof p);
	Prof get_prof_by_name(char const* name);
	
}
