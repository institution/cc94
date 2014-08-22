#ifndef COLONY_H
#define COLONY_H

#include "base.h"
#include "unit.h"
#include "build.h"
#include "error.h"
#include "field.h"

namespace col {

	

	using Storage = map<Item, Amount>;



	template<class T>
	struct cont{ using type = vector<T>; };

	template<>
	struct cont<Build>{ using type = array<Build,15>; };



	struct Colony {

		using Id = uint32;

		Id id;
		string name;
		Storage storage;
		int max_storage{100};

		int get_max_storage() { return max_storage; }

		Terr *terr{nullptr};

		cont<Field>::type fields;

		Colony & add_field(Field const& f) { fields.push_back(f); return *this; }
		void sub_field(Field const& f) { fields.erase(find(fields.begin(), fields.end(), f)); }


		cont<Build>::type builds;

		Build& construct_building(BuildType const& type, int slot) {
			builds.at(slot).ctype = &type;
			return builds.at(slot);
		}

		int get_build_index(Build const& b) const {
			return &b - &builds[0];
		}

		int get_field_index(Field const& f) const {
			for (size_t i = 0; i < fields.size(); ++i) {
				if (fields.at(i) == f) return i;
			}
			throw Error("no such field");
		}


		Build& get_build(int i) {
			return builds.at(i);
		}

		Build const& get_build(int i) const {
			return builds.at(i);
		}

		Colony & set_build(int const& i, Build const& build) { builds.at(i) = build; return *this; }



		void add(Item const& item, Amount num) {
			auto key = item;
			if (storage.count(key)) {
				storage[key] += num;
			}
			else {
				storage.insert({key, num});
			}
		}

		void sub(Item const& item, Amount num) {
			auto key = item;
			if (num <= get(item)) {
				storage[key] -= num;
			}
			else {
				throw Error("out of item");
			}
		}

		void set(Item const& item, Amount num) {
			if (num == 0) {
				storage.erase(item);
			}
			else {
				storage[item] = num;
			}
		}


		Amount get(Item const& item) const {
			auto key = item;
			if (storage.count(key)) {
				return storage.at(key);
			}
			else {
				return 0;
			}
		}

		bool has(Item const& item, Amount num) const {
			return get(item) >= num;
		}

		PlaceType::type place_type() {
			return PlaceType::Colony;
		}

		string const& get_name() const {
			return name;
		}



		Colony(Id const& id, string const& name=""):
			id(id),
			name(name),
			builds()
		{}



		Colony() = default;
		Colony(Colony &&) = default;

		Colony(Colony const&) = delete;

		~Colony() {
			assert(terr == nullptr);
		}



		template <typename T>
		typename cont<T>::type & get_cont();

		template <typename T>
		typename cont<T>::type const& get_cont() const;



	};




	template <>	inline
	typename cont<Field>::type & Colony::get_cont<Field>() {
		return fields;
	}

	template <>	inline
	typename cont<Build>::type & Colony::get_cont<Build>() {
		return builds;
	}

	template <>	inline
	typename cont<Field>::type const& Colony::get_cont<Field>() const {
		return fields;
	}

	template <>	inline
	typename cont<Build>::type const& Colony::get_cont<Build>() const {
		return builds;
	}

}

#endif
