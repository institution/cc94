#ifndef VIEW_BASE44_H
#define VIEW_BASE44_H

#include "base.h"

#include <unordered_map>


namespace col {
	extern std::unordered_map<uint8,string> BIOME_NAMES;
	Biome::type get_biome_by_name(string const& name);
	
	extern std::unordered_map<uint8,string> PHYS_NAMES;
	Phys::type get_phys_by_name(string const& name);
	
}

#endif
