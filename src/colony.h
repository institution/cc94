#ifndef COLONY_H
#define COLONY_H

#include "base.h"
#include "unit.h"
#include "build.h"

namespace col {
	
	struct Colony: Placeable {
		
		using Id = uint32;
		
		Id const id;
		
		vector<Unit*> units;
		vector<Build*> builds;

		string name;

		Colony(): Placeable(), id(-1) {}
		
		
		
		Colony(Id const& id, string const& name): Placeable(), id(id), name(name) {
			
		}

	private:
		Colony(Colony const& c): id(c.id) {
			assert(0);
		}
	};
}

#endif
