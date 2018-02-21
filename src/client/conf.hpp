#pragma once

#include "base.hpp"

namespace col{

	struct Conf {

		int16_t screen_w;
		int16_t screen_h;
		int16_t tile_dim;
		int16_t line_ths;
		int16_t scale;

		bool hires;

		Path tile_path;
		Path font_tiny_path;
		Path csv_path;
		Path defaultmap_path;

		void read_rc(Path const& tile_path);		
		void read(Path const& tile_path);

	};

	extern Conf conf;



}


