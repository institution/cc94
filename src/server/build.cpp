#include "build.hpp"
#include "env.hpp"

namespace col{
	
	ostream& operator<<(ostream &out, Build const& t) {
		out << format("Build(type=%||)", uint32(t.type->id));
		return out;
	}

	Amount Build::get_prod(Env const& env, Unit const& unit, Item const& item) const {
		return env.get_prod(*this, unit, item);
	}
	
	Amount Build::get_cons(Env const& env, Unit const& unit, Item const& item) const { 
		return env.get_cons(*this, unit, item);
	}
	
}
