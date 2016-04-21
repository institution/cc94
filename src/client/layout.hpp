#pragma once

#include "props.hpp"
#include "conf.hpp"
#include "base.hpp"

namespace col{

	
	using Cell = b2s;

	struct Layout {

		int16_t pan_width;
		int16_t line;
		int16_t line_sel;   
		int16_t font_tiny;
		int16_t scale;
		v2s sep;

		Cell scr, bar, pan, map;
		Cell city_builds, city_fields, city_units, city_unit_cargo, city_supply_nums, city_resources, city_exit;
		v2s terr_dim;
		
		vector<v2s> pixs, dims;

		inline int S(int x) const {
			return x * conf.scale;
		}
		
		inline v2s S(v2s x) const {
			return x * conf.scale;
		}

		void recalc(int16_t w, int16_t h);
			
		

	};


	
	extern Layout ly;



}
