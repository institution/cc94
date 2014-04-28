#include "view_base.h"



using std::unordered_map;

namespace col {
	unordered_map<uint8,string> BIOME_NAMES{
		{eval(Biome::Tundra), "tundra"},
		{eval(Biome::Prairie), "prairie"},
		{eval(Biome::Plains), "plains"},
		{eval(Biome::Desert), "desert"},
		{eval(Biome::Savannah), "savannah"},
		{eval(Biome::Grassland), "grassland"},
		{eval(Biome::Marsh), "marsh"},
		{eval(Biome::Swamp), "swamp"},
		{eval(Biome::Arctic), "arctic"},
		{eval(Biome::Ocean), "ocean"},
		{eval(Biome::SeaLane), "sea_lane"},
	};

	Biome get_biome_by_name(string const& name) {
		for (auto& item: BIOME_NAMES) {
			if (item.second == name) {
				return static_cast<Biome>(item.first);
			}
		}
		assert(0);
	}
	
	unordered_map<uint8,string> PHYS_NAMES {
		{eval(Phys::Hill), "hill"},
		{eval(Phys::Mountain), "mountain"},
		{eval(Phys::Road), "road"},
		{eval(Phys::Forest), "forest"},
		{eval(Phys::MinorRiver), "minor_river"},
		{eval(Phys::MajorRiver), "major_river"},
		{eval(Phys::Plow), "plow"}
	};

	Phys get_phys_by_name(string const& name) {
		for (auto& item: PHYS_NAMES) {
			if (item.second == name) {
				return static_cast<Phys>(item.first);
			}
		}
		assert(0);
	}
	
	
}