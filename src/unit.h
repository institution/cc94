#ifndef UNIT2_H
#define UNIT2_H

#include "objs.h"

namespace col{

	// colonist   spec type, spec level
	// frigate
	
	
	
	struct UnitType {
		using Id = uint32;
		
		string name;
		Id id;
		uint8 movement;
		uint8 attack;
		uint8 combat;
		uint8 cargo;
		uint8 size;
		uint8 movement_type;

		UnitType() {}
		
		UnitType(vector<string> const& xs) {
			assert(xs.size() >= 8);
			name = trim_copy(xs[0]);
			id = stoi(xs[1]);
			movement = stoi(xs[2]) * UNIT_OF_MOVEMENT;
			attack = stoi(xs[3]);
			combat = stoi(xs[4]);
			cargo = stoi(xs[5]);
			size = stoi(xs[6]);
			// unused column 7 
			// unused column 8
			movement_type = stoi(xs[9]);			
		}
				
	};
	
	
	struct Unit: Place {
		using Id = uint32;
		
		Id id;
		UnitType const* type;
		Place *place;
		Coords pos;
		Player const* player;	
		uint8 movement_used;  // movement used in this turn
				
		// uint8 spec_id;
		// int8 spec_lvl;
		
		string name;
		
		uint16 free_space;
		

		Unit(): movement_used(0) {
			name = "";
		}
		
		Unit(
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
		
		void produce() {
			
		}
		
		void turn() {
			
			movement_used = 0;
			
			/*if (supply != -1) {
				if (supply) {
					supply -= 1;
				}
				else {				
					hp -= 1;				
				}
			}*/
						
		}
		
		MovementType get_movement_type() {
			return static_cast<MovementType>(type->movement_type);
		}

	}; 

	ostream& operator<<(ostream &out, Unit const& obj);

	
}

#endif
