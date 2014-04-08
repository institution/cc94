#include "icon.h"

namespace col{
	
	ostream& operator<<(ostream &out, const Icon &t) {
		out << "Icon("<<"id="<<t.id<<','<<"type="<<uint32(t.type->id)<<")";
		//out << (format(
		//	"Icon(id=%||, type_id=%||, x=%||, y=%||, player_id=%||)"
		//) % t.id % t.type->id % t.x % t.y % t.player->id);
	}
	
}