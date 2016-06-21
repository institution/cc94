#define CATCH_CONFIG_MAIN
#include "../catch/catch.hpp"

#include "halo.hpp"
#include "console.hpp"

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

TEST_CASE( "console.get_letter", "" ) {
	
	Env env({1,1}, Terr(AltFlat, BiomePlains, PhysForest));
	
	auto & unit = env.create<Unit>(	
		env.create<UnitType>().set_travel(TravelLand).set_speed(1).set_equip1(ItemTools, 20)
	);
	env.init(env.get_terr({0,0}), unit);
			
	unit.time_left = 0;
			
	Console con(env, nullptr);
	
	con.cmd_unit(unit.id, Cmd(InsClear));	
	REQUIRE(con.get_letter(unit) == 'O');
	
}

TEST_CASE( "console.get_next_to_repeat", "" ) {
	
	Env env({1,1}, Terr(AltFlat, BiomePlains, PhysForest));
	
	auto & unit = env.create<Unit>(	
		env.create<UnitType>()
	);
	env.init(env.get_terr({0,0}), unit);
			
	Console con(env, nullptr);
	
	// no command
	REQUIRE(con.get_next_to_repeat(nullptr) == nullptr);
	
	// with command
	con.get_unit_ext(unit).set_cmd(Cmd(InsRoad, 0));
	REQUIRE(con.get_next_to_repeat(nullptr) == &unit);
	
	// with command but can't cont
	con.get_unit_ext(unit).can_cont = false;
	REQUIRE(con.get_next_to_repeat(nullptr) == nullptr);
	
}

TEST_CASE( "console.cmd_unit", "" ) {
	/// can_cont == false after cmd Wait
	
	Env env({1,1}, Terr(AltFlat, BiomePlains, PhysForest));
	
	auto & unit = env.create<Unit>(	
		env.create<UnitType>()
	);
	
	Console con(env, nullptr);
	
	con.cmd_unit(unit.id, Cmd(InsWait));	
	REQUIRE(con.get_unit_ext(unit).can_cont == false);
	
}		

TEST_CASE( "chain.add_cmd", "" ) {
	
	Cmds cs;
	
	REQUIRE(cs.size() == 0);
	
	cs.add(Cmd(InsMove, DirQ));
	
	REQUIRE(cs.size() == 1);
	REQUIRE(cs.at(0) == Cmd(InsMove, DirQ));
	
	cs.add(Cmd(InsMove, DirW));
	
	REQUIRE(cs.size() == 2);
	REQUIRE(cs.at(1) == Cmd(InsMove, DirQ));
	REQUIRE(cs.at(0) == Cmd(InsMove, DirW));
	
	cs.add(Cmd(InsMove, DirE));
	
	REQUIRE(cs.size() == 3);
	REQUIRE(cs.at(2) == Cmd(InsMove, DirQ));
	REQUIRE(cs.at(1) == Cmd(InsMove, DirW));
	REQUIRE(cs.at(0) == Cmd(InsMove, DirE));
	
}	

TEST_CASE( "console.find_path", "" ) {
	
	Env env({2,1}, Terr(AltFlat,BiomePlains,PhysNone));
	
	Console con(env, nullptr);
	
	auto & u = env.create<Unit>(
		env.create<UnitType>().set_travel(TravelLand).set_speed(1)
	);
	
	auto path = con.find_path({0,0},{1,0}, u);
	
	REQUIRE(path.exists());
	REQUIRE(path.cmds.size() == 1);
	REQUIRE(path.cmds.at(0) == Cmd(InsMove, DirD));	
}

TEST_CASE( "console.find_path2", "" ) {
	
	Env env({3,3}, Terr(AltFlat,BiomePlains,PhysNone));
	env.get_terr({0,1}).set_alt(AltSea);
	env.get_terr({1,1}).set_alt(AltSea);
	
	auto & u = env.create<Unit>(
		env.create<UnitType>().set_travel(TravelLand).set_speed(1)
	);
	
	
	Console con(env, nullptr);
	
	auto path = con.find_path({0,0},{0,2},u);
	auto & cs = path.cmds;
	
	REQUIRE(cs.size() == 4);
	REQUIRE(cs.at(3) == Cmd(InsMove, DirD));
	REQUIRE(cs.at(2) == Cmd(InsMove, DirC));
	REQUIRE(cs.at(1) == Cmd(InsMove, DirZ));
	REQUIRE(cs.at(0) == Cmd(InsMove, DirA));	
}


