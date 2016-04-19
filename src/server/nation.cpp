#include "nation.hpp"

namespace col{
	
	ostream& operator<<(ostream &out, const Nation &obj) {
		out << format("Nation(id=%||,name=%||)", int(obj.id), obj.name);
		return out;
	}
	
	
}
