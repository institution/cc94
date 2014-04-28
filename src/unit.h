#ifndef UNIT2_H
#define UNIT2_H

#include "objs.h"

namespace col{

	// colonist   spec type, spec level
	// frigate
	
	
	
	
	struct UnitType {
		using Id = uint32;
		
		string name;
		Id id; // const
		uint8 speed;  // flat tiles per 1t (TIME_UNIT)
		uint8 attack; // attack strength
		uint8 combat; // defense strength
		uint8 cargo;
		uint8 size;
		uint8 travel; // travel flags LAND | SEA
		
		UnitType& set_travel(uint8 const& t) { travel = t; return *this; }
		UnitType& set_speed(uint8 const& s) { speed = s; return *this; }
		UnitType& set_attack(uint8 const& a) { attack = a; return *this; }
		UnitType& set_combat(uint8 const& c) { combat = c; return *this; }
		
		string const& get_name() const { return name; }
		uint8 const& get_travel() const { return travel; }
		uint8 const& get_speed() const { return speed; }
		uint8 const& get_attack() const { return attack; }
		uint8 const& get_combat() const { return combat; }
		
		
		UnitType() {}
		
		explicit UnitType(Id const& id): id(id), name("unnamed") {}
		
		explicit UnitType(vector<string> const& xs) {
			assert(xs.size() >= 8);
			
			name = trim_copy(xs[0]);
			id = stoi(xs[1]);
			
			set_speed(stoi(xs[2]));
			set_attack(stoi(xs[3]));
			set_combat(stoi(xs[4]));
			
			//cargo = stoi(xs[5]);
			//size = stoi(xs[6]);
			// unused column 7 
			// unused column 8
			set_travel(stoi(xs[9]));
		}
				
	};
	
	struct Unit: Place, Placeable {
		using Id = uint32;
		
		Id id;
		UnitType const* type;
		Player* player;
		uint8 time_left;
		Workplace const* workplace;
		Item workitem;
		uint16 free_space;
		
		string const& get_name() const { return type->get_name(); }
		uint8 const& get_travel() const { return type->get_travel(); }
		uint8 const& get_speed() const { return type->get_speed(); }
		uint8 const& get_attack() const { return type->get_attack(); }
		uint8 const& get_combat() const { return type->get_combat(); }		
		UnitType const& get_type() const { return *type; }
		UnitType::Id const& get_type_id() const { return type->id; }
		Player & get_player() const { return *player; }
		// uint8 spec_id;
		// int8 spec_lvl;
		
		Unit& set_work(Workplace const& place, Item const& item) {
			workplace = &place;
			workitem = item;
			return *this;
		}
		
		Unit& clear_work() {
			workplace = nullptr;
			workitem = Item::None;
			return *this;
		}
		
		PlaceType place_type() {
			return PlaceType::Unit;
		}
				
		
		
		Unit():
			Place(), 
			Placeable(), 
			id(-1), 
			type(nullptr), 
			player(nullptr), 
			time_left(0),
			workplace(nullptr),
			workitem(Item::None),
			free_space(0)
		{}
		
		Unit(
			Id const& id, 
			UnitType const& type,
			Player & player
		): 
			Place(), 
			Placeable(), 
			id(id), 
			type(&type), 
			player(&player), 
			time_left(6),
			workplace(nullptr),
			workitem(Item::None),
			free_space(0)
		{}
		
		Unit(Unit &&) = default;
		Unit(Unit const&) = delete;
		
	}; 

	ostream& operator<<(ostream &out, Unit const& obj);

	
}

#endif
