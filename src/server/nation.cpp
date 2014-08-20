#include "nation.h"

namespace col{
	
	ostream& operator<<(ostream &out, const Nation &obj) {
		out << format("Nation(id=%||,name=%||)", obj.id, obj.name);
		
		//out << "Nation(" << "id"<<"="<<obj.id<<','<<"name"<<"="<<obj.name << ")";
		return out;
	}
	
	
}