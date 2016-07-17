#pragma once

#include "control.hpp"

namespace col {

	struct Faction {
		using Id = Control;
		Id id;
		string name;
		// TODO: faction traits
		
		Faction() = default;
		Faction(Faction const&) = delete;
		Faction(Id id): id(id), name("") {}
		Faction(Id id, string name): id(id), name(name) {}
	
		string get_name() const { return name; }
		Control get_control() const { return id; }
	
	};
	
}
