#include "field.hpp"


namespace col {
	
	Amount Field::get_prod(Item const& item, Amount const& base) const {
		assert(terr != nullptr);
		return terr->get_yield(item, base);
	};
	
	bool operator==(Field const& self, Field const& other) {
		return self.terr == other.terr;
	}

	
	
}
