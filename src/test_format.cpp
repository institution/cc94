#define CATCH_CONFIG_MAIN
#include "catch.hpp"


#include "format.hpp"


TEST_CASE( "format", "" ) {
	
	REQUIRE(format("aaa") == "aaa");
	REQUIRE(format("x = %||", 1) == "x = 1");
	REQUIRE(format("%|| + %||", 1 ,2) == "1 + 2");

}
