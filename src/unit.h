#ifndef UNIT2_H
#define UNIT2_H

#include "objs.h"
#include "error.h"

namespace col{

	// colonist   spec type, spec level
	// frigate




	struct UnitType {
		using Id = uint32;

		string name{""};
		Id id;
		uint8 speed{0};  // flat tiles per 1t (TIME_UNIT)
		uint8 attack{0}; // attack strength
		uint8 combat{0}; // defense strength
		uint8 slots{0};
		uint8 size{0};
		uint8 icon{0};
		uint8 travel{0}; // travel flags LAND | SEA

		UnitType& set_speed(uint8 const& s) { speed = s; return *this; }
		UnitType& set_attack(uint8 const& a) { attack = a; return *this; }
		UnitType& set_combat(uint8 const& c) { combat = c; return *this; }
		UnitType& set_slots(uint8 const& c) { slots = c; return *this; }
		UnitType& set_size(uint8 const& c) { size = c; return *this; }
		UnitType& set_icon(uint8 const& c) { icon = c; return *this; }
		UnitType& set_travel(uint8 const& t) { travel = t; return *this; }

		string const& get_name() const { return name; }
		uint8 const& get_speed() const { return speed; }
		float get_attack() const { return attack; }
		float get_combat() const { return combat; }
		uint8 const& get_slots() const { return slots; }
		uint8 const& get_size() const { return size; }
		uint8 const& get_icon() const { return icon; }
		uint8 const& get_travel() const { return travel; }

		UnitType() {}

		explicit UnitType(Id const& id): id(id) {}

		explicit UnitType(vector<string> const& xs) {
			assert(xs.size() >= 9);

			name = trim_copy(xs[0]);
			id = stoi(xs[1]);
			speed = stoi0(xs[2]);
			attack = stoi0(xs[3]);
			combat = stoi0(xs[4]);
			slots = stoi0(xs[5]);
			size = stoi0(xs[6]);
			icon = stoi0(xs[7]);
			travel = stoi0(xs[8]);
		}

	};


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

		Id id{0};
		UnitType const* type{nullptr};
		Player* player{nullptr};
		uint8 time_left{0};
		bool transported{0};






		Order::type order{Order::Unknown};

		Workplace * workplace{nullptr};
		Item workitem{ItemNone};

		uint16 space_left{0}; // [t]
		uint8 extend{0}; // num of boarded units

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
			space_left(type.slots*100)
		{}

		Unit() = default;
		Unit(Unit &&) = default;
		Unit(Unit const&) = delete;

		Player & get_player() const { return *player; }
		UnitType const& get_type() const { return *type; }
		UnitType::Id const& get_type_id() const { return type->id; }
		string const& get_name() const { return type->get_name(); }
		uint8 const& get_travel() const { return type->get_travel(); }
		float get_speed() const { return type->get_speed(); }
		float get_slots() const { return type->get_slots(); }
		float get_space_left() const { return space_left; }
		float get_size() const { return type->get_size(); }
		float get_attack() const { return type->get_attack(); }
		float get_combat() const { return type->get_combat(); }
		float get_extend() const { return extend; }
		uint8 const& get_time_left() const { return time_left; }
		Order::type const& get_order() const { return order; }
		Workplace const& get_workplace() const { return *workplace; }
		Item const& get_workitem() const { return workitem; }


		bool assigned() {
			return workplace != nullptr;
		}
		// uint8 spec_id;
		// int8 spec_lvl;

		Unit & set_workitem(Item const& workitem) { this->workitem = workitem; return *this; }

		Unit& set_work(Workplace & place, Item const& item) {
			if (assigned()) {
				if (!workplace->leave()) {
					throw Critical("should leave");
				}
			}

			if (!place.assign()) {
				throw Critical("should assign");
			}

			workplace = &place;
			workitem = item;
			return *this;
		}

		Unit& set_work() {
			if (assigned()) {
				if (!workplace->leave()) {
					throw Critical("should leave");
				}
			}
			// clear work
			workplace = nullptr;
			workitem = ItemNone;
			return *this;
		}
	};

	ostream& operator<<(ostream &out, Unit const& obj);


}

#endif
