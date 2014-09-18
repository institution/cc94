#include "serialize.h"


namespace col{
	
	string write_string(Env const& env, Unit const& x) {
		std::ostringstream s;
		boost::archive::binary_oarchive ar(s);
		write(ar, env, x);
		string st = s.str();
		return st;
	}

	void read_string(Env & env, Unit & x, string const& data) {
		std::istringstream s(data);
		boost::archive::binary_iarchive ar(s);
		read(ar, env, x);
	}

	
	
	
}
