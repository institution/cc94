#include "unit.hpp"

namespace col{
	
	ostream& operator<<(ostream &out, Unit const& t) {
		out << format("Unit(id=%||,type=%||)", t.id, uint32(t.type->id));
		return out;
	}
	


}
