#define CATCH_CONFIG_MAIN
#include "catch.hpp"


#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


#include "env.h"
#include "build.h"
#include "serialize.h"


using namespace col;
using roll::replay;

// wc -l client/* server/* error.h backend/*
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
 * equip/unequip units *
 * map tasks consume tools *
 * ordered colony production *
 *     MAKEFILE: source organization - separate modules *
 *     REGRESS: zero tool building builds on first turn *
 *     VISUAL: unselect unit after assigment to field/build  *
 * colony screen: background on fields *
 *     uniform EVENT handling *
 * portable COLONIZE data import *
 v 0.1.0 *
 
 * colony screen transport *
 * load/unload cargo into transport * 
 * orders -- keep trying *
 * clear forest O / plow P *
 v 0.1.5 *
 
 * emscripten compatible * 
 * v0.2.0 *
 
 * open graphics generator, terrain/textures * 
 * unified construct building/unit framework * 
 i 14722 LOC *
 * equip pioneers, soldiers gui improv *
 * select field production gui improv * 
 * show colony resources ballance * 
 
 * AA font 64 base *
 * load/unload cargo into transport, gui rework
 *  arbitrary number of items
 *  drop cargo on transport icon
 
 * idle factory button 
   
 * simple AI
 
 * AA font - use font_height in layout
 
 v 0.2.4
  
 * unified construct building/unit framework, equip tools
 
 * land combat 
 * artillery fire during enemy turn?
 * temporary fortification
 * simple AI - combat
 * v0.2.8
 
 * fog of war 
 * map size
 * food as default field prod ?
 * v0.3.0
 
 * repair unit
 * specialists framework
 * teach unit (school)
 * specialist by practice
 * v0.4.0
 
 * menu gui, orders
 * trade framework
 * sell/buy in europe  
 * trade with forein powers/indians
 * travel to europe by sea - exit_map(ship, dest) order - trade
 * europe screen
 * simple AI, trade
 * v0.5.0
 
 * transport framework (routes) 
 * simple AI, sea combat
 * simple AI, blockade
 * sea combat/blockade framework
 * v0.6.0
   
 * natives  
 v 0.7.0
 
 * loyality     
 v 0.7.3
 
 * king
 * taxes
 * immigration?
 v 0.8.6
 
 * scoring
 * war of independence
 v 0.8.9
 
 * autosave
 * reports 
 * founding fathers
 * ...
 v 0.9.0 BETA
 
 v 1.0.0 
  
 * keyboard gui
 * colony abandonment 
 * consumable items in colonies
 * silver -> coins conversion?
 * map edit gui (modding)
 * by name from csv (modding)
 * animal husbandry
 * allow land use outside of colony range (hunting?, no-colony buildings & improvments)
 * play as natives
 * play after independence
 * game goals/scenarios for map editing
 
 
 
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


TEST_CASE( "serialize_base", "" ) {
	SECTION("serialize_int16") {

		std::stringstream s;

		int16_t x = 12;
		write(s, x);
		int16_t y;
		read(s, y);
		
		REQUIRE(x == y);

	}	
}


TEST_CASE( "sys", "" ) {
	SECTION("env_clean_units") {
		Env env;		
		env.resize({1,1});
		auto& u = env.create<Unit>(
			env.create<UnitType>(),
			env.create<Nation>()
		);
		REQUIRE_NOTHROW(env.init(env.get_terr({0,0}), u));
		REQUIRE_NOTHROW(env.clear_units());
		REQUIRE(env.units.size() == 0);
		REQUIRE(env.get_terr({0,0}).units.size() == 0);
	}	
}


TEST_CASE( "env", "" ) {
	Env env;
	
	// resize
	REQUIRE_NOTHROW(env.resize({1,1}));
	
	auto& t = env.get_terr({0,0});
	REQUIRE(t.get_biome() == BiomeNone);
	
	
	// fill
	REQUIRE_NOTHROW(env.fill(Terr{AltSea, BiomeTundra}));
	REQUIRE(env.get_terr({0,0}).get_alt() == AltSea);
	REQUIRE(env.get_terr({0,0}).get_biome() == BiomeTundra);
	
	// get<Terr>
	REQUIRE_NOTHROW(env.get<Terr>({0,0}));
		
	
}


TEST_CASE( "inter", "" ) {
	
	REQUIRE(inter::type_name<inter::build_colony>::get() == "build_colony");
	
	//Env env;
	
	//inter::Any a;
	//a = inter::noa(1);
	//REQUIRE_NOTHROW(env.apply_inter(a, 0));
	
	
}



TEST_CASE( "terr", "" ) {
	Terr t(AltFlat, BiomePlains, PhysForest);
	REQUIRE(t.has_phys(PhysForest));
	
	SECTION("phys: worker does not change forest") {
		t.add_phys(PhysWorker);
		REQUIRE(t.has_phys(PhysWorker));
		REQUIRE(t.has_phys(PhysForest));		
		t.sub_phys(PhysWorker);
		REQUIRE(t.has_phys(PhysForest));
	}
	
	SECTION("yield food mixed forest no-expert == 4") {
		REQUIRE(4 == t.get_yield(ItemFood, 1));
	}
	
	SECTION("yield food sea") {
		Terr t(AltSea, BiomePlains);
		REQUIRE(t.get_yield(ItemFood, 1) == 4);	
	}
	
			
	
}


TEST_CASE( "load_cargo", "" ) {

	Env env;
	
	auto& u = env.create<Unit>(
		env.create<UnitType>().set_slots(1),
		env.create<Nation>()
	);
	
	u.add(ItemOre, 23);
	REQUIRE(u.get(ItemOre) == 23);
	// regress
	REQUIRE(u.get_space_left() == 77);  
	
}

TEST_CASE( "env::move_unit", "" ) {

	Env env;

	env.resize({2,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,0}, Terr(AltFlat, BiomePlains));

	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND).set_speed(1),
		env.create<Nation>()
	);

	// init
	REQUIRE_NOTHROW(env.init(env.get_terr({0,0}), u));
	REQUIRE(env.get_coords(u) == Coords(0,0));
	
	REQUIRE_NOTHROW(env.start());

	env.set_random_gen(replay({0}));
	// move/board
	REQUIRE(env.move_unit(1, 0, u) == true);
	REQUIRE(env.get_coords(u) == Coords(1,0));


}



TEST_CASE( "equip unit", "" ) {

	Env env;
	env.resize({1,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	auto& t = env.get_terr({0,0});
	
	auto& u = env.create<Unit>(
		env.create<UnitType>().set_base(5).set_equip2(ItemHorses, 50),
		env.create<Nation>()
	);
	env.init(t, u);
	
	auto& ut = env.create<UnitType>().set_base(5).set_equip1(ItemMuskets, 50).set_equip2(ItemHorses, 50);
	
	auto& c = env.create<Colony>();
	env.init(t, c);
	
	Store & st = env.get_store(t);
	
	REQUIRE_NOTHROW(st.add(ItemMuskets, 50));	
	
	REQUIRE_NOTHROW(env.start());
	REQUIRE_NOTHROW(env.equip(u, ut));	
	REQUIRE(u.get_type() == ut);
	REQUIRE(st.get(ItemMuskets) == 0);
	REQUIRE(st.get(ItemHorses) == 0);
	}



TEST_CASE( "board ship", "" ) {

	Env env;
	env.resize({3,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,0}, Terr(AltSea, BiomePlains));
	env.set_terr({2,0}, Terr(AltSea, BiomePlains));
		
	auto& p = env.create<Nation>();
	
	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND).set_speed(1),
		p
	);
	env.init(env.get_terr({0,0}), u);
	
	auto& s = env.create<Unit>(
		env.create<UnitType>().set_travel(SEA).set_speed(2).set_slots(2),
		p
	);
	env.init(env.get_terr({1,0}), s);
		
	REQUIRE(env.get_transport_space(env.get_terr({1,0}), p) == 200);	
	REQUIRE(env.has_transport(env.get_terr({1,0}), u) == true);
	
	REQUIRE_NOTHROW(env.start());
	REQUIRE(env.move_unit(1,0,u) == true);

	REQUIRE(env.get_coords(u) == Coords(1,0));
	REQUIRE(u.transported == true);
	
	REQUIRE(env.move_unit(1,0,s) == true);
	
	REQUIRE(env.get_coords(s) == Coords(2,0));
	REQUIRE(env.get_coords(u) == Coords(2,0));
	REQUIRE(u.transported == true);
	
	//env.order_board(u, {1,0});
	
	//REQUIRE(env.get_pos(u) == Coords(1,0));

}

TEST_CASE( "turn sequence", "" ) {

	Env env;

	auto& p1 = env.create<Nation>();

	env.start();

	SECTION("1p") {
		auto t = env.turn_no;

		REQUIRE(env.get_current_nation().id == p1.id);
		env.ready(p1);
		REQUIRE(env.get_current_nation().id == p1.id);
		REQUIRE(env.turn_no == t + 1);


	}

	auto& p2 = env.create<Nation>();

	SECTION("2p") {
		auto t = env.turn_no;

		REQUIRE(env.get_current_nation().id == p1.id);
		env.ready(p1);
		env.ready(p2);
		REQUIRE(env.get_current_nation().id == p1.id);
		REQUIRE(env.turn_no == t+1);

	}

}


// regress
TEST_CASE( "can_make", "" ) {
	
	Env env;
	env.loads<BuildType>("res/csv/builds.csv");
	env.loads<UnitType>("res/csv/units.csv");
		
	auto & lumber_mill = env.get<BuildType>(BuildLumberMill);
	auto & caravel = env.get<UnitType>(18);
	
	auto r = env.can_make(Build(lumber_mill), caravel);
	REQUIRE(r == false);
}


TEST_CASE( "food_prod", "" ) {
	
	Env env;
	env.loads<BuildType>("res/csv/builds.csv");
	env.resize({1,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	auto & terr = env.get_terr({0,0});
	
	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND),
		env.create<Nation>()
	);

	auto& t = env.get_terr({0,0});	
	REQUIRE_NOTHROW( env.init(t, u) );
	REQUIRE_NOTHROW( env.init_colony(t) );
	REQUIRE_NOTHROW( env.work_field(0, u) );
	auto& f = env.get_field(t, 0);
	REQUIRE_NOTHROW( f.set_proditem(ItemFood) );
	auto & c = t.get_colony();
	auto &  st = env.get_store(t);
	
	REQUIRE(st.get(ItemFood) == 0);
		
	SECTION("produce") {
		REQUIRE_NOTHROW( env.produce(st, f) );	
		REQUIRE((terr.get_yield(ItemFood, 1) - 2) == st.get(ItemFood));
	}
	
	
	
	/*SECTION("resolve") {
		env.resolve<Field, 1, 10>(c);
		REQUIRE(st.get(ItemFood) > 0);
	}
	
	REQUIRE(c.get_max_storage() > 0);
	
	SECTION("turn_terr") {
		env.turn(t);
		REQUIRE(st.get(ItemFood) > 0);
	}*/
	
	
}



TEST_CASE("construct_build") {
	Env env;
	env.resize({1,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	
	auto& unit = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND),
		env.create<Nation>()
	);
	auto& t = env.get_terr({0,0});	
	env.init(t, unit);
			
	env.loads<BuildType>("res/csv/builds.csv");
	
	
	
	auto & bt = env.get<BuildType>(col::BuildCarpentersShop);
	REQUIRE(bt.get_proditem() == ItemHammers);
	
	auto fact = Build(bt);
	fact.add(unit);
	
	Store st;
	st.set(ItemLumber, 100);
	st.set(ItemFood, 100);
	
	auto & lumber_mill = env.get<BuildType>(col::BuildLumberMill);
	fact.set_task(lumber_mill);	
	
	
	REQUIRE( fact.task.get() == 0 );
	env.produce(st, fact);	
	REQUIRE( fact.task.get() > 0 );
	
	
	
	//REQUIRE_NOTHROW(env.start());
}



TEST_CASE( "colony_workplace_production", "" ) {
	
	Env env;
	BuildType::Id const BuildFurTradersHouse = 33;

	env.resize({1,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	
	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND),
		env.create<Nation>()
	);

	auto& t = env.get_terr({0,0});	
	env.init(t, u);
		
	env.loads<BuildType>("res/csv/builds.csv");
	REQUIRE(env.get<BuildType>(BuildFurTradersHouse).get_proditem() == ItemCoats);
	
	REQUIRE_NOTHROW(env.start());

	SECTION("Build.get_prod") {
		auto& u = env.create<Unit>(
			env.create<UnitType>(),
			env.create<Nation>()
		);
		
		auto & bt = env.get<BuildType>(BuildFurTradersHouse);
		auto b = Build(bt);
		b.add(u);
		
		REQUIRE(b.get_proditem() == ItemCoats);
		REQUIRE(b.get_consitem() == ItemFurs);
		
		Amount base = 3;
		
		REQUIRE(b.get_prod(ItemCoats, base) == 3);
		REQUIRE(b.get_cons(ItemCoats, base) == 3);
		
		REQUIRE(b.get_prod(ItemSilver, base) == 0);
		REQUIRE(b.get_cons(ItemSilver, base) == 0);
		
	}	

	
	
	SECTION("build") {
		env.set_random_gen(replay({0}));
		
		
		
		REQUIRE_NOTHROW(env.colonize(u, "aaa"));
		REQUIRE(t.colony != nullptr);

		auto& c = t.get_colony();
		auto & st = env.get_store(t);
	
		SECTION("non_worker_on_colony_hangup") {
			// regress
			// non working unit on colony square causes hangup when turn ends
			REQUIRE_NOTHROW(env.turn());
		}
		
		
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
		
		SECTION("work_field") {
			REQUIRE_NOTHROW(env.work_field(0, u));
			REQUIRE_NOTHROW(env.get_field(t, 0).set_proditem(ItemFood));
			REQUIRE_NOTHROW(env.turn());
			
			Amount base = 1;
			
			// yield - consumption -> produced
			REQUIRE( (t.get_yield(ItemFood, base) - 2) == st.get(ItemFood) );
			// unit used all time
			REQUIRE(u.time_left == 0);
		}

		SECTION("work_build") {
			
			REQUIRE_NOTHROW( env.put_build(t, 0, BuildFurTradersHouse)   );
			REQUIRE( env.get_build(t, 0).get_proditem() == ItemCoats );
			REQUIRE_NOTHROW( env.work_build(0, u) );

			SECTION("just_enough") {
				REQUIRE_NOTHROW(st.add(ItemFood, 2));
				REQUIRE_NOTHROW(st.add(ItemFurs, 3));
				REQUIRE_NOTHROW(env.turn());
				
				REQUIRE(st.get(ItemFood) == 0);
				REQUIRE(st.get(ItemCoats) == 3);
				REQUIRE(st.get(ItemFurs) == 0);
				REQUIRE(u.time_left == 0);
			}

			SECTION("not enough") {
				REQUIRE_NOTHROW(st.add(ItemFood, 2));
				REQUIRE_NOTHROW(st.add(ItemFurs, 1));
				REQUIRE_NOTHROW(env.turn());
				
				REQUIRE(st.get(ItemFood) == 0);
				REQUIRE(st.get(ItemCoats) == 1);
				REQUIRE(st.get(ItemFurs) == 0);
				REQUIRE(u.time_left == 0);
			}

			SECTION("more than enough") {
				REQUIRE_NOTHROW(st.add(ItemFood, 2));
				REQUIRE_NOTHROW(st.add(ItemFurs, 5));
				REQUIRE_NOTHROW(env.turn());

				REQUIRE(st.get(ItemFood) == 0);
				REQUIRE(st.get(ItemCoats) == 3);
				REQUIRE(st.get(ItemFurs) == 2);
				REQUIRE(u.time_left == 0);
			}

		}

	}



}



TEST_CASE( "scoring", "" ) {

	Env env;

	env.resize({2,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,0}, Terr(AltFlat, BiomePlains));

	auto& p1 = env.create<Nation>();
	auto& p2 = env.create<Nation>();

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



TEST_CASE( "serialize", "" ) {

	Env env;

	env.resize({2,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,0}, Terr(AltFlat, BiomePlains));

	auto& u = env.create<Unit>(
		env.create<UnitType>().set_travel(LAND),
		env.create<Nation>()
	);

	env.init(env.get_terr({0,0}), u);

	SECTION("save_load_unstarted_game") {

		std::stringstream stream;

		write(stream, env);
		
		Env env2;
		env2.uts = env.uts;
		read(stream, env2);
		

	}

	SECTION("save_load_started_game") {

		env.start();

		std::stringstream stream;

		write(stream, env);
		
		Env env2;
		env2.uts = env.uts;
		
		read(stream, env2);
		

	}


	/*
	REQUIRE(env.get_coords(u) == Coords(0,0));

	env.set_random_gen(replay({0}));
	env.order_move(u, Dir::D);

	REQUIRE(env.get_coords(u) == Coords(1,0));
	*/

}





TEST_CASE( "two_units", "" ) {

	Env env;

	env.resize({2,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,0}, Terr(AltFlat, BiomePlains));

	auto& t1 = env.get_terr({0,0});
	auto& t2 = env.get_terr({1,0});

	auto& ut = env.create<UnitType>().set_travel(LAND);
	ut.set_attack(2).set_combat(1);

	auto& p = env.create<Nation>();

	auto& u1 = env.create<Unit>(ut, p);
	env.init(t1, u1);

	auto& u2 = env.create<Unit>(ut, p);
	env.init(t2, u2);

	SECTION("have diffrent id") {
		REQUIRE(u1.id != u2.id);  // regress
	}

	SECTION("attack") {
		auto id1 = u1.id;
		auto id2 = u2.id;

		env.set_random_gen(replay({1}));
		env.order_attack(u1, Dir::D);

		REQUIRE(env.exist<Unit>(id1) == true);
		REQUIRE(env.exist<Unit>(id2) == false);
	}
}


TEST_CASE( "compatible", "[env][misc][regress]" ) {		
	REQUIRE(compatible(LAND,SEA) == false);
	REQUIRE(compatible(SEA,LAND) == false);
	REQUIRE(compatible(LAND,LAND) == true);
	REQUIRE(compatible(SEA,SEA) == true);
}


TEST_CASE( "improve square", "" ) {

	Env env;
	env.resize({1,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));

	auto& t = env.get_terr({0,0});
	
	auto& ut1 = env.create<UnitType>().set_base(11).set_equip1(ItemTools, 20);
	auto& ut2 = env.create<UnitType>().set_base(11).set_travel(LAND).set_equip1(ItemTools, 40);
	
	auto& u = env.create<Unit>(
		ut2,
		env.create<Nation>()
	);
	env.init(t, u);

	SECTION("build road") {
		env.set_random_gen(replay({0}));

		REQUIRE_NOTHROW(env.start());
		REQUIRE_NOTHROW( env.improve(u, PhysRoad) );
		REQUIRE( t.has_phys(PhysRoad) );
		REQUIRE( u.get_item1() == ItemTools );
		REQUIRE( u.get_num1() == 20 );

		// cant double-improve
		REQUIRE_THROWS_AS(env.improve(u, PhysRoad), Error);				
	}

	SECTION("plow field") {
		env.set_random_gen(replay({0}));

		REQUIRE_NOTHROW(env.start());
		REQUIRE_NOTHROW( env.improve(u, PhysPlow) );
		REQUIRE( t.has_phys(PhysPlow) );
		REQUIRE( u.get_item1() == ItemTools );
		REQUIRE( u.get_num1() == 20 );
		
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

	io::write<Env>(buf, src);
	//io::read<Env>(trg, buf);


}
 * */
