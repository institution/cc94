#include "conf.h"


namespace col {
	
	Conf conf;

	Conf::Conf() {
		scale = 4;

		screen_w = 320 * scale;
		screen_h = 200 * scale;
		tile_dim = 16 * scale;


		tile_path = "res/tile64/";
		//font_tiny_path = "res/Liberation Sans-Regular-16.png";
		font_tiny_path = "res/tile64/FONTTINY_FF/001.png";
		
		csv_path = "res/csv/";
		defaultmap_path = "res/default.mp";



	}

}
