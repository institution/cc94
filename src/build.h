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
	
	struct Build: Place {
		
		BuildType const* type;
		//int8 free_slots;
		Place *place;
		
		Build(BuildType const& type): type(&type) {	
			//this->free_slots = type.slots;
		}
		
		PlaceType place_type() {
			return PlaceType::Build;
		}
		
				
	}; 
	
	using buildp = unique_ptr<Build>;


	ostream& operator<<(ostream &out, Build const& obj);

	
}

#endif
