#include "layout.hpp"
#include "rcparser.hpp"


namespace col{

	Layout ly;

	void read_layout(RCParser & p, Layout & ly)
	{
		while (p.ok())
		{		
			auto const& sym = p.read_symbol();
			if (sym == "tile-dim") {
				ly.tile_dim = p.read_int16();	
			}
			else if (sym == "line-thickness") {
				ly.line_th = p.read_int16();
			}
			else if (sym == "hires") {
				ly.hires = p.read_int16();
			}
			else if (sym == "reflow") {
				ly.recalc();
			}
			else {
				p.unread();
				ly.recalc();
				break;				
			}					
		}
		p.end();		
	} 

	void Layout::recalc()
	{
		// scr   is should be (320 200)*scale
		// map   is 15x12 tiles
		// bar   is rest (should be more than font height)
		// pan   is rest ()

		this->font_height = 0;

		this->scale = this->tile_dim / 16;		
		this->scr.dim = v2s(320 * scale, 200 * scale);	
		
		v2s sep = v2s(0,0);

		terr_dim = v2s(tile_dim, tile_dim);   // map tile
		
		map.dim = v2s(15,12) * tile_dim;
		bar.dim = v2s(scr.dim[0], scr.dim[1] - map.dim[1] - line_th);
		pan.dim = v2s(scr.dim[0] - map.dim[0] - line_th, map.dim[1]);

		scr.pos = v2s(0,0);
		bar.pos = scr.pos;
		map.pos = v2s(0, scr.pos[1] + bar.dim[1] + line_th);
		pan.pos = v2s(scr.pos[0] + map.dim[0] + line_th, scr.pos[1] + bar.dim[1] + line_th);

		if (bar.dim[1] < font_height)
		{
			print("WARNING: top.dim[1] < font_height");
		}

		
		city_builds = b2s(
			map.pos[0], map.pos[1],
			S(199), S(121)
		);   // area with buildings

		int res_height = line + S(12 + 1 + 5) + line; // line icon sep number line

		city_fields = b2s(
			city_builds.end()[0] + line, city_builds.pos[1],
			scr.dim[0] - city_builds.dim[0] - line, city_builds.dim[1]
		);   // big area with fields around city

		
		city_units = b2s(
			scr.pos[0], city_builds.end()[1] + line,
			S(16) * 20, S(16)
		);   // unit bar

		city_unit_cargo = b2s(
			city_units.pos[0], city_units.end()[1],
			city_units.dim[0], S(12)
		);   // unit cargo bar
		
		city_resources = b2s(
			scr.pos[0], scr.end()[1] - res_height,
			S(16) * 20, res_height
		);   // bottom bar with resources

		auto supply_nums_height = font_height * 3;
		city_supply_nums = b2s(
			city_resources.pos[0], city_resources.pos[1] - supply_nums_height,
			city_resources.dim[0], supply_nums_height
		);   // resources ballance table
		
		auto b_width = int(res_height * 1.618);
		city_exit = b2s(
			scr.end()[0] - b_width, scr.end()[1] - res_height,
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
