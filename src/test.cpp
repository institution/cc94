#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "env.h"


using namespace col;
using roll::replay;

TEST_CASE( "env::build_road", "[env]" ) {
	
	Env env;
	
	env.set_random_gen(replay({0}));
	
	env.resize({1,1}).set_terr({0,0}, Terr(BIOME_PLAINS));
	
	auto t = env.ref_terr({0,0});
	
	auto u = env.create<Unit>(
		1,
		env.create<UnitType>(1).set_travel(LAND)
	);
	
	env.move_in(t, u);
	
	auto r1 = env.build_road(u);
	
	REQUIRE(r1 == OK);
	REQUIRE(t.has(PHYS_ROAD));
	
	auto r2 = env.build_road(u);

	REQUIRE(r2 != OK); // already exists
    
}

/*
TEST_CASE( "io::write<Env>", "[env]" ) {
	
	Env env;
	
	env.resize({1,1}).set_terr({0,0}, Terr(BIOME_PLAINS));
	
	auto t = env.ref_terr({0,0});
	
	auto u = env.create<Unit>(
		1,
		env.create<UnitType>(1).set_travel(LAND)
	);
	
	env.move_in(t, u);
	
			
		
	std::stringstream buf;
	
	io::write<EnvGame>(buf, src);		
	//io::read<EnvGame>(trg, buf);
	
    
}
 * */