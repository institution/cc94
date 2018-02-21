#include "conf.hpp"
#include "rcparser.hpp"
#include "../ext/filesys.hpp"


namespace col {

	using filesys::operator/;

	Conf conf;

	void Conf::read_rc(Path const& path)
	{
		RCParser p(path / "layout.rc");
		while (p.ok())
		{		
			auto const& sym = p.read_symbol();
			if (sym == "tile-dim") {
				conf.tile_dim = p.read_int16();
			}
			else if (sym == "line-thickness") {
				conf.line_ths = p.read_int16();
			}
			else if (sym == "hires") {
				conf.hires = p.read_int16();
			}
			else {
				p.set_error("unknown command: %||", sym);				
			}					
		}		
	} 
	
	
	void Conf::read(Path const& tile_path)
	{
		// gives tile_dim, line_ths, hires
		read_rc(tile_path);
		
		this->scale = tile_dim / 16;
		this->tile_path = tile_path;
		
		this->screen_w = 320 * scale;
		this->screen_h = 200 * scale;
		
		this->csv_path = "res/csv/";		
		this->defaultmap_path = "res/default.mp";	
	}
	
	
}




