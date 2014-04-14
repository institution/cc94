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
		using Id = uint32;
		
		Id id;
		BuildType const* type;
		//int8 free_slots;
		Place *place;
		
		Build() {}
		
		Build(
			Id id, 
			BuildType const& type
		) {	
			this->id = id; 
			this->type = &type;
			//this->free_slots = type.slots;
		}
				
	}; 

	ostream& operator<<(ostream &out, Build const& obj);

	
}

#endif
