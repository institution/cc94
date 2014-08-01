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
 * construct building by working in lumber mill *
 * board/leave shib by units *
 * working colonist consume food *
 * equip/unequip units
 * map tasks consume tools
 * load/unload cargo into ship 
 * travel to europe by sea - exit_map(ship, dest) order
 * sell/buy in europe 
 * clear forest (add lumber to colony)
 *
 */




/*
TEST_CASE( "tree2", "" ) {
	using NodeType = tree2::Node<int>;
	
	auto root = NodeType(0);
	root.insert_child(1);
	root.insert_child(2);
	root.insert_child(3);

	auto const& const_root = root;
	
	// iteration
	for (auto& node: root) {
		cout << node << endl;
	}	
	
	// const iteration
	for (auto& node: const_root) {
		cout << node << endl;
	}
	
	// find
	REQUIRE((*root.find_child(2)).payload == 2);
	
	// const find
	REQUIRE((*const_root.find_child(2)).payload == 2);
	
	

	REQUIRE(true);
	
}
*/



TEST_CASE( "terr", "" ) {
	Terr t(AltFlat, BiomePlains, PhysForest);
	REQUIRE(t.has(PhysForest));
	
	SECTION("phys: worker does not change forest") {
		t.add(PhysWorker);
		REQUIRE(t.has(PhysWorker));
		REQUIRE(t.has(PhysForest));		
		t.sub(PhysWorker);
		REQUIRE(t.has(PhysForest));
	}
	
	SECTION("yield food mixed forest no-expert == 4") {
		REQUIRE(4 == t.get_yield(ItemFood, 0));
	}
	
	SECTION("yield food sea") {
		Terr t(AltSea, BiomePlains);
		REQUIRE(t.get_yield(ItemFood, 0) == 4);	
	}
	
			
	
}

TEST_CASE( "get terr", "" ) {

	Env env;

	env.resize({1,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));


	auto& t = env.get<Terr>(Coords(0,0));
	REQUIRE(t.biome == BiomePlains);

}



TEST_CASE( "env::move_unit", "" ) {

	Env env;

	env.resize({2,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,0}, Terr(AltFlat, BiomePlains));

	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND).set_speed(1),
		env.create<Player>()
	);

	env.init(u, env.get_terr({0,0}));

	REQUIRE(env.get_coords(u) == Coords(0,0));

	env.set_random_gen(replay({0}));
	env.order_move(u, Dir::D);

	REQUIRE(env.get_coords(u) == Coords(1,0));


}



TEST_CASE( "equip unit", "" ) {

	Env env;
	env.resize({1,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	auto& t = env.get_terr({0,0});
	
	auto& u = env.create<Unit>(
		env.create<UnitType>().set_base(5).set_equip2(ItemHorses, 50),
		env.create<Player>()
	);
	env.init(u, t);
	
	auto& ut = env.create<UnitType>().set_base(5).set_equip1(ItemMuskets, 50).set_equip2(ItemHorses, 50);
	
	auto& c = env.create<Colony>();
	env.init(c, t);
	
	REQUIRE_NOTHROW(c.add(ItemMuskets, 50));	
	REQUIRE_NOTHROW(env.equip(u, ut));	
	REQUIRE(u.get_type() == ut);
	REQUIRE(c.get(ItemMuskets) == 0);
	REQUIRE(c.get(ItemHorses) == 0);
	

}



TEST_CASE( "board ship", "" ) {

	Env env;
	env.resize({3,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,0}, Terr(AltSea, BiomePlains));
	env.set_terr({2,0}, Terr(AltSea, BiomePlains));
		
	auto& p = env.create<Player>();
	
	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND).set_speed(1),
		p
	);
	env.init(u, env.get_terr({0,0}));
	
	auto& s = env.create<Unit>(
		env.create<UnitType>().set_travel(SEA).set_slots(2),
		p
	);
	env.init(s, env.get_terr({1,0}));
		
	REQUIRE(env.get_transport_space(env.get_terr({1,0}), p) == 2);	
	REQUIRE(env.has_transport(env.get_terr({1,0}), u) == true);
	
	env.order_move(u, Dir::D);

	REQUIRE(env.get_coords(u) == Coords(1,0));
	REQUIRE(u.transported == true);
	
	env.order_move(s, Dir::D);
	
	REQUIRE(env.get_coords(s) == Coords(2,0));
	REQUIRE(env.get_coords(u) == Coords(2,0));
	REQUIRE(u.transported == true);
	
	//env.order_board(u, {1,0});
	
	//REQUIRE(env.get_pos(u) == Coords(1,0));

}

TEST_CASE( "turn sequence", "" ) {

	Env env;

	auto& p1 = env.create<Player>();

	env.start();

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



TEST_CASE( "colony workplace/production", "" ) {
	
	Env env;
	BuildType::Id const BuildFurTradersHouse = 33;

	env.resize({1,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	
	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND),
		env.create<Player>()
	);

	auto& t = env.get_terr({0,0});	
	env.init(u, t);

	
	env.loads<BuildType>("../col94/builds.csv");
	REQUIRE(env.get<BuildType>(BuildFurTradersHouse).get_proditem() == ItemCoats);
	
	SECTION("build") {
		env.set_random_gen(replay({0}));
		
		REQUIRE_NOTHROW(env.colonize(u, "aaa"));
		REQUIRE(t.colony != nullptr);

		auto& c = t.get_colony();
		
		SECTION("multiple assign to same workplace") {
			REQUIRE_NOTHROW(env.work_build(0, u));
			REQUIRE_NOTHROW(env.work_build(0, u));
			REQUIRE_NOTHROW(env.work_build(0, u));
			REQUIRE_NOTHROW(env.work_build(0, u));
			REQUIRE_NOTHROW(env.work_build(0, u));
			REQUIRE_NOTHROW(env.work_build(0, u));
			REQUIRE_NOTHROW(env.work_build(0, u));
			REQUIRE_NOTHROW(env.work_build(0, u));			
		}
		
		SECTION("work field") {
			REQUIRE_NOTHROW(c.add(ItemFood, 2));
			REQUIRE_NOTHROW(env.work_field(0, u));
			REQUIRE_NOTHROW(env.get_field(t, 0).set_proditem(ItemFood));
			REQUIRE_NOTHROW(env.turn());
			REQUIRE(c.get(ItemFood) > 0);
			REQUIRE(u.time_left == 0);
		}

		SECTION("work build") {
			
			REQUIRE_NOTHROW(env.put_build(t, 0, BuildFurTradersHouse));
			REQUIRE(env.get_build(t, 0).get_proditem() == ItemCoats);
			REQUIRE_NOTHROW(env.work_build(0, u));

			SECTION("just enough") {
				REQUIRE_NOTHROW(c.add(ItemFood, 2));
				REQUIRE_NOTHROW(c.add(ItemFurs, 3));
				REQUIRE_NOTHROW(env.turn());
				REQUIRE(c.get(ItemCoats) == 3);
				REQUIRE(c.get(ItemFurs) == 0);
				REQUIRE(u.time_left == 0);
			}

			SECTION("not enough") {
				REQUIRE_NOTHROW(c.add(ItemFood, 2));
				REQUIRE_NOTHROW(c.add(ItemFurs, 1));
				REQUIRE_NOTHROW(env.turn());				
				REQUIRE(c.get(ItemCoats) == 1);
				REQUIRE(c.get(ItemFurs) == 0);
				REQUIRE(u.time_left == 0);
			}

			SECTION("more than enough") {
				REQUIRE_NOTHROW(c.add(ItemFood, 2));
				REQUIRE_NOTHROW(c.add(ItemFurs, 5));
				REQUIRE_NOTHROW(env.turn());				
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
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,0}, Terr(AltFlat, BiomePlains));

	auto& p1 = env.create<Player>();
	auto& p2 = env.create<Player>();

	auto& ut = env.create<UnitType>();

	REQUIRE(env.get_result(p1.id) == 0.0f);
	REQUIRE(env.get_result(p2.id) == 0.0f);


	env.create<Unit>(ut, p1);

	REQUIRE(env.get_result(p1.id) == 1.0f);
	REQUIRE(env.get_result(p2.id) == 0.0f);

	env.create<Unit>(ut, p2);

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
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,0}, Terr(AltFlat, BiomePlains));

	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND),
		env.create<Player>()
	);

	env.init(u, env.get_terr({0,0}));

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

		env.start();

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





TEST_CASE( "two units", "" ) {

	Env env;

	env.resize({2,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,0}, Terr(AltFlat, BiomePlains));

	auto& t1 = env.get_terr({0,0});
	auto& t2 = env.get_terr({1,0});

	auto& ut = env.create<UnitType>().set_travel(LAND);
	ut.set_attack(2).set_combat(1);

	auto& p = env.create<Player>();

	auto& u1 = env.create<Unit>(ut, p);
	env.init(u1, t1);

	auto& u2 = env.create<Unit>(ut, p);
	env.init(u2, t2);

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
	env.resize({1,1}).set_terr({0,0}, Terr(AltFlat, BiomePlains));



	auto& t = env.get_terr({0,0});
	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND),
		env.create<Player>()
	);
	env.init(u, t);

	SECTION("build road") {
		env.set_random_gen(replay({0}));

		REQUIRE( env.build_road(u) == true );
		REQUIRE( t.has(PhysRoad)       );

		REQUIRE_THROWS_AS(env.build_road(u), Error);				
	}

	SECTION("plow field") {
		env.set_random_gen(replay({0}));

		REQUIRE( env.plow_field(u) == true );
		REQUIRE( t.has(PhysPlow)       );
	}


}

/*
TEST_CASE( "io::write<Env>", "[env]" ) {

	Env env;

	env.resize({1,1}).set_terr({0,0}, Terr(BIOME_PLAINS));

	auto t = env.get_terr({0,0});

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