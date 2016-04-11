#pragma once

#include "props.hpp"
#include "conf.hpp"
#include "base.hpp"

namespace col{

	

	template <class T>
	struct CellT {

		v2<T> pos, dim, end;

		CellT() = default;

		CellT(v2<T> pos, v2<T> dim):
			pos(pos), dim(dim)
		{
			end = pos + dim;
		}

		CellT(T x, T y, T w, T h):
			pos(x,y), dim(w,h)
		{
			end = pos + dim;
		}

	};

	using Cell = CellT<int16_t>;

	struct Layout {

		int16_t pan_width;
		int16_t line;
		int16_t line_sel;   
		int16_t font_tiny;
		int16_t scale;

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
