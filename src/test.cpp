#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "env.h"


using namespace col;
using roll::replay;


/*
 * build road *
 * move *
 * attack *
 * build colony *
 * work at colony field (end turn) *
 * plow fields *
 * work at colony manufacture (end turn)
 * construct building by working in lumber mill (col94 style)
 * load/unload cargo into ship
 * travel to europe by sea - exit_map(ship, dest) order
 * sell/buy in europe 
 * working colonist consume food
 * clear forest (add lumber to colony)
 * 
 */



TEST_CASE( "colony", "" ) {
	
	Env env;
	
	env.resize({1,1});
	env.set_terr({0,0}, Terr(Biome::Plains));
	
	auto& u = env.create<Unit>(
		1,
		env.create<UnitType>(1).set_travel(LAND)
	);
	
	auto& t = env.ref_terr({0,0});
	
	env.move_in(t, u);

	
	SECTION("build") {
		env.set_random_gen(replay({0}));
		env.colonize(u, "aaa");

		REQUIRE(t.colony != nullptr);
		
		auto& c = t.get_colony();
	
		SECTION("work field") {
		
			REQUIRE(t.assign() == true);
			u.set_work(t, Item::Food);

			REQUIRE(u.workplace == &t);
			REQUIRE(u.workitem == Item::Food);
			
			env.turn();

			REQUIRE(c.get(Item::Food) > 0);
			REQUIRE(u.time_left == 0);
		}
		
		SECTION("work build") {
		
			// TODO: new colony gratis buildings
			env.create<Build>(5, )
			
			auto& b = c.builds.at(0);
			
			REQUIRE(b.assign() == true);
			u.set_work(b, Item::Coats);

			env.turn();

			REQUIRE(c.get(Item::Coats) > 0);
			REQUIRE(u.time_left == 0);
		}

	}
	
	
	
}

TEST_CASE( "env::move_unit", "" ) {
	
	Env env;
	
	env.resize({2,1});
	env.set_terr({0,0}, Terr(Biome::Plains));
	env.set_terr({1,0}, Terr(Biome::Plains));
	
	auto& u = env.create<Unit>(
		1,
		env.create<UnitType>(1).set_travel(LAND)
	);
	
	env.move_in(env.ref_terr({0,0}), u);
	
	REQUIRE(env.get_coords(u) == Coords(0,0));
	
	env.set_random_gen(replay({0}));
	env.order_move(u, Dir::D);	
	
	REQUIRE(env.get_coords(u) == Coords(1,0));
	
	
}


TEST_CASE( "env::get_coords", "" ) {
	
	Env env;
	
	env.resize({5,3});
	env.set_terr({4,1}, Terr(Biome::Plains));
		
	REQUIRE(env.get_coords(env.ref_terr({4,1})) == Coords(4,1));
	
}

		
			

TEST_CASE( "env::order_attack", "" ) {
	
	Env env;
	
	env.resize({2,1});
	env.set_terr({0,0}, Terr(Biome::Plains));
	env.set_terr({1,0}, Terr(Biome::Plains));
	
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

TEST_CASE( "improve square", "" ) {
	
	Env env;	
	env.resize({1,1}).set_terr({0,0}, Terr(Biome::Plains));
	
	
	
	auto& t = env.ref_terr({0,0});
	auto& u = env.create<Unit>(
		1,
		env.create<UnitType>(1).set_travel(LAND)
	);
	env.move_in(t, u);
	
	SECTION("build road") {
		env.set_random_gen(replay({0}));
		
		REQUIRE( env.build_road(u) == OK );
		REQUIRE( t.has(Phys::Road)       );
		
		REQUIRE( env.build_road(u) != OK);		
	}
	
	SECTION("plow field") {
		env.set_random_gen(replay({0}));
		
		REQUIRE( env.plow_field(u) == OK );
		REQUIRE( t.has(Phys::Plow)       );
	}
	
    
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