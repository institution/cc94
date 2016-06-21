#pragma once

namespace col {


	struct Marks {
	
		struct Mark {
			v2c pos;
			string label;
			Mark() = default;
			Mark(v2c pos, string const& label): pos(pos), label(label) {}			
		};
	
		std::vector<Mark> marks;
		
		std::vector<Mark> const& list() const {return marks;}
		std::vector<Mark> & list() {return marks;}
		
		void clear() { marks.clear(); }
		void add(v2c pos, string const& m) { marks.push_back(Mark(pos, m)); }
		
		
	};
	
	
}
