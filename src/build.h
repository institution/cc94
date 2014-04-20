#ifndef BUILD2_H
#define BUILD2_H

#include "objs.h"

namespace col{

	// colonist   spec type, spec level
	// frigate
	
	
	struct BuildType{
		using Id = uint8;
		
		string name;
		Id id;
		
		BuildType() {}
		
		BuildType(vector<string> const& xs) {
			assert(xs.size() >= 2);
			name = trim_copy(xs[0]);
			id = stoi(xs[1]);
		}
				
	};
	
	struct Build: Workplace {
		
		BuildType const* type;
		int8 free_slots;
		
		
		Build(BuildType const& type): 
			type(&type), 
			free_slots(3) 
		{}
		
		// Workplace
		
		uint16 get_yield(Item const& item, bool const& is_expert) const {
			return 3;
		}
		
		bool assign() {
			if (!free_slots) {
				return false;
			}
			free_slots -= 1;
			return true;
		}
			
		bool leave() {
			if (free_slots < 3) {
				return false;
			}
			free_slots += 1;
			return true;
		}
				
	}; 
	
	using buildp = unique_ptr<Build>;


	ostream& operator<<(ostream &out, Build const& obj);

	
}

#endif
