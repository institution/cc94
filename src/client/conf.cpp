#include "conf.hpp"
#include "../ext/filesys.hpp"


namespace col {

	using filesys::operator/;

	Conf conf;
	
	void Conf::read(Path const& tile_path) {
		auto img = front::load_png_RGBA8(tile_path / "TERRAIN_SS" / "001.png");
		auto dim = img.get_dim();

		assert(dim[0] % 16 == 0);
		
		this->scale = dim[0] / 16;
		this->tile_path = tile_path;
		//this->font_tiny_path = tile_path / "FONTTINY_FF" / "001.png";
		this->font_tiny_path = tile_path / "ZZ" / "FONTTINY.ftb";

		this->screen_w = 320 * scale;
		this->screen_h = 200 * scale;
		this->tile_dim = 16 * scale;

		this->csv_path = "res/csv/";		
		this->defaultmap_path = "res/default.mp";
		
	}
	
	
}




