#include "../ext/ext.hpp"


namespace front {

	using Tick = int64_t;

	using v2s = ext::v2<int16_t>;
	using v2i = ext::v2<int32_t>;
	using v2f = ext::v2<float_t>;
	
	using b2s = ext::b2<int16_t>;
	using b2i = ext::b2<int32_t>;
	using b2f = ext::b2<float_t>;

	using aabb2f = ext::aabb2<float_t>;


	//using Image = darray::darray2<Color, int16_t>;

	struct Front;
	struct Texture;


	using b2c = b2s;
	using v2c = v2s;

}
