#pragma once

#include "../front/color.hpp"

namespace col {

	using namespace front::Color;

	auto const ColorSelectedBG = RGBA8(0x3c,0x20,0x18,0xff);

	auto const ColorNone = RGBA8(0,0,0,0);

	auto const ColorFontSelected = RGBA8(0xc7,0xa2,0x20,0xff);
	auto const ColorFont = RGBA8(0x55,0x96,0x34,0xff);
	auto const ColorFontDisabled = RGBA8(85,85,85,255);

	auto const ColorBlack = RGBA8(0,0,0,255);
	auto const ColorGreen = RGBA8(0,255,0,255);
	auto const ColorBlue = RGBA8(0,0,255,255);
	auto const ColorRed = RGBA8(255,0,0,255);
	auto const ColorOrange = RGBA8(255,165,0,255);
	
	auto const ColorDarkGreen = RGBA8(0,127,0,255);
	auto const ColorDarkBlue = RGBA8(0,0,127,255);
	auto const ColorDarkRed = RGBA8(127,0,0,255);
	auto const ColorYellow = RGBA8(255,255,0,255);
	auto const ColorWhite = RGBA8(255,255,255,255);
	auto const ColorGray = RGBA8(128,128,128,128);

	auto const ColorDarkSkyBlue = RGBA8(76,100,172,255);
	auto const ColorSkyBlue = RGBA8(104,136,192,255);
	auto const ColorLightSkyBlue = RGBA8(156,184,220,255);


	auto const ColorDarkBrown = RGBA8(60,32,24,255);
	auto const ColorBrown = RGBA8(121,73,52,255);
	auto const ColorLightBrown = RGBA8(152,128,92,255);
		

	auto const ColorDarkWoodBrown = RGBA8(84,68,44,255);
	auto const ColorWoodBrown = RGBA8(132,112,80,255);
	auto const ColorLightWoodBrown = RGBA8(152,128,92,255);




}
