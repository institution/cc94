#include "objs.hpp"
#include <type_traits>

namespace col {


	ostream& operator<<(ostream &out, NationColor const& color) {
		print(out, "NColor(%||,%||,%||)", color.r, color.g, color.b);		
		return out;
	}



}
