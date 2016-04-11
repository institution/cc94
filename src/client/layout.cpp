#include "layout.hpp"

namespace col{

	Layout ly;

	void Layout::recalc(int16_t w, int16_t h) {
		scale = conf.scale;
		
		pan_width = S(79);     // right panel
		line = S(1);           // line separating gui panels
		line_sel = S(1);       // line used to draw selection boxes
		font_tiny = S(7);      // font tiny height

		terr_dim = v2s(conf.tile_dim, conf.tile_dim);   // map tile

		scr = Cell(0, 0, w, h);    // whole screen

		
		bar = Cell(
			0, 0,
			scr.dim[0], font_tiny
		);   // top menu bar 

		pan = Cell(
			scr.end[0] - pan_width, bar.end[1] + line,
			pan_width, scr.dim[1] - bar.dim[1] - line
		);   // right panel

		map = Cell(
			scr.pos[0], bar.end[1] + line,
			scr.dim[0] - pan.dim[0] - line, scr.dim[1] - bar.dim[1] - line
		);   // map panel

		city_builds = Cell(
			map.pos[0], map.pos[1],
			S(199), S(121)
		);   // area with buildings

		int res_height = line + S(12 + 1 + 5) + line; // line icon sep number line

		

		
		
		
		

		city_fields = Cell(
			city_builds.end[0] + line, city_builds.pos[1],
			scr.dim[0] - city_builds.dim[0] - line, city_builds.dim[1]
		);   // big area with fields around city

		
		city_units = Cell(
			scr.pos[0], city_builds.end[1] + line,
			S(16) * 20, S(16)
		);   // unit bar

		city_unit_cargo = Cell(
			city_units.pos[0], city_units.end[1],
			city_units.dim[0], S(12)
		);   // unit cargo bar
		
		city_resources = Cell(
			scr.pos[0], scr.end[1] - res_height,
			S(16) * 20, res_height
		);   // bottom bar with resources

		auto supply_nums_height = font_tiny * 3;
		city_supply_nums = Cell(
			city_resources.pos[0], city_resources.pos[1] - supply_nums_height,
			city_resources.dim[0], supply_nums_height
		);   // resources ballance table
		
		auto b_width = int(res_height * 1.618);
		city_exit = Cell(
			scr.end[0] - b_width, scr.end[1] - res_height,
			b_width, res_height
		);   // exit button (bottom-right)


		auto T1 = S(v2s(23,27));
		auto T2 = S(v2s(44,22));
		auto T3 = S(v2s(53,37));
		auto T4 = S(v2s(75,48));
				
		// relative positions
		pixs = vector<v2s>({
			S(v2s(6,6)), S(v2s(56,5)), S(v2s(87,3)), S(v2s(145,7)), S(v2s(173,10)),
			S(v2s(8,33)), S(v2s(37,37)), S(v2s(67,45)), S(v2s(96,45)), S(v2s(128,45)),
			S(v2s(10,68)), S(v2s(15,94)), S(v2s(66,79)), S(v2s(123,47)), S(v2s(123,98))
		});
		
		dims = vector<v2s>({
			T2,T1,T3,T1,T1,
			T1,T1,T1,T1,T2,
			T2,T2,T3,T4,T2			
		});
		


	}

}
