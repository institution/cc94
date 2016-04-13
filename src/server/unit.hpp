#pragma once

#include "objs.hpp"
#include "storage.hpp"
#include "makeable.hpp"
#include "prof.hpp"

namespace col{

	// colonist   spec type, spec level
	// frigate


	struct Workplace;

	struct UnitType: Makeable {
		using Id = Makeable::Id;

		// data
		string name{""};
		Id id;
		uint8 speed{0};  // flat tiles per 1t (TIME_UNIT)
		uint8 attack{0}; // attack strength
		uint8 combat{0}; // defense strength
		uint8 slots{0};
		uint8 size{0};
		uint8 icon{0};
		uint8 travel{0}; // travel flags LAND | SEA
		Id base{0};
		Item item1{ItemNone};
		Amount num1{0};
		Item item2{ItemNone};
		Amount num2{0};
		Amount cost{0};

		// setters
		UnitType& set_speed(uint8 const& s) { speed = s; return *this; }
		UnitType& set_attack(uint8 const& a) { attack = a; return *this; }
		UnitType& set_combat(uint8 const& c) { combat = c; return *this; }
		UnitType& set_slots(uint8 const& c) { slots = c; return *this; }
		UnitType& set_size(uint8 const& c) { size = c; return *this; }
		UnitType& set_icon(uint8 const& c) { icon = c; return *this; }
		UnitType& set_travel(uint8 const& t) { travel = t; return *this; }
		UnitType& set_base(int const& t) { base = t; return *this; }
		UnitType& set_equip1(Item const& it, Amount num) { item1 = it; num1 = num; return *this; }
		UnitType& set_equip2(Item const& it, Amount num) { item2 = it; num2 = num; return *this; }
		UnitType& set_cost(Amount num) { cost = num; return *this; }

		// getters
		string const& get_name() const { return name; }
		Id get_id() const { return id; }
		uint8 const& get_speed() const { return speed; }
		float get_attack() const { return attack; }
		float get_combat() const { return combat; }
		uint8 const& get_slots() const { return slots; }
		uint8 const& get_size() const { return size; }
		uint8 const& get_icon() const { return icon; }
		uint8 const& get_travel() const { return travel; }
		Id const& get_base() const { return base; }
		Item const& get_item2() const { return item2; }
		Amount const& get_num1() const { return num1; }
		Item const& get_item1() const { return item1; }
		Amount const& get_num2() const { return num2; }
		Amount get_cost() const { return cost; }

		// methods
		Amount get_space() const { return Amount(slots) * 100; }
		Class get_class() const { return Class::UnitType; }

		// constructors
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
			cost = stoi0(xs[14]);
		}

	}; // UnitType

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

	

	struct Unit{
		using Id = uint32;

		Id id{0};
		UnitType const* type{nullptr};
		Nation * nation{nullptr};
		uint8 time_left{TIME_UNIT};
		bool transported{0};
		
		Storage store;

		// where am I
		Terr *terr{nullptr};
		Workplace *workplace{nullptr};
		
		// system - not part of unit, false means unit non-existant
		bool in_game{true};

		Order::type order{Order::Unknown};   // TODO: remove

		Amount space_left{0}; // [t]
		uint8 extend{0}; // num of boarded units TODO: need this?
		
		Amount health{0};
		
		Prof prof_dir{ProfNone};
		int8_t prof_num{0};

		Prof get_prof() const {
			return (prof_num >= 8) ? prof_dir : ProfNone;
		}

		void set_prof(Prof p) {
			prof_dir = p;
			prof_num = 8;			
		}

		void learn(Prof p, int8_t num) {
			if (prof_dir == p) {
				prof_num += num;
			}
			else {
				prof_dir = p;
				prof_num = num;
			}
		}
		
		Amount get(Item item) const {
			return store.get(item);
		}

		void set(Item item, Amount num) {
			space_left += store.get(item);
			space_left -= num;
			store.set(item, num);
		}

		static Amount used_space(Amount num) {
			return 20 * Amount((num + 19) / 20);
		}

		void add(Item item, Amount num) {
			set(item, get(item) + num);
		}

		void sub(Item item, Amount num) {
			set(item, get(item) - num);
		}



		Unit(
			Id const& id
		):
			id(id)
		{}

		Unit(
			Id const& id,
			UnitType const& type,
			Nation & nation
		):
			id(id),
			type(&type),
			nation(&nation),
			space_left(type.get_space())
		{}

		Unit() = default;

		Unit& operator=(Unit const&) = default;

		Unit(Unit && w) {
			*this = w;

			assert(w.terr == nullptr);
			assert(w.workplace == nullptr);
			w.type = nullptr;
			w.nation = nullptr;
		}

		Unit(Unit const&) = delete;

		~Unit() {
			assert(terr == nullptr);
			assert(workplace == nullptr);
			assert(type == nullptr);
			assert(nation == nullptr);
		}

		//bool is_expert(Item const& item) const { return false; }

		Nation & get_nation() const { return *nation; }
		Unit & set_nation(Nation & nation) { this->nation = &nation; return *this; }

		UnitType const& get_type() const { return *type; }
		UnitType::Id const& get_type_id() const { return type->id; }
		string const& get_name() const { return type->get_name(); }
		uint8 const& get_travel() const { return type->get_travel(); }
		float get_speed() const { return type->get_speed(); }
		float get_slots() const { return type->get_slots(); }

		Amount get_space() const { return type->get_space(); }

		Amount get_space_left() const { return space_left; }


		float get_size() const { return type->get_size(); }
		float get_attack() const { return type->get_attack(); }
		float get_combat() const { return type->get_combat(); }
		float get_extend() const { return extend; }
		bool get_transported() const { return transported; }
		uint8 const& get_time_left() const { return time_left; }
		Order::type const& get_order() const { return order; }
		int get_icon() const { return type->icon; }

		Id const& get_base() const { return type->base; }
		Item const& get_item2() const { return type->item2; }
		Amount const& get_num1() const { return type->num1; }
		Item const& get_item1() const { return type->item1; }
		Amount const& get_num2() const { return type->num2; }

		Unit& set_terr(Terr & t) { terr = &t; return *this; }
		Unit& set_type(UnitType const& t) {
			store.clear();
			space_left = t.get_space();
			type = &t;
			return *this;
		}
		
		Amount get_prod(Item const& item) const { 
			// if expert in item return 6
			
			if (item < ItemNoneCount) return 0; 				
			if (item < ItemFieldCount) return 1;			
			if (item < ItemBuildCount) return 3;
			
			throw Error("unknown item");			
		}

		bool is_working() const {
			return workplace != nullptr;
		}


		Workplace & get_workplace() {
			return *(this->workplace);
		}
		
		Workplace const& get_workplace() const {
			return *(this->workplace);
		}


		// uint8 spec_id;
		// int8 spec_lvl;

	};



	ostream& operator<<(ostream &out, Unit const& obj);


}


