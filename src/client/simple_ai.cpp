#include "simple_ai.hpp"


namespace simple_ai{

	
	/*struct Map{
		darray2<TerrInfo> map;
		
	};*/
	
	
	
	
	float dist(v2f x, v2f y) {
		auto d = v2f(x - y);
		return std::abs(d[0]) + std::abs(d[1]);		
	}

	void SimpleAi::sync() {
		if (env.get_dim() != val.get_dim()) {
			val.resize(env.get_dim(), 0.0f);
		
			//tile_exts.resize(env.get_dim());
			
			//recalc_tile_exts();
		}
		
		
	}
	
	bool SimpleAi::get_food_item(Terr const& terr) const {
		if (terr.get_alt() == AltSea) {
			return ItemFish;
		}
		else {
			return ItemFood;
		}
	}
	
	bool SimpleAi::has_vision(Terr const& terr) const {
		return has_control() and terr.get_vision(get_control());
	}

	bool SimpleAi::is_discovered(Terr const& terr) const {
		return has_control() and terr.get_discovered(get_control());
	}
	
	float SimpleAi::get_terr_food_value(Terr const& terr) const {
		if (is_discovered(terr)) 
		{			
			return std::max(
				env.get_base_prod(terr, ItemFood), 
				env.get_base_prod(terr, ItemFish)
			);
		}
		else {
			return 0.0f;
		}
	}
	
	
	Terr const& SimpleAi::get_tile_def(v2c pos)
	{
		if (env.in_bounds(pos)) {
			auto & terr = env.get_terr(pos);
			if (is_discovered(terr)) {
				return terr;
			}
		}
		return default_tile;
	}
	
	
	
	float SimpleAi::rank_colony_tile(v2c pos, float ccolo, float cfood, float cable, float cdist) 
	{
		auto & terr = get_tile_def(pos);
		
		float v = 0.0f;
		v += ccolo * terr.has_colony();
		v += cfood * get_terr_food_value(terr);
		v += cable * env.is_colonizable(terr);
		v += cdist / (dist(v2f(pos), empire_center) + 1.0f);
		v += rm.random_float(0.0f, 0.1f);
		return v;
	}
	
	float SimpleAi::get_terr_colony_value(v2c p) 
	{
		// y y y y y
		// y x x x y
		// y x z x y
		// y x x x y
		// y y y y y
		
		float v = 0.0f;
		
		v += rank_colony_tile(p + v2c(-2,-2), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c(-1,-2), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c( 0,-2), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c(+1,-2), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c(+2,-2), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c(-2,-1), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c(+2,-1), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c(-2,-0), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c(+2, 0), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c(-2,+1), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c(+2,+1), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c(-2,+2), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c(-1,+2), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c( 0,+2), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c(+1,+2), -100, 0, 0, 0);
		v += rank_colony_tile(p + v2c(+2,+2), -100, 0, 0, 0);
				
		v += rank_colony_tile(p + v2c(-1,-1), -200, 1, 0, 0);
		v += rank_colony_tile(p + v2c( 0,-1), -200, 1, 0, 0);
		v += rank_colony_tile(p + v2c(+1,-1), -200, 1, 0, 0);
		
		v += rank_colony_tile(p + v2c(-1, 0), -200, 1, 0, 0);
		v += rank_colony_tile(p + v2c( 0, 0), -1000, 1, 1000, 2);
		v += rank_colony_tile(p + v2c(+1, 0), -200, 1, 0, 0);
		
		v += rank_colony_tile(p + v2c(-1,+1), -200, 1, 0, 0);
		v += rank_colony_tile(p + v2c( 0,+1), -200, 1, 0, 0);
		v += rank_colony_tile(p + v2c(+1,+1), -200, 1, 0, 0);
		
		return v;
		
	}
	
	
	/// What is the value of job 'cmd performed by 'unit starting in 'tdist turns
	float get_job_value(Unit const& unit, Cmd cmd, int tdist)
	{
		return 1.0f;
		
		
	}
	
	
	/*
	void SimpleAi::recalc_tile_exts() 
	{
		for (auto & te: tile_exts) 
		{
			auto pos = tile_exts.get_pos(te);
			auto const& t = env.get_terr(pos);
			
			if (is_discovered(t))
			{
				te.colony_value = get_terr_colony_value(pos);
			}
		}
		
		
	
			

	}
	
	struct JobVal 
	{
		Job job;
		float val;
	};
	
	using Job = int8_t;
	Job const
		JobColonize
		JobWorkFood
		JobDiscover
	
	/// job val calculation pipe
	/// + tile   terrain  jv
	/// + unit   adjusted jv
	/// + dist   delayed  jv
	
	JobVal adjusted_jv_colonize(JobVal jv, Unit::Id unit_id) 
	{
		auto & u = env.get_unit(unit_id);
		return JobVal(jv.job, jv.val * float(env.is_colonist(u)));		
	}
	
	JobVal adjusted_jv_work_food(JobVal jv, Unit::Id unit_id) 
	{
		auto & u = env.get_unit(unit_id);
		auto & te = get_tile_ext(pos);
		return jv;
	}
	
	JobVal adjusted_jv_discover(JobVal jv, Unit::Id unit_id) 
	{
		return jv;
	}
	
	
	JobVal adjusted_jv(JobVal jv, Unit::Id unit_id) 
	{
		switch (jv.job)
		{
			case JobColonize: return adjusted_jv_colonize(jv, unit_id);
			case JobWorkFood: return adjusted_jv_work_food(jv, unit_id);
			case JobDiscover: return adjusted_jv_discover(jv, unit_id);
			// case Job: return adjusted_jv_(jv, unit_id);
			default: print(std::err, "WARNING: adjusted_jv: unknown job"); break;
		}
	
	}
		
	float get_delayed_value(float reward, float delay) {
		float q = 0.9f;
		return value * std::pow(q, delay);
	}
	
	
	
	jobs_here()	
		
		// colonize
		get_terr_colony_value
		
		// work food
		
		// discover
		is_discovered(pos)
	
	
	
	*/
	
	
	/*bool SimpleAi::find_field_work(Terr & terr, UnitExt & unit_ext, Field::Id & out) 
	{
		auto const& c = terr.get_colony();
		auto & unit = get_unit(unit_ext);
		
		for (size_t i = 0; i < c.fields.size(); ++i)
		{
			auto & field = c.fields.at(i);
			
			
			if (field.get_space_left() > 0)
			{			
				if (env.get_prod(field.get_terr(), unit, ItemFood) > 2) 
				{						
					env.set_proditem_field(terr, i, ItemFood);
					out = i;
					return true;
				}
				else {
					print("simple_ai: find_field_work: field=%||; NO PROD\n", i);
				}
			}
			else {
				print("simple_ai: find_field_work: field=%||; NO SPACE\n", i);
			}
		}			
		return false;
	}*/
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	/*
	Job best_job_here(v2c pos, Unit::Id unit_id, float delay) 
	{
		auto const& te = get_tile_ext(pos);
		for (auto jv0: te.jobvals) {
			
			auto jv = jv0;
			
			jv = adjusted_jv(jv, unit_id);
			jv = delayed_jv(jv, tdist);
			
			if (jv.val > best_jv.val) {
				best_jv = jv;
			}
		}
		
	}
	*/
	
	// TODO: should also return empire radius
	v2f SimpleAi::get_empire_center() const {
		auto total = v2f(0,0);
		float count = 0;
		
		for (auto const& unit: list_values(env.units)) {
			if (env.has_control(cc, unit)) {
				total += v2f(env.get_coords(unit));
				count += 1;
			}
		}
		
		return total * (1.0f/count);
	}
	
	
	
	template<class T, class U, class F>
	void for_area(ext::darray2<T,U> & array, ext::b2<U> rect, F f) 
	{
		auto p = rect.pos;	
		auto r = rect.dim;
		auto dim = array.get_dim();
				
		auto i0 = std::max<U>(0, p[0]-r[0]);
		auto i1 = std::min<U>(p[0]+r[0], dim[0]-1);
		
		auto j0 = std::max<U>(0, p[1]-r[1]);
		auto j1 = std::min<U>(p[1]+r[1], dim[1]-1);
		
		for (U j = j0; j <= j1; ++j)
		{
			for (U i = i0; i <= i1; ++i)
			{
				f(array({i,j}));
			}				
		}		
	}
	
	
	Coords SimpleAi::get_best_colony_spot() 
	{
		empire_center = get_empire_center();
		auto radius = v2c(20.0f, 20.0f);
				
		auto area = b2c(Coords(empire_center - radius), Coords(radius * 2));
		
		Terr const* best_terr{nullptr};
		float best_val{0};
		
		for_area(env.terrs, area, [this, &best_terr, &best_val](Terr const& terr) {
			auto val = get_terr_colony_value(env.get_coords(terr));
			if (val > best_val) {
				best_val = val;
				best_terr = &terr;
			}
		});
		
		return env.get_coords(*best_terr);	
	}
	

	bool SimpleAi::find_field_work(Terr & terr, UnitExt & unit_ext, Field::Id & out) 
	{
		auto const& c = terr.get_colony();
		auto & unit = get_unit(unit_ext);
		
		for (size_t i = 0; i < c.fields.size(); ++i)
		{
			auto & field = c.fields.at(i);
			
			
			if (field.get_space_left() > 0)
			{			
				if (env.get_prod(field.get_terr(), unit, ItemFood) > 2) 
				{						
					env.set_proditem_field(terr, i, ItemFood);
					out = i;
					return true;
				}
				else {
					print("simple_ai: find_field_work: field=%||; NO PROD\n", i);
				}
			}
			else {
				print("simple_ai: find_field_work: field=%||; NO SPACE\n", i);
			}
		}			
		return false;
	}
	
	
	
	bool SimpleAi::assign_colony_work(Unit & unit) 
	{
		print("simple_ai: assign_colony_work: BEGIN; unit=%||\n", unit.id);
		auto & terr = env.get_terr(unit);
		
		auto & unit_ext = get_unit_ext(unit);
		
		// if on colony 
		if (terr.has_colony() and has_control(terr)) 
		{
			print("simple_ai: assign_colony_work: on colony\n");
		
			Field::Id field_id = -1;
			
			// and can work
			if (find_field_work(terr, unit_ext, field_id)) 
			{
				print("simple_ai: assign_colony_work: OK; field=%||\n", field_id);
				
				// work at field
				env.work_field(field_id, unit);
				return true;
			}
			
		}
		
		print("simple_ai: assign_colony_work; FAIL\n");
		return false;
	}


	auto const Infinity = std::numeric_limits<float>::infinity();

	
	/// return move cost as percived by AI
	float SimpleAi::get_move_cost(v2c p1, v2c p0, Unit const& u) {
		auto const& t0 = env.get_terr(p0);
		auto const& t1 = env.get_terr(p1);
		
		if (is_discovered(t1) and is_discovered(t0)) {
			auto x = env.get_move_cost(t1, t0, u);
			if (x == TimeInf) {
				return Infinity;
			}
			else {
				return x + rm.random_float(0.0f, 0.01f);
			}
		}
		else {
			return time2float(TimeUnit * 2);
		}
		
	}

	PathRes SimpleAi::find_path(v2c src, v2c trg, UnitExt const& ue) 
	{
		auto const& u = get_unit(ue);
	
		return col::find_path(env, src, trg, [this,&u](v2c w1, v2c w0) {		
			return get_move_cost(w1, w0, u);			
		});	
	}



	bool SimpleAi::step() {
		
		sync();
		
		
		if (env.in_progress() and env.get_current_control() == cc) 
		{
			marks.clear();
		
			print("simple_ai: units\n");
			for (auto & unit: list_values(env.units))
			{
				if (env.has_control(cc, unit)) 
				{
					// what this unit should do?
					auto & unit_ext = ues.get(unit);
					
					// if no order
					if (not unit_ext.cmds.has()) 
					{
						if (assign_colony_work(unit)) {
							print("simple_ai: unit(%||) assigned colony work\n", unit.id);
						}
						else {
						
							auto & terr = env.get_terr(unit);
							auto unit_pos = env.get_coords(terr);
						
							// add orders
							
							auto col_pos = get_best_colony_spot();
							
							marks.add(col_pos, "col");
							
							if (env.get_terr(col_pos) == terr) 
							{
								print("simple_ai: unit(%||) colonize at %||\n", unit.id, col_pos);
							
								// colonize
								env.colonize(unit);
							}
							else {
								print("simple_ai: unit(%||) move to colonize %||\n", unit.id, col_pos);
							
								// move to good colony spot and colonize
								auto path = find_path(unit_pos, col_pos, unit_ext);
								
								unit_ext.cmds.clear();
								
								if (std::isinf(path.cost)) {									
									print("cmds: %||\n", path.cmds);
									unit_ext.cmds.adds(path.cmds);
									unit_ext.cmds.add(Cmd(InsBuild));
								}
							}
							
						}
					}
					else {
						// exec current orders
						exec_cmds(unit_ext);
					}
										
					
					// move in random direction
					//auto dir = Coords(rm.random_int8(-1,+1), rm.random_int8(-1,+1));
					//env.move_unit(dir[0], dir[1], unit);
				}
			}
			
			print("simple_ai: ready\n");
			env.ready();
		}
		
		return true;
	}




	void SimpleAi::exec_cmds(UnitExt & ext) 
	{
		auto & cmds = ext.cmds;
		Unit & unit = env.get_unit(ext.id);
		
		while (cmds.has() and exec_cmd(unit, cmds)) {
			handle_error();
		}
		handle_error();
		
		
	}
	


	/// Make 'unit do current command.
	/// return true when unit is ready to execute next command
	/// return false when current command wasn't finished yet
	bool SimpleAi::exec_cmd(Unit & unit, Cmds & cmds) 
	{	
		//print("SimpleAi.exec_cmd: id=%||, cmd=%||\n", ext.id, ext.get_curr_cmd());
	
		// assert(ext.cmds.has());
	
		auto cmd = cmds.get();
				
		switch (cmd.ins) {
			case InsNone: {
				cmds.pop();
				return 1;
			}					
			case InsWait: {
				return 0;
			}										
			case InsMove: {
				auto dir = (Dir)cmd.arg;
				auto r = env.move_unit(unit, dir);
				
				if (r) {
					cmds.pop();
				}
				return r;
			}
			case InsAttack: {
				auto dir = (Dir)cmd.arg;
				// auto r = env.move_unit(dir, unit);
				auto r = 1;  // TODO ^
				if (r) {
					cmds.pop();
				}
				return r;
			}
			case InsBuild: {					
				auto r = env.colonize(unit);
				if (r) { cmds.pop(); }
				return r;
			}
			case InsClear: {
				auto r = env.destroy(unit, PhysForest);
				if (r) { cmds.pop(); }
				return r;
			}
			case InsRoad: {
				auto r = env.improve(unit, PhysRoad);
				if (r) { cmds.pop(); }
				return r;					
			}
			case InsPlow: {
				auto r = env.improve(unit, PhysPlow);
				if (r) { cmds.pop(); }
				return r;
			}
			case InsWork: {
				return 0;
			}
			default: {
				print(std::cerr, "WARNING: unknown unit command code: %||\n", cmd.ins);
				cmds.pop();
				return 1;
			}
		}
		

	}

}
