#include "ftb.hpp"

namespace ftb 
{


	void read(std::ifstream & f, int8_t & r) { read_le(f, r); }
	void write(std::ofstream & f, int8_t const& r) { write_le(f, r); }

	void read(std::ifstream & f, int16_t & r) { read_le(f, r); }
	void write(std::ofstream & f, int16_t const& r) { write_le(f, r); }

	void read(std::ifstream & f, uint8_t & r) { read_le(f, r); }
	void write(std::ofstream & f, uint8_t const& r) { write_le(f, r); }

	void read(std::ifstream & f, uint32_t & r) { read_le(f, r); }
	void write(std::ofstream & f, uint32_t const& r) { write_le(f, r); }


	void write(std::ofstream & f, FontHead const& g) 
	{ 
		write(f, g.height); 
		write(f, g.ascender);
		write(f, g.descender);
	}

	void read(std::ifstream & f, FontHead & g) 
	{ 
		read(f, g.height); 
		read(f, g.ascender);
		read(f, g.descender);
	}

	void write(std::ofstream & f, GlyphHead const& g)
	{
		write(f, g.code);
		write(f, g.bearing_x);
		write(f, g.bearing_y);
		write(f, g.advance);
		write(f, g.width);
		write(f, g.height);
		write(f, g.pos_x);
		write(f, g.pos_y);
	}

	void read(std::ifstream & f, GlyphHead & g)
	{
		read(f, g.code);
		read(f, g.bearing_x);
		read(f, g.bearing_y);
		read(f, g.advance);
		read(f, g.width);
		read(f, g.height);
		read(f, g.pos_x);
		read(f, g.pos_y);
	}



	void write(std::ofstream & f, Mask const& img)
	{
		write(f, img.dim[0]);
		write(f, img.dim[1]);
		
		for (size_t i = 0; i < img.size(); ++i)
		{
			write(f, img.at(i));
		}	
		
	}


	void read(std::ifstream & f, Mask & img)
	{
		decltype(img.dim) dim;
		read(f, dim[0]);
		read(f, dim[1]);
		img.resize(dim);
		
		for (size_t i = 0; i < img.size(); ++i)
		{
			read(f, img.at(i));
		}	
	}
	
	

	void read(std::ifstream & f, Font & font)
	{
		int verbose = 1;
		
		if (verbose) { print("ftb_debug_mark: %||\n", f.tellg()); }
		
		ftb::read(f, font.head);
		
		if (verbose) { print("ftb_debug_mark: %||\n", f.tellg()); }
		
		uint32_t nglyph;
		ftb::read(f, nglyph);
		
		if (verbose) { print("ftb_debug: nglyph: %||\n", nglyph); }
		
		font.glyphs.resize(256);
		for (uint32_t i = 0; i < nglyph; ++i)
		{
			ftb::GlyphHead gh;
			ftb::read(f, gh);		
			font.glyphs.at(gh.code) = gh;
		}
		
		if (verbose) { print("ftb_debug_mark: %||\n", f.tellg()); }
		
		ftb::read(f, font.mask);
		
		if (verbose) { print("ftb_debug_mark: %||\n", f.tellg()); }
		if (verbose) { print("END ftb_debug\n"); }
						
		
	}

}
