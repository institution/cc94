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
		
		Id const id;
		
		vector<Unit*> units;
		vector<Build*> builds;

		string name;
		
		Storage storage;
		
		void add(Cargo const& c) {
			auto key = enum_value(c.item);
			if (storage.count(key)) {
				storage[key] += c.amount; 
			}
			else {
				storage.insert({key, c.amount});
			}
		}

		void sub(Cargo const& c) {
			auto key = enum_value(c.item);
			if (get(c.item) <= c.amount) {
				storage[key] -= c.amount; 
			}
			else {
				throw Error("out of item");
			}
		}
		
		uint16 get(Item const& item) {
			auto key = enum_value(item);
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
		
		
		
		Colony(Id const& id, string const& name): 
			Place(), Placeable(), 
			id(id), 
			name(name) 
		{}
		
		
		
	};
}

#endif
