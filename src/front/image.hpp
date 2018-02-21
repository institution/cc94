#pragma once
#include "base.hpp"
#include "color.hpp"

namespace front {

	using namespace Color;

	using ImageRGBA8 = ext::darray2<RGBA8, int16_t>;
	using ImageR8 = ext::darray2<R8, int16_t>;
		
	ImageRGBA8 load_png_RGBA8(filesys::Path const& path);
	ImageR8 load_png_R8(filesys::Path const& path);

}
