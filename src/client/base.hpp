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
	
	
	using v2c = front::v2s;
	using Coord = int16_t;
	
	// custom hash can be a standalone function object:
	template <class T> struct MyHash;
	template<> struct MyHash<v2c>
	{
		std::size_t operator()(v2c const& a) const 
		{
			std::size_t h1 = std::hash<Coord>()(a[0]);
			std::size_t h2 = std::hash<Coord>()(a[1]);
			return h1 ^ (h2 << 1); // or use boost::hash_combine
		}
	};
	 
	
}
