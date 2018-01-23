#pragma once
#include "base.hpp"
#include "color.hpp"

namespace front {

	using namespace Color;

	using ImageRGBA8 = ext::darray2<RGBA8, int16_t>;
	using ImageA8 = ext::darray2<A8, int16_t>;
		
	ImageRGBA8 load_png_RGBA8(filesys::Path const& path);
	ImageA8 load_png_A8(filesys::Path const& path);

}
