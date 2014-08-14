#ifndef VIEW3_H
#define VIEW3_H

#include "box.h"
#include "tree2.h"
#include "view_base.h"

namespace col{

	struct View {

		on

	};
	

	struct Layout {

		static int const PAN_WIDTH = 79;
		static int const LINE = 1;
		static int const TERR_W = TILE_DIM;
		static int const TERR_H = TILE_DIM;


		Box scr, bar, pan, map, city, city_res, city_fields;

		Layout(int const& w, int const& h) {

			scr = Box(0, 0, w, h);

			bar = Box(
				0, 0,
				scr.dim[0], 7
			);

			pan = Box(
				scr.end[0] - PAN_WIDTH, bar.end[1] + LINE,
				PAN_WIDTH, scr.dim[1] - bar.dim[1] - LINE
			);

			map = Box(
				scr.pos[0], bar.end[1] + LINE,
				scr.dim[0] - pan.dim[0] - LINE, scr.dim[1] - bar.dim[1] - LINE
			);

			city = Box(
				map.pos[0], map.pos[1],
				199, 121
			);

			int res_height = LINE + 12 + 1 + 5 + LINE; // line icon sep number line
			city_res = Box(
				scr.pos[0], scr.end[1] - res_height,
				scr.dim[0] - 15, res_height
			);

			city_fields = Box(
				scr.end[0] - TILE_DIM*5, bar.end[1] + LINE,
				TILE_DIM*5, TILE_DIM*5
			);
		}

	};


}


#endif
