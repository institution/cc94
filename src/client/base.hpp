#pragma once

#include "../ext/ext.hpp"
#include "../ext/format.hpp"
#include "../front/front.hpp"

namespace col{
	using filesys::Path;


	// TODO: dont use base file ?
		
	using front::ImageRGBA8;
	using front::ImageR8;
	using front::Texture;
	using namespace front::Color;

	using front::Front;

	using front::v2s;
	using front::v2i;
	using front::v2f;

	using front::b2s;
	using front::b2i;
	using front::b2f;

	using front::aabb2f;


	using ext::v2;
	using ext::fail;
	using ext::vdiv;


	using Action = std::function<void()>;
	
	using Coord = int16_t;
	using v2c = ext::v2<Coord>;
	
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
	
	// frequent short literals lol :D
	int16_t const
		_0 = 0,
		_1 = 1,
		_2 = 2,
		_3 = 3,
		_4 = 4;


}
