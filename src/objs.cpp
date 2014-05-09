#include "objs.h"
#include <type_traits>

namespace col {


	ostream& operator<<(ostream &out, const Color &color) {
		out << "Color("<<color.r<<','<<color.g<<","<<color.b<<")";
	}



}