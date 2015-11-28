#ifndef CONF340673_H
#define CONF340673_H

#include "server/col.hpp"

namespace col{

	struct Conf {

		int screen_w;
		int screen_h;
		int tile_dim;
		int scale;

		int terr_w;
		int terr_h;

		Path tile_path;
		Path csv_path;
		Path defaultmap_path;
		
		Conf() {
			scale = 4;
			
			screen_w = 320 * scale;
			screen_h = 200 * scale;
			tile_dim = 16 * scale;


			tile_path = "res/tile64open/";
			csv_path = "res/csv/";
			defaultmap_path = "res/default.mp";
		
		}

	};

	extern Conf conf;



}


#endif
