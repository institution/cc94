#include "prof.hpp"

namespace col {
	std::array<char const*, ProfEnd> const prof_names = {
		"None",
		"Farmer",
		"Fisherman",
		"SugarPlanter",
		"TobaccoPlanter",
		"CottonPlanter",
		"FurTrapper",
		"Lumberjack",
		"OreMiner",
		"SilverMiner",
		"Distiller",
		"Tobaconist",
		"Weaver",
		"FurTrader",
		"Carpenter",
		"Scout",
		"Blacksmith",
		"Gunsmith",
		"Soldier",		
		"Pioneer",
		"Preacher",
		"Statesman"
	};

	char const* get_prof_name(Prof p) {
		return prof_names.at(p);
	}

	Prof get_prof_by_name(char const* name) {
		for (Prof p = ProfNone; p < ProfEnd; ++p) {
			if (strcmp(get_prof_name(p), name) == 0) {
				return p;
			}
		}
		print(std::cerr, "INFO: invalid profession name: %||\n", name);
		return ProfNone;
	}
}
