#include "field.hpp"
#include "env.hpp"

namespace col {
	
	Amount Field::get_prod(Env const& env, Unit const& unit, Item const& item) const {
		assert(terr != nullptr);
		return env.get_prod(*terr, unit, item);		
	};
	
	bool operator==(Field const& self, Field const& other) {
		return self.terr == other.terr;
	}

	
	
}
