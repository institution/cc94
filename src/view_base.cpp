#include "view_base.h"



using std::unordered_map;

namespace col {
	unordered_map<uint8,string> BIOME_NAMES{
		{Biome::Tundra, "tundra"},
		{Biome::Prairie, "prairie"},
		{Biome::Plains, "plains"},
		{Biome::Desert, "desert"},
		{Biome::Savannah, "savannah"},
		{Biome::Grassland, "grassland"},
		{Biome::Marsh, "marsh"},
		{Biome::Swamp, "swamp"},
		{Biome::Arctic, "arctic"},
		{Biome::Ocean, "ocean"},
		{Biome::SeaLane, "sealane"},
	};

	Biome::type get_biome_by_name(string const& name) {
		for (auto& item: BIOME_NAMES) {
			if (item.second == name) {
				return item.first;
			}
		}
		return Biome::None;
	}
	
	unordered_map<Phys::type, string> PHYS_NAMES {
		{Phys::Hill, "hill"},
		{Phys::Mountain, "mountain"},
		{Phys::Road, "road"},
		{Phys::Forest, "forest"},
		{Phys::MinorRiver, "minorriver"},
		{Phys::MajorRiver, "majorriver"},
		{Phys::Plow, "plow"}
	};

	Phys::type get_phys_by_name(string const& name) {
		for (auto& item: PHYS_NAMES) {
			if (item.second == name) {
				return item.first;
			}
		}
		return Phys::None;
	}
	
	
	
}