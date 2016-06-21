#include "colony.hpp"

namespace col {
	
	int Colony::get_field_index(Field const& f) const {
		for (size_t i = 0; i < fields.size(); ++i) {
			if (fields.at(i) == f) return i;
		}
		throw Error("no such field");
	}
	
	
	
}
