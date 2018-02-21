#define CATCH_CONFIG_MAIN
#include "../catch/catch.hpp"


#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


#include "env.hpp"
#include "build.hpp"
#include "serialize.hpp"
#include "serialize3.hpp"


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
 v 0.2.0 *
 
 * open graphics generator, terrain/textures * 
 * unified construct building/unit framework * 
 i 14722 LOC *
 * equip pioneers, soldiers gui improv *
 * select field production gui improv * 
 * show colony resources ballance * 
 
 * AA font 64 base *  
 * tileset folder can be passed as cmd arg *
 
 * arbitrary map size *
 *  scroll map with arrows *
 *  resize command *
 
 * editor hlinks *
 
 * base specialists *
 *  unit can have speciality *
 *  teaching (school) *
 *  speciality affects productivity *
 *  clear speciality (edit) *
 
 * idle factory button -> zoom to colony *
 
 * fog of war *
 
 * path-finding + gui *
     
 * simple AI, colony building *
 *  choose place: >1forest, >1ore, nearby(<4tile) but do not overlap(>3tile) *
 *  move nearest colonist and build *
 
 * simple AI: task creation
 *  colonize
 *  discover
 *  improve
 *  work food  
 
 * simple AI: task assigment
 *  assign tasks to units
 
 * simple AI, colony managment; maximize food prod
 *  assign to work: food, ore, lumber
 *  improve tiles
 *  construct blacksmith house
  
 * simple AI:
 *  construct in fixed order: docks, lumber mill, ...
 
 * teaching: block 2 teachers one student, unit attr
 * teaching: choose best student, search units
  
 v 0.2.8
 
 * menu: save/load
 * gui improvments
 * somehow playable scenario map
 
 v 0.3.0
  
 * cargo gui rework
 *  arbitrary number of items *
 *  drop cargo on transport
 *  square cargo icons
 
 * travel to europe by sea - exit_map(ship, dest) order - trade
 * europe screen
 * sell/buy in europe  
  
 * trade framework
 * trade with forein powers/indians
 
 * simple AI, trade
 
 * teaching revisited ?
 
 * v0.3.5
  
 * land combat 
 * artillery fire during enemy turn?
 * temporary fortification
 * simple AI - combat
 
 * food as default field prod ?
 * v0.4.0
  
 * repair unit
 * specialist by practice
 
 * v0.4.5
 
 * startup time log
 
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
 
 * hires layout
 *  AA font - use font_height in layout
 
 
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


TEST_CASE( "serialize", "" ) {
	
	SECTION("int16") {
		std::stringstream s;

		int16_t x = 0x4063;
		write(s, x);
		int16_t y;
		read(s, y);
		
		REQUIRE(x == y);		
	}
}


TEST_CASE( "env", "" ) {

	SECTION("constructor") {
		Env env({1,1}, Terr{AltSea, BiomeTundra});
		
		auto & t = env.get_terr({0,0});

		REQUIRE(t.get_alt() == AltSea);
		REQUIRE(t.get_biome() == BiomeTundra);
	}
	
	SECTION("clean_units") {
		Env env({1,1}, Terr{AltFlat, BiomePlains});
		
		auto & u = env.create<Unit>(env.get_unittype(0));
		
		env.put(env.get_terr({0,0}), u);
		env.clear_units();
		
		REQUIRE(env.units.size() == 0);
		REQUIRE(env.get_terr({0,0}).units.size() == 0);
	}
	
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
	
}


TEST_CASE( "load_cargo", "" ) {

	Env env;
	
	auto& u = env.create<Unit>(
		env.get_unittype(1).set_slots(1)
	);
	
	u.add(ItemOre, 23);
	REQUIRE(u.get(ItemOre) == 23);
	// regress
	REQUIRE(u.get_space_left() == 77);  
	
}


void require_no_error(Env & env) {
	if (env.has_error()) {
		print("INFO: %||\n", env.read_error());	
		REQUIRE(false);	// env.has_error() should be false
	}	
}

TEST_CASE( "compatible", "[env][misc][regress]" ) {		
	REQUIRE(compatible(TravelLand,TravelSea) == false);
	REQUIRE(compatible(TravelSea,TravelLand) == false);
	REQUIRE(compatible(TravelLand,TravelLand) == true);
	REQUIRE(compatible(TravelSea,TravelSea) == true);
}

TEST_CASE( "move_unit", "[env][order]" ) 
{
	Env env({2,1}, Terr{AltFlat, BiomePlains});
	
	auto & u = env.create<Unit>(
		env.get_unittype(1).set_travel(TravelLand).set_speed(1)
	);

	env.put(env.get_terr({0,0}), u);

	REQUIRE(env.get_coords(u) == Coords(0,0));	
	CHECK(env.move_unit(u, DirD) == ResOk);
	require_no_error(env);
	REQUIRE(env.get_coords(u) == Coords(1,0));
	
}



TEST_CASE( "equip unit", "" ) {

	Env env;
	env.resize({1,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	auto& t = env.get_terr({0,0});
	
	auto& u = env.create<Unit>(
		env.get_unittype(1).set_base(5).set_equip2(ItemHorses, 50)
	);
	env.put(t, u);
	
	auto& ut = env.get_unittype(1).set_base(5).set_equip1(ItemMuskets, 50).set_equip2(ItemHorses, 50);
	
	auto& c = env.create<Colony>();
	env.put(t, c);
	
	Store & st = env.get_store(t);
	
	REQUIRE_NOTHROW(st.add(ItemMuskets, 50));	
	
	REQUIRE_NOTHROW(env.equip(u, ut));	
	REQUIRE(u.get_type() == ut);
	REQUIRE(st.get(ItemMuskets) == 0);
	REQUIRE(st.get(ItemHorses) == 0);
}


/*
TEST_CASE( "board ship", "" ) {

	Env env;
	env.resize({3,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,0}, Terr(AltSea, BiomePlains));
	env.set_terr({2,0}, Terr(AltSea, BiomePlains));
		
	auto& u = env.create<Unit>(
		env.get_unittype(1).set_travel(TravelLand).set_speed(1)
	);
	
	env.put(env.get_terr({0,0}), u);
	
	auto& s = env.create<Unit>(
		env.get_unittype(1).set_travel(TravelSea).set_speed(2).set_slots(2)
	);
	env.put(env.get_terr({1,0}), s);
		
	
	REQUIRE(env.find_transport(env.get_terr({1,0}), u) != nullptr);
	
	REQUIRE(env.move_unit(u, {1,0}) == ResOk);

	REQUIRE(env.get_coords(u) == Coords(1,0));
	REQUIRE(u.is_transported() == true);
	
	REQUIRE(env.move_unit(s, {1,0}) == ResOk);
	
	REQUIRE(env.get_coords(s) == Coords(2,0));
	REQUIRE(env.get_coords(u) == Coords(2,0));
	REQUIRE(u.is_transported() == true);
	
	//env.order_board(u, {1,0});
	
	//REQUIRE(env.get_pos(u) == Coords(1,0));

}*/

TEST_CASE( "turn sequence", "" ) {

	Env env;

	SECTION("0p") {
		auto t = env.turn_no;

		REQUIRE(env.get_current_control() == ControlNone);
		env.ready();
		REQUIRE(env.get_current_control() == ControlNone);
		REQUIRE(env.turn_no == t + 1);

	}

	auto & p1 = env.create<Faction>("England");

	SECTION("1p") {
	
		auto t = env.turn_no;

		REQUIRE(env.get_current_control() == ControlNone);
		
		env.ready();
		REQUIRE(env.get_current_control() == p1.get_control());
		REQUIRE(env.turn_no == t);

		env.ready();
		REQUIRE(env.get_current_control() == p1.get_control());
		REQUIRE(env.turn_no == t + 1);


	}


}


// regress
TEST_CASE( "can_make", "" ) {
	
	Env env;

	read_defs("res/csv/defs.rc", env);
		
	auto & lumber_mill = env.get_buildtype(BuildLumberMill);
	auto & caravel = env.get_unittype(18);
	
	auto r = env.can_make(Build(lumber_mill), caravel);
	REQUIRE(r == false);
}


TEST_CASE( "food_prod", "" ) {
	
	Env env({1,1}, Terr(AltFlat, BiomePlains));
	read_defs("res/csv/defs.rc", env);
	
	auto & terr = env.get_terr({0,0});
	
	auto& u = env.create<Unit>(
		env.get_unittype(1).set_travel(TravelLand)		
	);

	auto& t = env.get_terr({0,0});	
	env.put(t, u);
	env.init_colony(t);
	REQUIRE( env.work_field(0, u) == ResOk );
	
	auto& f = env.get_field(t, 0);
	REQUIRE_NOTHROW( f.set_proditem(ItemFood) );
	auto & c = t.get_colony();
	auto &  st = env.get_store(t);
	
	REQUIRE(st.get(ItemFood) == 0);
		
	SECTION("produce") {
		REQUIRE_NOTHROW( env.produce(st, f) );	
		REQUIRE((env.get_prod(terr, u, ItemFood) - 2) == st.get(ItemFood));
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

	Env env({1,1}, Terr(AltFlat, BiomePlains));
	
	auto& unit = env.create<Unit>(
		env.get_unittype(1).set_travel(TravelLand)
	);
	
	auto& t = env.get_terr({0,0});	
	env.put(t, unit);
			
	read_defs("res/csv/defs.rc", env);
	
	
	
	auto & bt = env.get_buildtype(col::BuildCarpentersShop);
	REQUIRE(bt.get_proditem() == ItemHammers);
	
	auto fact = Build(bt);
	fact.add(unit);
	
	Store st;
	st.set(ItemLumber, 100);
	st.set(ItemFood, 100);
	
	auto & lumber_mill = env.get_buildtype(col::BuildLumberMill);
	fact.set_task(lumber_mill);	
	
	
	REQUIRE( fact.task.get() == 0 );
	env.produce(st, fact);	
	REQUIRE( fact.task.get() > 0 );
	
	
	
	//REQUIRE_NOTHROW(env.start());
}

TEST_CASE( "is_expert", "" ) {	
	Unit unit;
	unit.set_prof(ProfCarpenter);
	REQUIRE(unit.get_prof() == ProfCarpenter);
	REQUIRE(get_prof_by_item(ItemHammers) == ProfCarpenter);
	
	REQUIRE(is_expert(unit, ItemHammers) == true);
}



TEST_CASE( "colony_workplace_production", "" ) {
	
	Env env({1,1}, Terr(AltFlat, BiomePlains));	
		
	auto& u = env.create<Unit>(
		env.get_unittype(1).set_travel(TravelLand)
	);

	auto& t = env.get_terr({0,0});	
	env.put(t, u);
		
		
	auto& bt = env.get_buildtype(1)
		.set_slots(3)
		.set_prod(ItemCoats, 3)
		.set_cons(ItemFurs, 3);
		
	// REQUIRE_NOTHROW(env.start());

	SECTION("Build.get_prod") {
		auto& u = env.create<Unit>(env.get_unittype(1));
		
		auto b = Build(bt);
		b.add(u);
		
		REQUIRE(b.get_proditem() == ItemCoats);
		REQUIRE(b.get_consitem() == ItemFurs);
		
		REQUIRE(env.get_prod(b, u, ItemCoats) == 3);
		REQUIRE(env.get_cons(b, u, ItemCoats) == 3);
		
		REQUIRE(env.get_prod(b, u, ItemSilver) == 0);
		REQUIRE(env.get_cons(b, u, ItemSilver) == 0);
		
	}	

	SECTION("build") {
		env.set_random_gen(replay({0}));
		
		env.put(t, env.create<Colony>());
		env.get_build(t, 0).set_type(bt);
		t.get_colony().add_field(Field(env.get_terr({0,0})));
		
		
		auto & st = env.get_store(t);
	
		
	
		SECTION("non_worker_on_colony_hangup") {
			// regress
			// non working unit on colony square causes hangup when turn ends
			REQUIRE_NOTHROW(env.turn());
		}		
		
		
		
		SECTION("multiple assign to same workplace") {
			REQUIRE(env.work_build(0, u) == ResOk);
			REQUIRE(env.work_build(0, u) == ResOk);
			REQUIRE(env.work_build(0, u) == ResOk);
			REQUIRE(env.work_build(0, u) == ResOk);
			REQUIRE(env.work_build(0, u) == ResOk);
			REQUIRE(env.work_build(0, u) == ResOk);
			REQUIRE(env.work_build(0, u) == ResOk);
			REQUIRE(env.work_build(0, u) == ResOk);			
		}
		
		SECTION("work_field") {
			REQUIRE_NOTHROW(env.work_field(0, u));
			REQUIRE_NOTHROW(env.get_field(t, 0).set_proditem(ItemFood));
			REQUIRE_NOTHROW(env.turn());
			
			Amount base = 1;
			
			// yield - consumption -> produced
			REQUIRE( (env.get_prod(t, u, ItemFood) - 2) == st.get(ItemFood) );
			// unit used all time
			REQUIRE(u.time_left == 0);
		}

		SECTION("work_build") {
			
			auto & b = env.get_build(t, 0);
			REQUIRE_NOTHROW( b.set_type(bt) );
			REQUIRE( b.get_proditem() == ItemCoats );
			REQUIRE( env.work_build(0, u) == ResOk );
			

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






TEST_CASE( "serialize2", "" ) {

	Env env;

	env.resize({2,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,0}, Terr(AltFlat, BiomePlains));

	auto& u = env.create<Unit>(
		env.get_unittype(1).set_travel(TravelLand),
		env.create<Faction>()
	);

	env.put(env.get_terr({0,0}), u);

	SECTION("save_load_unstarted_game") {

		std::stringstream stream;

		write(stream, env);
		
		Env env2;
		env2.uts = env.uts;
		read(stream, env2);
		

	}

	SECTION("save_load_started_game") {

		std::stringstream stream;

		write(stream, env);
		
		Env env2;
		env2.uts = env.uts;
		
		read(stream, env2);
		

	}


	/*
	REQUIRE(env.get_coords(u) == Coords(0,0));

	env.set_random_gen(replay({0}));
	env.order_move(u, DirD);

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

	auto& ut = env.get_unittype(1).set_travel(TravelLand);
	ut.set_attack(2).set_combat(1);

	auto& p = env.create<Faction>();

	auto& u1 = env.create<Unit>(ut, p);
	env.put(t1, u1);

	auto& u2 = env.create<Unit>(ut, p);
	env.put(t2, u2);

	SECTION("have diffrent id") {
		REQUIRE(u1.id != u2.id);  // regress
	}

	SECTION("attack") {
		auto id1 = u1.id;
		auto id2 = u2.id;

		env.set_random_gen(replay({1}));
		env.order_attack(u1, DirD);

		REQUIRE(env.exist<Unit>(id1) == true);
		REQUIRE(env.exist<Unit>(id2) == false);
	}
}




TEST_CASE( "improve square", "" ) {

	Env env;
	env.resize({1,1});
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));

	auto & t = env.get_terr({0,0});
	
	auto & ut1 = env.get_unittype(1).set_base(11).set_equip1(ItemTools, 20);
	auto & ut2 = env.get_unittype(2).set_base(11).set_travel(TravelLand).set_equip1(ItemTools, 40);
	
	auto & u = env.create<Unit>(
		ut2,
		env.create<Faction>()
	);
	env.put(t, u);

	SECTION("build road") {
		env.set_random_gen(replay({0}));

		//REQUIRE_NOTHROW(env.start());
		REQUIRE_NOTHROW( env.improve(u, PhysRoad) );
		REQUIRE( t.has_phys(PhysRoad) );
		REQUIRE( u.get_item1() == ItemTools );
		REQUIRE( u.get_num1() == 20 );

		// cant double-improve
		REQUIRE(not env.has_error());
		env.improve(u, PhysRoad);		
		REQUIRE(env.has_error());
	}

	SECTION("plow field") {
		env.set_random_gen(replay({0}));

		//REQUIRE_NOTHROW(env.start());
		REQUIRE_NOTHROW( env.improve(u, PhysPlow) );
		REQUIRE( t.has_phys(PhysPlow) );
		REQUIRE( u.get_item1() == ItemTools );
		REQUIRE( u.get_num1() == 20 );
		
	}


}



TEST_CASE( "move cost", "" ) {

	// move cost impossible between land and ocean

	Env env;
	env.resize({2,2});
	
	env.set_terr({0,0}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,0}, Terr(AltSea, BiomePlains));
	env.set_terr({0,1}, Terr(AltFlat, BiomePlains));
	env.set_terr({1,1}, Terr(AltSea, BiomePlains));
	
	
	auto& t00 = env.get_terr({0,0});
	auto& t10 = env.get_terr({1,0});
	auto& t01 = env.get_terr({0,1});
	auto& t11 = env.get_terr({1,1});
	
	//env.init_colony(t00);
	t01.add_phys(PhysMinorRiver);
	
	
	auto& u = env.create<Unit>(
		env.get_unittype(1).set_travel(TravelLand).set_speed(1)
	);
	
	REQUIRE(env.get_move_cost(t10, t00, u) == TimeInf);
	REQUIRE(env.get_move_cost(t01, t00, u) < TimeInf);


}

/*
TEST_CASE( "io::write<Env>", "[env]" ) {

	Env env;

	env.resize({1,1}).set_terr({0,0}, Terr(BIOME_PLAINS));

	auto t = env.get_terr({0,0});

	auto u = env.create<Unit>(
		1,
		env.get_unittype(1).set_travel(TravelLand)
	);

	env.move_in(t, u);



	std::stringstream buf;

	io::write<Env>(buf, src);
	//io::read<Env>(trg, buf);


}
 * */
