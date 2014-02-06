#include "objs.h"
#include <type_traits>

namespace col {
	

	ostream& operator<<(ostream &out, const Icon &t) {
		out << "Icon(id="<<t.id<<")";
		//out << (format(
		//	"Icon(id=%||, type_id=%||, x=%||, y=%||, player_id=%||)"
		//) % t.id % t.type->id % t.x % t.y % t.player->id);
	}

	
	namespace io {

		template <>
		Color read<Color>(istream &f) {
			Color color;
			color.r = read<uint8>(f);
			color.g = read<uint8>(f);
			color.b = read<uint8>(f);
			return color;
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
		cout << "Color("<<color.r<<','<<color.g<<","<<color.b<<")";
	}
	

	ostream& operator<<(ostream &out, const Player &obj) {
		cout << "Player(" << "id"<<"="<<obj.id<<','<<"name"<<"="<<obj.name << ")";
	}

}