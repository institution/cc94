#pragma once

#include "col.hpp"
#include "enums.hpp"


namespace col {

	//namespace detail { struct Biome; }
	//using Biome = distinct::Enum<detail::Biome, uint8>;

	
	using Biome = int8_t;
	
	Biome const
		BiomeNone{0},
		BiomeTundra{1}, //boreal forest
		BiomePrairie{2}, // broadleaf forest
		BiomePlains{3}, // mixed_forest
		BiomeDesert{4}, //scrub forest
		BiomeSavannah{5}, //tropical forest
		BiomeGrassland{6}, //confier itemforest
		BiomeMarsh{7}, //wetland forest
		BiomeSwamp{8}, //rain forest
		BiomeArctic{9},
		BiomeEnd{10};


	array<char const*, BiomeEnd> const biome_names = {
		"None",
		"Tundra",
		"Prairie",
		"Plains",
		"Desert",
		"Savannah",
		"Grassland",
		"Marsh",
		"Swamp",
		"Arctic"
	};
	
	inline char const* get_biome_name(Biome x) {
		return biome_names.at(x);
	}
	
	inline Biome get_biome_by_name(char const* name) {
		return _get_by_name<Biome, BiomeEnd>(biome_names, name);
	}

	inline Biome get_biome_by_name(string const& name) {
		return _get_by_name<Biome, BiomeEnd>(biome_names, name);
	}	

}

