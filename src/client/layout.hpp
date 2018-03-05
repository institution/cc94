#pragma once
#include "props.hpp"
#include "base.hpp"
#include "rcparser.hpp"
#include "res.hpp"

namespace col{

	struct Layout {

		int16_t tile_dim;
		int16_t line_th; // line thickness
		int16_t hires{0};

		int16_t font_height; // font height
		int16_t scale;
		
		v2s sep;
		v2s terr_dim;
		int16_t line;

		b2s scr, bar, pan, map;
		b2s city_builds, city_fields, city_units, city_unit_cargo, city_supply_nums, city_resources, city_exit;
		
		vector<v2s> pixs, dims;

		inline int16_t S(int x) const {
			return x * scale;
		}
		
		inline v2s S(v2s x) const {
			return x * scale;
		}

		void recalc();
	
	};

	void read_layout(RCParser & p, Layout & ly);

	
	extern Layout ly;



}
