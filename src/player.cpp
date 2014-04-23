#include "player.h"

namespace col{
	
	ostream& operator<<(ostream &out, const Player &obj) {
		out << format("Player(id=%||,name=%||)") % obj.id % obj.name;
		
		//out << "Player(" << "id"<<"="<<obj.id<<','<<"name"<<"="<<obj.name << ")";
	}
	
	
}