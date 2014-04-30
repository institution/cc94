#ifndef COLONY_H
#define COLONY_H

#include "base.h"
#include "unit.h"
#include "build.h"
#include "error.h"

namespace col {
	
	using Storage = map<uint8, uint16>;
	
	struct Colony: Place, Placeable {
		
		using Id = uint32;
		
		Id id;
		string name;
		Storage storage;
		
		// vector<Unit*> units;
		
		array<Build,15> builds;
		
		Build& construct_building(BuildType const& type, int slot) {
			builds.at(slot) = Build(type);
			return builds[slot];
		}		
		
		void add(Cargo const& c) {
			auto key = c.item;
			if (storage.count(key)) {
				storage[key] += c.amount; 
			}
			else {
				storage.insert({key, c.amount});
			}
		}

		void sub(Cargo const& c) {
			auto key = c.item;
			if (get(c.item) <= c.amount) {
				storage[key] -= c.amount; 
			}
			else {
				throw Error("out of item");
			}
		}
		
		uint16 get(Item::type const& item) {
			auto key = item;
			if (storage.count(key)) {
				return storage.at(key);
			}
			else {
				return 0;
			}			
		}
		
		PlaceType place_type() {
			return PlaceType::Colony;
		}
		
		string const& get_name() const {
			return name;
		}
		
		
		
		Colony(Id const& id, string const& name=""): 
			Place(), 
			Placeable(), 
			id(id), 
			name(name),
			builds()
		{}
		
		
		
		Colony() = default;
		Colony(Colony &&) = default;
		
		Colony(Colony const&) = delete;
		
		
	};
}

#endif
