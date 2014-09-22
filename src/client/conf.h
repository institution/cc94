#ifndef CONF340673_H
#define CONF340673_H

#include "col.hpp"

namespace col{

	struct Conf {

		int screen_w;
		int screen_h;
		int tile_dim;
		int global_scale;

		int terr_w;
		int terr_h;


		Path res_path;
		Path font_path;

		Conf() {

			screen_w = 320;
			screen_h = 200;
			tile_dim = 16;
			terr_w = 16;
			terr_h = 16;

			global_scale = 3;

			res_path = "../COLONIZE/";
			font_path = "../font/";

		}

	};

	extern Conf conf;



}


#endif
