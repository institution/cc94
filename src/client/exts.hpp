#pragma once

namespace col {

	
	template <class T, class TE>
	struct Exts {
		using Id = typename T::Id;
	
		using Xs = unordered_map<Id, TE>;
		Xs xs;
			
		TE & get(Id id) {
			auto p = xs.find(id);
			if (p != xs.end()) {
				return p->second;
			}
			
			auto r = xs.emplace(piecewise_construct,
				forward_as_tuple(id),
				forward_as_tuple(id)
			);
			
			assert(r.second == true);
			
			//return r.first.second;
				
			return xs.at(id);			
		}
		
		TE & get(T const& u) {
			return get(u.id);
		}
		
		auto begin() { return xs.begin(); }		
		auto end() { return xs.end(); }
		auto begin() const { return xs.begin(); }		
		auto end() const { return xs.end(); }
		
		using iterator = typename Xs::iterator;
		using const_iterator = typename Xs::const_iterator;
		
	};

}
