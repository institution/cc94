#include "objs.h"
#include <type_traits>

namespace col {

	
	namespace io {

		template <>
		void read<Color>(Color &color, istream &f) {
			color.r = read<uint8>(f);
			color.g = read<uint8>(f);
			color.b = read<uint8>(f);
		}

		template <>
		size_t write<Color>(ostream &f, const Color &color) {
			size_t l = 0;
			l += write<uint8>(f, color.r);
			l += write<uint8>(f, color.g);
			l += write<uint8>(f, color.b);
			return l;
		}
	}
	
	
	ostream& operator<<(ostream &out, const Color &color) {
		out << "Color("<<color.r<<','<<color.g<<","<<color.b<<")";
	}
	


}