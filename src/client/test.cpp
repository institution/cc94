#define CATCH_CONFIG_MAIN
#include "../catch/catch.hpp"

#include "halo.hpp"

using namespace col;

TEST_CASE( "halo", "" ) {
	
	using halo::Pattern;
	
	halo::Patterns ps;
	
	SECTION("empty_patterns") {
		Pattern q;
		REQUIRE( halo::handle_event(ps, q) == false );
	}
	
	SECTION("catch_all_regress") {
		ps.push_back(Pattern());
		Pattern q;		
		REQUIRE( halo::handle_event(ps, q) == true );
	}
	
	
}
