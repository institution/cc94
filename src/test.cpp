#define CATCH_CONFIG_MAIN
#include "catch.hpp"


#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


#include "env.h"
#include "envgame.h"
#include "serialize.hpp"
#include "action.h"


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


TEST_CASE( "turn sequence", "" ) {

	Env env;

	auto& p1 = env.create<Player>();

	env.start(p1);

	SECTION("1p") {
		auto t = env.turn_no;

		REQUIRE(env.get_current_player().id == p1.id);
		env.ready(p1);
		REQUIRE(env.get_current_player().id == p1.id);
		REQUIRE(env.turn_no == t + 1);


	}

	auto& p2 = env.create<Player>();

	SECTION("2p") {
		auto t = env.turn_no;

		REQUIRE(env.get_current_player().id == p1.id);
		env.ready(p1);
		env.ready(p2);
		REQUIRE(env.get_current_player().id == p1.id);
		REQUIRE(env.turn_no == t+1);

	}

}



TEST_CASE( "colony", "" ) {

	string const CSV_PATH = "../col94/";
	
	Env env;

	env.resize({1,1});
	env.set_terr({0,0}, Terr(Biome::Plains));

	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND),
		env.create<Player>()
	);

	auto& t = env.ref_terr({0,0});
	env.move_in(t, u);

	env.loads<BuildType>(CSV_PATH + "builds.csv");


	SECTION("build") {
		env.set_random_gen(replay({0}));
		env.colonize(u, "aaa");

		REQUIRE(t.colony != nullptr);

		auto& c = t.get_colony();

		SECTION("work field") {

			REQUIRE(t.assign() == true);
			u.set_work(t, ItemFood);

			REQUIRE(u.workplace == &t);
			REQUIRE(u.workitem == ItemFood);

			env.turn();

			REQUIRE(c.get(ItemFood) > 0);
			REQUIRE(u.time_left == 0);
		}

		SECTION("work build") {

			auto& b = c.builds.at(0);  //

			REQUIRE(b.assign() == true);
			u.set_work(b, ItemCoats);

			SECTION("just enough") {
				c.add({ItemFurs, 3});

				env.turn();

				REQUIRE(c.get(ItemCoats) == 3);
				REQUIRE(c.get(ItemFurs) == 0);
				REQUIRE(u.time_left == 0);
			}

			SECTION("not enough") {
				c.add({ItemFurs, 1});

				env.turn();

				REQUIRE(c.get(ItemCoats) == 1);
				REQUIRE(c.get(ItemFurs) == 0);
				REQUIRE(u.time_left == 0);
			}

			SECTION("more than enough") {
				c.add({ItemFurs, 5});

				env.turn();

				REQUIRE(c.get(ItemCoats) == 3);
				REQUIRE(c.get(ItemFurs) == 2);
				REQUIRE(u.time_left == 0);
			}

		}

	}



}



TEST_CASE( "scoring", "" ) {

	EnvGame env;

	env.resize({2,1});
	env.set_terr({0,0}, Terr(Biome::Plains));
	env.set_terr({1,0}, Terr(Biome::Plains));

	auto& p1 = env.create<Player>();
	auto& p2 = env.create<Player>();

	auto& ut = env.create<UnitType>();

	REQUIRE(env.get_result(p1.id) == 0.0f);
	REQUIRE(env.get_result(p2.id) == 0.0f);


	auto& u1 = env.create<Unit>(ut, p1);

	REQUIRE(env.get_result(p1.id) == 1.0f);
	REQUIRE(env.get_result(p2.id) == 0.0f);

	auto& u2 = env.create<Unit>(ut, p2);

	REQUIRE(env.get_result(p1.id) == 0.5f);
	REQUIRE(env.get_result(p2.id) == 0.5f);

}


TEST_CASE( "actions equality", "" ) {

	REQUIRE(OrderMove(1,1,1) == OrderMove(1,1,1));
	REQUIRE(!(OrderMove(1,1,1) == OrderAttack(1,1,1)));
	REQUIRE(!(OrderMove(1,1,1) == OrderMove(1,1,2)));

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

	SECTION("save/load unstarted game") {

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

	}

	SECTION("save/load started game") {

		env.start(u.get_player());

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


TEST_CASE( "compatible", "[env][misc]" ) {	
	REQUIRE(compatible(LAND,SEA) == false);
	REQUIRE(compatible(SEA,LAND) == false);
	REQUIRE(compatible(LAND,LAND) == true);
	REQUIRE(compatible(SEA,SEA) == true);
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

		REQUIRE( env.build_road(u) == true );
		REQUIRE( t.has(Phys::Road)       );

		REQUIRE_THROWS_AS(env.build_road(u), Error);				
	}

	SECTION("plow field") {
		env.set_random_gen(replay({0}));

		REQUIRE( env.plow_field(u) == true );
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