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
		int base{0};
		Item item1{ItemNone};
		int num1{0};
		Item item2{ItemNone};
		int num2{0};


		UnitType& set_speed(uint8 const& s) { speed = s; return *this; }
		UnitType& set_attack(uint8 const& a) { attack = a; return *this; }
		UnitType& set_combat(uint8 const& c) { combat = c; return *this; }
		UnitType& set_slots(uint8 const& c) { slots = c; return *this; }
		UnitType& set_size(uint8 const& c) { size = c; return *this; }
		UnitType& set_icon(uint8 const& c) { icon = c; return *this; }
		UnitType& set_travel(uint8 const& t) { travel = t; return *this; }
		UnitType& set_base(int const& t) { base = t; return *this; }
		UnitType& set_equip1(Item const& it, int num) { item1 = it; num1 = num; return *this; }
		UnitType& set_equip2(Item const& it, int num) { item2 = it; num2 = num; return *this; }


		string const& get_name() const { return name; }
		uint8 const& get_speed() const { return speed; }
		float get_attack() const { return attack; }
		float get_combat() const { return combat; }
		uint8 const& get_slots() const { return slots; }
		uint8 const& get_size() const { return size; }
		uint8 const& get_icon() const { return icon; }
		uint8 const& get_travel() const { return travel; }
		int const& get_base() const { return base; }
		Item const& get_item2() const { return item2; }
		int const& get_num1() const { return num1; }
		Item const& get_item1() const { return item1; }
		int const& get_num2() const { return num2; }

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
			base = stoi0(xs[9]);
			item1 = stoi0(xs[10]);
			num1 = stoi0(xs[11]);
			item2 = stoi0(xs[12]);
			num2 = stoi0(xs[13]);
		}

	};

	inline
	bool operator==(UnitType const& t1, UnitType const& t2) {
		return t1.id == t2.id;
	}


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


	struct Terr;
	struct Build;
	struct Field;

	struct Unit {
		using Id = uint32;

		Id id{0};
		UnitType const* type{nullptr};
		Player * player{nullptr};
		uint8 time_left{0};
		bool transported{0};

		// where am I
		Terr *terr{nullptr};
		Build *build{nullptr};
		Field *field{nullptr};




		Order::type order{Order::Unknown};

		uint16 space_left{0}; // [t]
		uint8 extend{0}; // num of boarded units

		Unit(
			Id const& id,
			UnitType const& type,
			Player & player
		):
			id(id),
			type(&type),
			player(&player),
			time_left(6),
			space_left(type.slots)
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
		int get_icon() const { return type->icon; }

		int const& get_base() const { return type->base; }
		Item const& get_item2() const { return type->item2; }
		int const& get_num1() const { return type->num1; }
		Item const& get_item1() const { return type->item1; }
		int const& get_num2() const { return type->num2; }

		Unit& set_terr(Terr & t) { terr = &t; return *this; }
		Unit& set_type(UnitType const& t) { type = &t; return *this; }

		bool is_working() const {
			return field or build;
		}

		template <typename T>
		T& get_workplace();

/*
		Unit& set_speed(uint8 const& s) { type->speed = s; return *this; }
		Unit& set_attack(uint8 const& a) { type->attack = a; return *this; }
		Unit& set_combat(uint8 const& c) { type->combat = c; return *this; }
		Unit& set_slots(uint8 const& c) { type->slots = c; return *this; }
		Unit& set_size(uint8 const& c) { type->size = c; return *this; }
		Unit& set_icon(uint8 const& c) { type->icon = c; return *this; }
		Unit& set_travel(uint8 const& t) { type->travel = t; return *this; }
*/


		// uint8 spec_id;
		// int8 spec_lvl;

	};

	template <> inline
	Build& Unit::get_workplace<Build>() {
		return *(this->build);
	}

	template <> inline
	Field& Unit::get_workplace<Field>() {
		return *(this->field);
	}

	ostream& operator<<(ostream &out, Unit const& obj);


}



#endif
