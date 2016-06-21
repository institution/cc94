#pragma once

#include <chrono>
#include <iostream>
#include <random>

#include "../ext/format.hpp"
#include "env.hpp"
#include "player.hpp"
#include "inter.hpp"
#include "agent.hpp"
#include "random_module.hpp"
#include "exts.hpp"
#include "command.hpp"
#include "path.hpp"
#include "marks.hpp"

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

	using col::Cmds;
	using col::Exts;
	
	
	struct UnitExt {
		Unit::Id id;
		
		Cmds cmds;
										
		UnitExt() = default;
		explicit UnitExt(Unit::Id id): id(id) {}
		UnitExt(UnitExt const&) = delete;
	};
	
	using UnitExts = Exts<Unit, UnitExt>;
	
	/*
	struct Job 
	{
		Cmd cmd;
		float value;
		bool assigned;	
	};
	
	using Jobs = vector<Job>;
	
	struct TileExt 
	{
		float colony_value{0.0f};
		float randvar;
		//float danger;
		
		Jobs jobs;
	};
	
	using TileExts = darray2<TileExt, Coord>;
	*/
	
		
	
	

	Class const ClassSimpleAi = 102;

	struct SimpleAi: Agent {

		RandomModule rm;

		Env & env;
		Nation & nation;
		Nation::Id nation_id;
		//Nation::Auth auth{0};
		
		ext::darray2<float, Coord> val;
		
		//TileExts tile_exts;
		
		
		
		UnitExts ues;
		
		Marks marks;
		
		Terr default_tile;
		v2f empire_center;
		
		Class get_class() const override { return ClassSimpleAi; }
		
		string get_name() const override {
			return format("Simple AI (%||)", nation.get_name());
		}

		bool step() override;
		
		SimpleAi(Env & env, Nation & nation): env(env), nation(nation) 
		{
			nation_id = nation.id;			
		}


		
		
		// -------------
		
		void recalc_tile_exts();

		
		bool has_control(Unit & unit) const { return env.has_control(nation, unit); }		
		bool has_control(Terr & terr) const { return env.has_control(terr, nation); }
		
		UnitExt & get_unit_ext(Unit & unit) { return ues.get(unit.id); }
		UnitExt const& get_unit_ext(Unit const& unit) { return ues.get(unit.id); }
		
		Unit & get_unit(UnitExt & unit_ext) { return env.get_unit(unit_ext.id); }
		Unit const& get_unit(UnitExt const& unit_ext) const { return env.get_unit(unit_ext.id); }

		bool find_field_work(Terr & terr, UnitExt & unit_ext, Field::Id & out);
		bool assign_colony_work(Unit & unit);
		
		bool get_food_item(Terr const& terr) const;	
		bool has_vision(Terr const& terr) const;
		bool is_discovered(Terr const& terr) const;	
		float get_terr_food_value(Terr const& terr) const;
		v2f get_empire_center() const;
		
		
		Coords get_best_colony_spot();

		Terr const& get_tile_def(v2c pos);
		float get_terr_colony_value(v2c pos);
		float rank_colony_tile(v2c pos, float ccolo, float cfood, float cable, float cdist);
		

		bool has_nation() const {
			return nation_id != 0;
		}
		
		Nation & get_nation() { return env.get_nation(nation_id); }
		Nation const& get_nation() const { return env.get_nation(nation_id); }
		
		
		Color get_color() const {
			if (has_nation()) {
				auto c = get_nation().color;
				return {c.r,c.g,c.b,255};
			}
			return {255,0,0,255};
		}
		

		void sync();
			
		
		float get_move_cost(v2c p1, v2c p0, Unit const& u);
		PathRes find_path(v2c src, v2c trg, UnitExt const& ue);
	
		
		void exec_cmds(UnitExt & ext);	

		bool exec_cmd(Unit & unit, Cmds & cmds);
		
		void handle_error() {
			if (env.has_error()) {
				show_error(env.read_error());
			}
		}
		
		void show_error(string const& msg) {
			// TODO
			print("simple_ai.message: %||\n", msg);
		}
		
		
		
		
		
		
	};



}

