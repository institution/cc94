#include "field.h"


namespace col {
	
	Amount Field::get_prod(Item const& item, bool const& is_expert) const {
		assert(terr != nullptr);
		return terr->get_yield(item, is_expert);
	};
	
	bool operator==(Field const& self, Field const& other) {
		return self.terr == other.terr;
	}

	
	
}
