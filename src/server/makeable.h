#ifndef MAKEABLE_H_3248956349
#define MAKEABLE_H_3248956349

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
	
	
		
}

#endif
