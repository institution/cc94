#ifndef COLONY_H
#define COLONY_H

#include "base.h"
#include "unit.h"
#include "build.h"

namespace col {
	
	struct Colony: Place, Placeable {
		
		using Id = uint32;
		
		Id const id;
		
		vector<Unit*> units;
		vector<Build*> builds;

		string name;

		PlaceType place_type() {
			return PlaceType::Colony;
		}
		
		string const& get_name() const {
			return name;
		}
		
		Colony(Id const& id, string const& name): 
			Place(), Placeable(), 
			id(id), 
			name(name) 
		{}
			
	};
}

#endif
