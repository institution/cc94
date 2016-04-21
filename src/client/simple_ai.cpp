#include "simple_ai.hpp"


namespace simple_ai{

	
	/*struct Map{
		darray2<TerrInfo> map;
		
	};*/
	
	
	
	
	float dist(v2f x, v2f y) {
		auto d = v2f(x - y);
		return std::abs(d[0]) + std::abs(d[1]);		
	}
	
	auto & SimpleAi::get_random_engine() {
		return random_engine;
	}
	

	float SimpleAi::random_number(float x, float y)
	{
		static std::uniform_real_distribution<float> d{};

		using parm_t = decltype(d)::param_type;
		return d(get_random_engine(), parm_t{x, y});
	}

	
	void SimpleAi::sync() {
		if (env.get_dim() != val.get_dim()) {
			val.resize(env.get_dim(), 0.0f);		
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
		return terr.get_vision(nation_id);
	}

	bool SimpleAi::is_discovered(Terr const& terr) const {
		return terr.get_discovered(nation_id);
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
	
	
	
	float SimpleAi::get_terr_colony_value(Terr const& terr, v2f center) 
	{
		if (is_discovered(terr) and env.is_colonizable(terr)) 
		{		
			auto pos = env.get_coords(terr);
	
			// distance from center point
			auto d = dist(v2f(pos), center);
			
			if (3.0f <= d and d < 30.0f) {
				
				// distance value goes like: 10, 7.5, 6
				float val = 30.0f / d;
				
				// food value: 0,2,4 per tile * 9 -> 18
				for (auto j = -1; j <= 1; ++j) {
					for (auto i = -1; i <= 1; ++i) {
						val += get_terr_food_value(
							env.get_terr(pos - Coords(i,j))
						);
					}
				}	
				
				// minimal randomization to avoid bias in one direction
				val += random_number(0.0f, 1.0f);
				
				return val;
			}
		}
		
		return 0;
		
	}
	
	
	// TODO: should also return empire radius
	v2f SimpleAi::get_empire_center() const {
		auto total = v2f(0,0);
		float count = 0;
		
		for (auto const& unit: list_values(env.units)) {
			if (env.has_control(nation, unit)) {
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
		auto center = get_empire_center();
		auto radius = v2c(20.0f, 20.0f);
				
		auto area = b2c(Coords(center - radius), Coords(radius * 2));
		
		Terr const* best_terr{nullptr};
		float best_val{0};
		
		for_area(env.terrs, area, [this, center, &best_terr, &best_val](Terr const& terr) {
			auto val = get_terr_colony_value(terr, center);
			if (val > best_val) {
				best_val = val;
				best_terr = &terr;
			}
		});
		
		return env.get_coords(*best_terr);	
	}

	bool SimpleAi::step() {
		
		sync();
		
		if (env.in_progress() and env.get_current_nation() == nation) 
		{
			print("simple_ai: moving units\n");
			
		
			print("simple_ai: ready\n");
			env.ready(nation);
		}
		
		return true;
	}

}
