#include "image.hpp"

#include "../lodepng/lodepng.h"

namespace front {
		
	void print_info_png(filesys::Path const& path, lodepng::State const& state)
	{
		LodePNGInfo const& info = state.info_png;
		LodePNGColorMode const& color = state.info_png.color;

		print("Load png: %||\n", path);
		std::cout << "Compression method: " << info.compression_method << std::endl;
		std::cout << "Filter method: " << info.filter_method << std::endl;
		std::cout << "Interlace method: " << info.interlace_method << std::endl;
		std::cout << "Color type: " << color.colortype << std::endl;
		std::cout << "Bit depth: " << color.bitdepth << std::endl;
		std::cout << "Bits per pixel: " << lodepng_get_bpp(&color) << std::endl;
		std::cout << "Channels per pixel: " << lodepng_get_channels(&color) << std::endl;
		std::cout << "Is greyscale type: " << lodepng_is_greyscale_type(&color) << std::endl;
		std::cout << "Can have alpha: " << lodepng_can_have_alpha(&color) << std::endl;
		std::cout << "Palette size: " << color.palettesize << std::endl;
		std::cout << "Has color key: " << color.key_defined << std::endl;
		if (color.key_defined)
		{
			std::cout << "Color key r: " << color.key_r << std::endl;
			std::cout << "Color key g: " << color.key_g << std::endl;
			std::cout << "Color key b: " << color.key_b << std::endl;
		}
		print("\n");
	}


	ImageRGBA8 load_png_RGBA8(filesys::Path const& path)
	{		
		// load
		std::vector<unsigned char> buffer;
		std::vector<unsigned char> image;		
		unsigned width1, height1;
		lodepng::load_file(buffer, path);
		lodepng::State state;
		unsigned error = lodepng::decode(image, width1, height1, state, buffer);
		if (error)
		{
			ext::fail("ERROR: lodepng: %||: %||\n", lodepng_error_text(error), path);
		}
		
		// check
		if (width1 > 16000) ext::fail("image too big");
		if (height1 > 16000) ext::fail("image too big");

		// move/convert
		int16_t width = width1;
		int16_t height = height1;
		uint8_t * p = &image[0];
		
		ImageRGBA8 r({width, height});
		for (int16_t j=0; j<height; ++j) {
			for (int16_t i=0; i<width; ++i) {
				RGBA8 c;
				c.r = *p;
				++p;
				c.g = *p;
				++p;
				c.b = *p;
				++p;
				c.a = *p;
				++p;

				r({i,j}) = c;
			}
		}		
		return r;
	}
	
	ImageR8 load_png_R8(filesys::Path const& path)
	{		
		// load
		std::vector<unsigned char> buffer;
		std::vector<unsigned char> image;		
		unsigned width1, height1;
		lodepng::load_file(buffer, path);
		lodepng::State state;
		unsigned error = lodepng::decode(image, width1, height1, state, buffer);
		if (error)
		{
			ext::fail("ERROR: lodepng: %||: %||\n", lodepng_error_text(error), path);
		}
		
		// check
		if (width1 > 16000) ext::fail("image too big");
		if (height1 > 16000) ext::fail("image too big");
	
		print_info_png(path, state);
		
		// move/convert
		int16_t width = width1;
		int16_t height = height1;
		uint8_t * p = &image[0];
		
		ImageR8 r({width, height});
		for (int16_t j=0; j<height; ++j) {
			for (int16_t i=0; i<width; ++i) {
				R8 c;
				c.r = *p;
				++p;

				r({i,j}) = c;
			}
		}		
		return r;
	}

}
