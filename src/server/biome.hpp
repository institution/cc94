#pragma once

#include "col.hpp"
#include "distinct.hpp"



namespace col {

	namespace detail { struct Biome; }
	using Biome = distinct::Enum<detail::Biome, uint8>;

	constexpr
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
		BiomeArctic{9};


}

