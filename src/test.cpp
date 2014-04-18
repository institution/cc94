#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "env.h"


using namespace col;
using roll::replay;


/*
 * build road *
 * move *
 * attack *
 * build colony 
 * work at colony manufacture (end turn)
 * work at colony field (end turn)
 *  
 * 
 */



TEST_CASE( "env::colonize", "env" ) {
	
	Env env;
	
	env.resize({1,1});
	env.set_terr({0,0}, Terr(BIOME_PLAINS));
	
	auto u = env.create<Unit>(
		1,
		env.create<UnitType>(1).set_travel(LAND)
	);
	
	env.move_in(env.ref_terr({0,0}), u);
	
	env.set_random_gen(replay({0}));
	env.colonize(u, "Alamakota");	
	
	REQUIRE(env.ref_terr({0,0}).colony != nullptr);
	
}

TEST_CASE( "env::move_unit", "env" ) {
	
	Env env;
	
	env.resize({2,1});
	env.set_terr({0,0}, Terr(BIOME_PLAINS));
	env.set_terr({1,0}, Terr(BIOME_PLAINS));
	
	auto u = env.create<Unit>(
		1,
		env.create<UnitType>(1).set_travel(LAND)
	);
	
	env.move_in(env.ref_terr({0,0}), u);
	
	REQUIRE(env.get_coords(u) == Coords(0,0));
	
	env.set_random_gen(replay({0}));
	env.order_move(u, Dir::D);	
	
	REQUIRE(env.get_coords(u) == Coords(1,0));
	
	
}


TEST_CASE( "env::get_coords", "env" ) {
	
	Env env;
	
	env.resize({5,3});
	env.set_terr({4,1}, Terr(BIOME_PLAINS));
		
	REQUIRE(env.get_coords(env.ref_terr({4,1})) == Coords(4,1));
	
}

		
			

TEST_CASE( "env::order_attack", "env" ) {
	
	Env env;
	
	env.resize({2,1});
	env.set_terr({0,0}, Terr(BIOME_PLAINS));
	env.set_terr({1,0}, Terr(BIOME_PLAINS));
	
	auto& t1 = env.ref_terr({0,0});
	auto& t2 = env.ref_terr({1,0});
	
	auto& ut = env.create<UnitType>(1).set_travel(LAND);
	ut.set_attack(2).set_combat(1);
	
	auto& u1 = env.create<Unit>(1, ut);
	env.move_in(t1, u1);
	
	auto& u2 = env.create<Unit>(2, ut);
	env.move_in(t2, u2);
	
	env.set_random_gen(replay({1}));
	env.order_attack(u1, Dir::D);
	
	REQUIRE(env.exist<Unit>(1) == true);
	REQUIRE(env.exist<Unit>(2) == false);
	
}

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