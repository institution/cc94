#include "../ext/ext.hpp"


namespace front {

	using Tick = int64_t;

	using v2s = ext::v2<int16_t>;
	using v2i = ext::v2<int32_t>;
	using v2f = ext::v2<float_t>;
	
	using b2s = ext::b2<int16_t>;
	using b2i = ext::b2<int32_t>;
	using b2f = ext::b2<float_t>;


	struct Color;

	using Image = darray2::darray2<Color, int16_t>;

	struct Front;
	struct Texture;
	struct PixFont;


}
