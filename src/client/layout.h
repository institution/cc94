#ifndef LAYOUT7_HPP
#define LAYOUT7_HPP

#include "box.h"
#include "view_base.h"

namespace col{






	struct Layout {

		static int const PAN_WIDTH = 79;
		static int const LINE = 1;

		Box2 scr, bar, pan, map;
		Box2 city_builds, city_resources, city_fields, city_exit, city_units, city_middle_bg, city_unit_cargo;
		v2i terr_dim;

		Layout(int const& w, int const& h) {

			terr_dim = v2i(conf.tile_dim, conf.tile_dim);

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

			city_builds = Box2(
				map.pos[0], map.pos[1],
				199, 121
			);

			int res_height = LINE + 12 + 1 + 5 + LINE; // line icon sep number line

			city_resources = Box2(
				scr.pos[0], scr.end[1] - res_height,
				scr.dim[0] - res_height, res_height
			);

			city_fields = Box2(
				city_builds.end[0] + LINE, city_builds.pos[1],
				scr.dim[0] - city_builds.dim[0] - LINE, city_builds.dim[1]
			);

			auto b_width = int(res_height * 1.618);
			city_exit = Box2(
				scr.end[0] - b_width, scr.end[1] - res_height,
				b_width, res_height
			);

			city_units = Box2(
				scr.pos[0], city_builds.end[1] + 1,
				city_resources.dim[0], 16
			);

			city_unit_cargo = Box2(
				city_units.pos[0], city_units.end[1],
				city_units.dim[0], 12
			);

			// horizontal area beetween (builds and fields) and (resources and exit)
			city_middle_bg = Box2(
				scr.pos[0], city_builds.end[1],
				city_resources.dim[0], city_resources.pos[1] - city_builds.end[1]
			);

		}

	};



}
#endif
