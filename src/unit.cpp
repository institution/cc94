#include "unit.h"

namespace col{
	
	ostream& operator<<(ostream &out, Unit const& t) {
		out << "UNit("<<"id="<<t.id<<','<<"type="<<uint32(t.type->id)<<")";
		//out << (format(
		//	"Icon(id=%||, type_id=%||, x=%||, y=%||, player_id=%||)"
		//) % t.id % t.type->id % t.x % t.y % t.player->id);
	}
	
}