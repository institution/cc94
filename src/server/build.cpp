#include "build.h"

namespace col{
	
	ostream& operator<<(ostream &out, Build const& t) {
		out << format("Build(type=%||)", uint32(t.type->id));
		return out;
	}
	
}