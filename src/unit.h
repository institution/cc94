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
		float get_attack() const { return attack; }
		float get_combat() const { return combat; }


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

/*
	struct WorkplaceIndex{
		unsigned int slot:3;
		unsigned int place:5;

		WorkplaceIndex() {
			// 15 -- stockade
			set_default();
		}

		bool is_field() {
			return 16 <= slot;
		}

		void set_field(Dir::type const& dir) {
			place = dir + 16;
		}

		void set_default() {
			place = 15;
			slot = 0;
		}


		Dir::type get_field() {
			assert(is_field());
			return place - 16;
		}

		uint8 get_slot() {
			return slot;
		}

		void set_slot(uint8 s) {
			slot = s;
		}

		uint8 get_build() {
			return slot;
		}

		void set_build(uint8 build) {
			slot = build;
		}
	};*/

	/*
	 * 0 - b0s0
	 * 1 - b0s1
	 * 2 - b0s2
	 * 4 - b0s3
	 *
	 *
	* 1-15 -- building
	* 16-24 -- field 0-8 (Dirs)
	*/

	struct Unit: Placeable {
		using Id = uint32;

		Id id;
		UnitType const* type;
		Player* player;
		uint8 time_left;

		Order::type order;
		Workplace * workplace;
		Item workitem;
		uint slot;

		uint16 free_space;

		string const& get_name() const { return type->get_name(); }
		uint8 const& get_travel() const { return type->get_travel(); }
		uint8 const& get_speed() const { return type->get_speed(); }
		float get_attack() const { return type->get_attack(); }
		float get_combat() const { return type->get_combat(); }
		UnitType const& get_type() const { return *type; }
		UnitType::Id const& get_type_id() const { return type->id; }
		Player & get_player() const { return *player; }
		// uint8 spec_id;
		// int8 spec_lvl;

		Unit& set_work(Workplace & place, Item const& item) {
			workplace = &place;
			workitem = item;
			return *this;
		}

		Unit& set_work() {
			// clear work
			workplace = nullptr;
			workitem = ItemNone;
			return *this;
		}



		Unit():
			Placeable(),
			id(-1),
			type(nullptr),
			player(nullptr),
			time_left(0),
			workplace(nullptr),
			order(Order::Unknown),
			workitem(ItemNone),
			free_space(0)
		{}

		Unit(
			Id const& id,
			UnitType const& type,
			Player & player
		):
			Placeable(),
			id(id),
			type(&type),
			player(&player),
			time_left(6),
			workplace(nullptr),
			order(Order::Unknown),
			workitem(ItemNone),
			free_space(0)
		{}

		Unit(Unit &&) = default;
		Unit(Unit const&) = delete;

	};

	ostream& operator<<(ostream &out, Unit const& obj);


}

#endif
