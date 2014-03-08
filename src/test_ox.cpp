#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "ox.h"


using namespace ox;


TEST_CASE( "OX::check_pos", "[OX]" ) {
	
	OX ox;

	/*
	'X' moved to 2,2
	O..
	XO.   player 'O'
	..X
	game should not end
	*/
	
	ox.load("O..XO...X", 'O');
	REQUIRE( ox.check_pos(2,2) == false );
    
	/*
	'O' moved to 1,2
	.OX
	XO.   player: 'X'
	OOX
	-> game should end
	*/
	
	ox.load(".OXXO.OOX", 'X');
	REQUIRE( ox.check_pos(1,2) == true );
    
	/*
	'O' moved to 1,1
	OXX
	XOO   player: 'X'
	OXO
	-> game should end
	*/
	
	ox.load("OXXXOOOXO", 'X');
	REQUIRE( ox.check_pos(1,1) == true );
    
}