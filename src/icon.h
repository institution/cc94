#ifndef ICON2_H
#define ICON2_H

#include "objs.h"

namespace col{

	struct Icon {
		using Id = uint32;
		
		Id id;
		UnitType const* type;
		Coords pos;
		Player const* player;	
		uint8 movement_used;  // movement used in this turn
		
		
				
		//uint8 spec_id;

		Icon(): movement_used(0) {}
		
		Icon(
			Id id, 
			UnitType const &type, 
			Coords pos, 
			Player const &player
		) {	
			this->id = id; 
			this->type = &type; 
			this->pos = pos;
			this->movement_used = 0;
			this->player = &player;
		}
		
		void turn() {
			movement_used = 0;
		}
		
		MovementType get_movement_type() {
			return static_cast<MovementType>(type->movement_type);
		}

	}; 

	ostream& operator<<(ostream &out, const Icon &obj);

	
}

#endif
