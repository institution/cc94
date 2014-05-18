#ifndef BIOME2_H
#define BIOME2_H

#include "col.hpp"
#include "enum.hpp"



namespace col {

	struct Biome: Enum<> {
		using Enum<>::Enum;

		//string const& get_name() const;
		int get_serial_id() const;

		//static
		//Biome by_name(string const& name);
	};

	inline
	int Biome::get_serial_id() const {
		return get_value();
	}

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

#endif
