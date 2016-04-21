#pragma once

#include <chrono>
#include <iostream>
#include <random>

#include "../ext/format.hpp"
#include "env.hpp"
#include "player.hpp"
#include "inter.hpp"
#include "agent.hpp"

namespace simple_ai{
	
	using v2f = ext::v2<float_t>;
	using v2s = ext::v2<int16_t>;
	
	
	using col::Agent;
	using col::Env;
	using col::Nation;
	using col::Unit;
	using col::Amount;
	using col::Terr;
	using col::Coords;
	using col::Coord;
	
	using v2c = ext::v2<Coord>;
	using b2c = ext::b2<Coord>;
	
	using namespace col;
	
	namespace inter = col::inter;

	
	

	
		

	struct SimpleAi: Agent {

		std::default_random_engine random_engine;

		Env & env;
		Nation & nation;
		Nation::Id nation_id;
		//Nation::Auth auth{0};
		
		ext::darray2<float, Coord> val;
		
		
		
		
		

		bool step();
		
		SimpleAi(Env & env, Nation & nation): env(env), nation(nation) 
		{
			nation_id = nation.id;			
			random_engine.seed(std::random_device{}());
		}

		

		// -------------
		
		auto & get_random_engine();
		float random_number(float x, float y);
		
		bool get_food_item(Terr const& terr) const;	
		bool has_vision(Terr const& terr) const;
		bool is_discovered(Terr const& terr) const;	
		float get_terr_food_value(Terr const& terr) const;
		float get_terr_colony_value(Terr const& terr, v2f center);
		v2f get_empire_center() const;
		
		Coords get_best_colony_spot();
		

		void sync();
			
	

	};



}

