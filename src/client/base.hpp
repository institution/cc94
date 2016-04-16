#pragma once

#include "../ext/format.hpp"
#include "../front/front.hpp"

namespace col{
	using filesys::Path;
	
	using front::Image;
	using front::Texture;
	using front::Front;
	using front::PixFont;
	using front::Color;

	using front::v2s;
	using front::v2i;
	using front::v2f;

	using front::b2s;
	using front::b2i;
	using front::b2f;

	using ext::v2;
	using ext::fail;

	using Action = std::function<void()>;
}
