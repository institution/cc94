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
		Path font_tiny_path;
		Path csv_path;
		Path defaultmap_path;
		
		Conf();

	};

	extern Conf conf;



}


#endif
