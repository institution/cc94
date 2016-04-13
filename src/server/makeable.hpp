#pragma once

#include "col.hpp"


namespace col {
	
	
	struct Makeable {
		using Id = uint32_t;
		
		virtual Amount get_cost() const = 0;
		virtual Class get_class() const = 0;
		virtual ~Makeable() {}
		virtual string const& get_name() const = 0;		
		virtual Id get_id() const = 0;
	};

/*
	

	struct Education: Makeable{
		Item speciality;

		Type type() { return TypeEducation; }
		
	};

			
 
	
		
	
	char const* get_prof_name(Prof p)
		switch (p) {
			"None"
			"Farmer"
			"Fisherman"
			"Sugar Planter"
			"Tobacco Planter"
			"Cotton Planter"
			"Fur Trapper"
			"Lumberjack"
			"Ore Miner"
			"Silver Miner"	
			"Distiller"
			"Tobaconist"
			"Weaver"
			"Fur Trader"
			"Blacksmith"
			"Gunsmith"
			"Soldier"
			"Pioneer"
			"Preacher"
			"Carpenter"
			"Statesman"
			default: return "Unknown";
		}
	}*/
		
}

