#pragma once

#include "base.hpp"
#include "../server/dir.hpp"
#include "command.hpp"


namespace col {


	template <class T, class U>
	void print_table(ostream & o, darray2::darray2<T,U> const& a) 
	{	
		
		for (U j = 0; j < a.dim[1]; ++j) {
			for (U i = 0; i < a.dim[0]; ++i)
			{
				auto x = a({i,j});
				int c;
				if (x == 0) {
					o << "  x" << ' ';
				}
				else if (x > 1000000) {
					o << "  ." << ' ';
				}
				else {
					o << std::setw(3) << int(x) << ' ';
				}
			}
			o << "\n";
		}
	}

	//std::function<float(v2c,v2c)>

	/// get_move_cost(trg,src) -> float
	
	struct PathRes {
		Cmds cmds;
		float cost{0.0f};
		bool exist() { return not std::isinf(cost); }
		bool exists() { return not std::isinf(cost); }
	};
	
	template <class F>
	PathRes find_path(Env const& env, v2c src, v2c trg, F get_move_cost) 
	{
		using Cost = float;
	
		//Cost const Neutral = -1;
		Cost const Infinity = std::numeric_limits<Cost>::infinity();
		
		ext::darray2<Cost, Coord> tmp(env.get_dim(), Infinity);
		
		std::unordered_set<v2c, MyHash<v2c>> ws0;
		std::unordered_set<v2c, MyHash<v2c>> ws1;
		auto * ps0 = &ws0;
		auto * ps1 = &ws1;
		
		bool ret = false;
		int8_t i = 0;
		
		
		// from src -> trg, max path length 100 tiles
		print("find path [%|| -> %||]\n", src, trg);
		
		ps0->insert(src);
		tmp(src) = 0.0f;
		
		
		while (not ret and i < 100) {				
			for (auto w0: *ps0) {
				for (auto d: Neighs) {
					auto w1 = w0 + dir2vec(d);						
					if (env.in_bounds(w1)) 
					{						
						auto cost = get_move_cost(w1, w0);
						
						auto new_cost = tmp(w0) + cost;
						
						if (new_cost < tmp(w1)) {
							tmp(w1) = new_cost;
							ps1->insert(w1);
							
							if (w1 == trg) {
								ret = true;
							}
						}				
					}
				}					
			}				
			std::swap(ps0, ps1);
			ps1->clear();
			++i;
		}
		
		print("find path 2: \n");
		print_table(std::cout, tmp);
		print("\n");
		
		PathRes res;
		
		
		if (ret == true) {
			
			auto p0 = trg;
			while (p0 != src) {				
				v2c best_p = p0;
				Dir best_d = DirS;
				Cost best_cost = Infinity;
				for (auto d: Neighs) {						
					auto p1 = p0 + dir2vec(d);
					if (env.in_bounds(p1)) {
						auto cost = tmp(p1);
						if (cost < best_cost) {
							best_cost = cost;
							best_p = p1;
							best_d = d;
						}
					}
				}
				p0 = best_p;
				
				res.cost += best_cost;
				res.cmds.radd(Cmd(InsMove, turnabout(best_d)));
			}				
		}
		else {
			res.cost = Infinity;
		}
					
		return res;
	}

	
	
	template <class T, class F>
	T find_nearest_dist(v2<T> p, F f) 
	{
		T r = 6;
		auto x0 = p[0] - r;
		auto y0 = p[1] - r;
		auto x1 = p[0] + r;
		auto y1 = p[1] + r;
		
		T min_dist = r + 1;
		for (auto j = y0; j <= y1; ++j)
		{
			for (auto i = x0; i <= x1; ++i)
			{
				if (f(p)) 
				{
					auto dist = std::max(std::abs(i - p[0]), std::abs(j - p[0]));
					if (dist < min_dist) 
					{
						min_dist = dist;
					}
				}
			}
			
		}
		
		return min_dist;
	}
	
}
