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


		Path res_path;
		Path font_path;

		Conf() {
			scale = 3;
			
			screen_w = 320 * scale;
			screen_h = 200 * scale;
			tile_dim = 16 * scale;
			
			res_path = "../COLONIZE/";
			font_path = "../font/";

		}

	};

	extern Conf conf;



}


#endif
