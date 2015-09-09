#ifndef LAYOUT_HPP_776334
#define LAYOUT_HPP_776334

#include "backend/box.hpp"
#include "props.h"
#include "conf.h"

namespace col{


	using backend::Box2;



	struct Layout {

		int pan_width;
		int line;
		int line_sel;   
		int font_tiny;
		int scale;

		Box2 scr, bar, pan, map;
		Box2 city_builds, city_resources, city_fields, city_exit, city_units, city_middle_bg, city_unit_cargo;
		v2i terr_dim;
		
		vector<v2i> pixs, dims;

		inline int S(int x) const {
			return x * conf.scale;
		}
		
		inline v2i S(v2i const& x) const {
			return x * conf.scale;
		}

		Layout(int const& w, int const& h) {
			scale = conf.scale;
			
			pan_width = S(79);     // right panel
			line = S(1);           // line separating gui panels
			line_sel = S(1);       // line used to draw selection boxes
			font_tiny = S(7);      // font tiny height

			terr_dim = v2i(conf.tile_dim, conf.tile_dim);   // map tile

			scr = Box2(0, 0, w, h);    // whole screen

			
			bar = Box2(
				0, 0,
				scr.dim[0], font_tiny
			);   // top menu bar 

			pan = Box2(
				scr.end[0] - pan_width, bar.end[1] + line,
				pan_width, scr.dim[1] - bar.dim[1] - line
			);   // right panel

			map = Box2(
				scr.pos[0], bar.end[1] + line,
				scr.dim[0] - pan.dim[0] - line, scr.dim[1] - bar.dim[1] - line
			);   // map panel

			city_builds = Box2(
				map.pos[0], map.pos[1],
				S(199), S(121)
			);   // area with buildings

			int res_height = line + S(12 + 1 + 5) + line; // line icon sep number line

			

			
			city_resources = Box2(
				scr.pos[0], scr.end[1] - res_height,
				scr.dim[0] - res_height, res_height
			);   // bottom bar with resources
			
			
			

			city_fields = Box2(
				city_builds.end[0] + line, city_builds.pos[1],
				scr.dim[0] - city_builds.dim[0] - line, city_builds.dim[1]
			);   // big area with fields around city

			auto b_width = int(res_height * 1.618);
			city_exit = Box2(
				scr.end[0] - b_width, scr.end[1] - res_height,
				b_width, res_height
			);   // exit button (bottom-right)

			city_units = Box2(
				scr.pos[0], city_builds.end[1] + 1,
				city_resources.dim[0], S(16)
			);   // unit bar

			city_unit_cargo = Box2(
				city_units.pos[0], city_units.end[1],
				city_units.dim[0], S(12)
			);   // unit cargo bar

			
			city_middle_bg = Box2(
				scr.pos[0], city_builds.end[1],
				city_resources.dim[0], city_resources.pos[1] - city_builds.end[1]
			);   // horizontal area beetween (builds and fields) and (resources and exit)



			auto T1 = S(v2i(23,27));
			auto T2 = S(v2i(44,22));
			auto T3 = S(v2i(53,37));
			auto T4 = S(v2i(75,48));
					
			// relative positions
			pixs = vector<v2i>({
				S(v2i(6,6)), S(v2i(56,5)), S(v2i(87,3)), S(v2i(145,7)), S(v2i(173,10)),
				S(v2i(8,33)), S(v2i(37,37)), S(v2i(67,45)), S(v2i(96,45)), S(v2i(128,45)),
				S(v2i(10,68)), S(v2i(15,94)), S(v2i(66,79)), S(v2i(123,47)), S(v2i(123,98))
			});
			
			dims = vector<v2i>({
				T2,T1,T3,T1,T1,
				T1,T1,T1,T1,T2,
				T2,T2,T3,T4,T2			
			});
			


		}

	};



}
#endif
