#pragma once

#include <unordered_map>
#include "base.hpp"
#include "conf.hpp"
#include "layout.hpp"

namespace col {

	using ResKey = uint32_t; // rescat + reskey
	using ResCat = uint16_t; // resource category
	using ResNum = uint16_t; // resource number

	ResCat const
		TERR = 0,
		ICON = 1,
		PHYS = 2,
		ARCH = 3,
		COLONY = 4,
		BUILD = 5,
		WOODTILE = 6,
		DIFFUSE = 7,
		COAST = 8,
		ZZ = 9;
	
	/// Get resource from global cache (load on demand)
	Texture const& res(Front & fr, ResCat cat, ResNum num);
	
	extern PixFont FontTiny;
	
	void preload_renderer(Front & fr);

	
}
