#define CATCH_CONFIG_MAIN
#include "catch.hpp"


#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


#include "env.h"
#include "serialize.hpp"


using namespace col;
using roll::replay;


/*
 * build road *
 * move *
 * attack *
 * build colony *
 * work at colony field (end turn) *
 * plow fields *
 * work at colony manufacture (end turn) *
 * 
 * construct building by working in lumber mill (col94 style)
 * load/unload cargo into ship
 * travel to europe by sea - exit_map(ship, dest) order
 * sell/buy in europe 
 * working colonist consume food
 * clear forest (add lumber to colony)
 * 
 */


TEST_CASE( "env::get_coords", "" ) {
	
	Env env;
	
	env.resize({5,2});
	// env.set_terr({4,1}, Terr(Biome::Plains));
	
	auto& b = env.terrs(Coords(0,0));
	
	cerr << &env.terrs(Coords(0,0)) - &b << endl;
	cerr << &env.terrs(Coords(1,0)) - &b << endl;
	cerr << &env.terrs(Coords(2,0)) - &b << endl;
	
	cerr << &env.terrs(Coords(0,1)) - &b << endl;
	cerr << &env.terrs(Coords(1,1)) - &b << endl;
	cerr << &env.terrs(Coords(2,1)) - &b << endl;
	
	// mem layout: 0   1   2   3   4   | 5   6   7   8   9
	// mem layout: 0,0 1,0 2,0 3,0 4,0 | 0,1 1,1 2,1 3,1 4,1	
	
	REQUIRE(int(&env.terrs(Coords(0,0)) - &b) == 0);
	REQUIRE(int(&env.terrs(Coords(1,0)) - &b) == 1);
	REQUIRE(int(&env.terrs(Coords(2,0)) - &b) == 2);
	
	REQUIRE(int(&env.terrs(Coords(0,1)) - &b) == 5);
	REQUIRE(int(&env.terrs(Coords(1,1)) - &b) == 6);
	REQUIRE(int(&env.terrs(Coords(2,1)) - &b) == 7);
	
	
	REQUIRE(env.get_coords(*(&b + 0)) == Coords(0,0));
	REQUIRE(env.get_coords(*(&b + 1)) == Coords(1,0));
	REQUIRE(env.get_coords(*(&b + 2)) == Coords(2,0));
	
	REQUIRE(env.get_coords(*(&b + 5)) == Coords(0,1));
	REQUIRE(env.get_coords(*(&b + 6)) == Coords(1,1));
	REQUIRE(env.get_coords(*(&b + 7)) == Coords(2,1));
	
	
	
	REQUIRE(env.get_coords(env.get_terr({4,1})) == Coords(4,1));
	REQUIRE(env.get_coords(env.get_terr({0,0})) == Coords(0,0));
	
	
}


TEST_CASE( "get terr", "" ) {
	
	Env env;
	
	env.resize({1,1});
	env.set_terr({0,0}, Terr(Biome::Plains));
	
	
	auto& t = env.get<Terr>(Coords(0,0));
		
}

TEST_CASE( "colony", "" ) {
	
	Env env;
	
	env.resize({1,1});
	env.set_terr({0,0}, Terr(Biome::Plains));
	
	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND),
		env.create<Player>()
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
			auto& b = env.create<Build>(env.create<BuildType>());
			env.move_in(c, b);
			
			REQUIRE(b.assign() == true);
			u.set_work(b, Item::Coats);

			c.add({Item::Furs, 3});
			
			env.turn();

			REQUIRE(c.get(Item::Coats) == 3);
			REQUIRE(c.get(Item::Furs) == 0);
			REQUIRE(u.time_left == 0);
		}

	}
	
	
	
}


TEST_CASE( "serialize", "" ) {
	
	Env env;
	
	env.resize({2,1});
	env.set_terr({0,0}, Terr(Biome::Plains));
	env.set_terr({1,0}, Terr(Biome::Plains));
	
	auto& u = env.create<Unit>(		
		env.create<UnitType>().set_travel(LAND),
		env.create<Player>()
	);
	
	env.move_in(env.ref_terr({0,0}), u);
	
	
	std::stringstream stream;
	
	{
		boost::archive::text_oarchive oa(stream);
		oa << env;
    }
	
	Env env2;
	env2.uts = env.uts;
	{
		boost::archive::text_iarchive ia(stream);
		ia >> env2;
    }
	
	/*
	REQUIRE(env.get_coords(u) == Coords(0,0));
	
	env.set_random_gen(replay({0}));
	env.order_move(u, Dir::D);	
	
	REQUIRE(env.get_coords(u) == Coords(1,0));
	*/
	
}

TEST_CASE( "env::move_unit", "" ) {
	
	Env env;
	
	env.resize({2,1});
	env.set_terr({0,0}, Terr(Biome::Plains));
	env.set_terr({1,0}, Terr(Biome::Plains));
	
	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND),
		env.create<Player>()
	);
	
	env.move_in(env.ref_terr({0,0}), u);
	
	REQUIRE(env.get_coords(u) == Coords(0,0));
	
	env.set_random_gen(replay({0}));
	env.order_move(u, Dir::D);	
	
	REQUIRE(env.get_coords(u) == Coords(1,0));
	
	
}


		
			

TEST_CASE( "two units", "" ) {
	
	Env env;
	
	env.resize({2,1});
	env.set_terr({0,0}, Terr(Biome::Plains));
	env.set_terr({1,0}, Terr(Biome::Plains));
	
	auto& t1 = env.ref_terr({0,0});
	auto& t2 = env.ref_terr({1,0});
	
	auto& ut = env.create<UnitType>().set_travel(LAND);
	ut.set_attack(2).set_combat(1);
	
	auto& p = env.create<Player>();
	
	auto& u1 = env.create<Unit>(ut, p);
	env.move_in(t1, u1);
	
	auto& u2 = env.create<Unit>(ut, p);
	env.move_in(t2, u2);
	
	SECTION("have diffrent id") {
		REQUIRE(u1.id != u2.id);  // regress
	}
	
	SECTION("can order attack") {
		auto id1 = u1.id;
		auto id2 = u2.id;

		env.set_random_gen(replay({1}));
		env.order_attack(u1, Dir::D);

		REQUIRE(env.exist<Unit>(id1) == true);
		REQUIRE(env.exist<Unit>(id2) == false);
	}
}

TEST_CASE( "improve square", "" ) {
	
	Env env;	
	env.resize({1,1}).set_terr({0,0}, Terr(Biome::Plains));
	
	
	
	auto& t = env.ref_terr({0,0});
	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND),
		env.create<Player>()
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