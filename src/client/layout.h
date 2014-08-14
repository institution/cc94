#ifndef LAYOUT7_HPP
#define LAYOUT7_HPP

#include "box.h"
#include "view_base.h"

namespace col{



	struct Layout {

		static int const PAN_WIDTH = 79;
		static int const LINE = 1;
		static int const TERR_W = TILE_DIM;
		static int const TERR_H = TILE_DIM;

		Box2 scr, bar, pan, map, city, city_res, city_fields, city_exit, city_units;
		v2i terr_dim;

		Layout(int const& w, int const& h) {

			terr_dim = v2i(TILE_DIM, TILE_DIM);

			scr = Box2(0, 0, w, h);

			bar = Box2(
				0, 0,
				scr.dim[0], 7
			);

			pan = Box2(
				scr.end[0] - PAN_WIDTH, bar.end[1] + LINE,
				PAN_WIDTH, scr.dim[1] - bar.dim[1] - LINE
			);

			map = Box2(
				scr.pos[0], bar.end[1] + LINE,
				scr.dim[0] - pan.dim[0] - LINE, scr.dim[1] - bar.dim[1] - LINE
			);

			city = Box2(
				map.pos[0], map.pos[1],
				199, 121
			);

			int res_height = LINE + 12 + 1 + 5 + LINE; // line icon sep number line

			city_res = Box2(
				scr.pos[0], scr.end[1] - res_height,
				scr.dim[0] - res_height, res_height
			);

			city_fields = Box2(
				city.end[0] + LINE, city.pos[1],
				scr.dim[0] - city.dim[0] - LINE, city.dim[1]
			);

			auto b_width = int(res_height * 1.618);
			city_exit = Box2(
				scr.end[0] - b_width, scr.end[1] - res_height,
				b_width, res_height
			);

			city_units = Box2(
				map.pos[0], city.end[1] + 1,
				city.dim[0], 16
			);

		}

	};



}
#endif
