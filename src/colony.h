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
		vector<buildp> builds;

		string name;

		Colony(): Placeable(), id(-1) {}
		
		
		
		Colony(Id const& id, string const& name): Placeable(), id(id), name(name) {
			
		}

		Colony(Colony &&c): id(c.id), name(c.name), units(move(c.units)), builds(move(c.builds)) {
			// place?
		}
		
	private:
		Colony(Colony const& c): id(c.id) {
			// place?
		}
		
			
	};
}

#endif
