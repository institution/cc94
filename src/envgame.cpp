#include "envgame.h"

namespace col {
	
	
	
	void copy(EnvGame &trg, EnvGame const& src)
	{
		assert(0);
		trg.uts = src.uts;
		
		std::stringstream buf;
		//io::write<EnvGame>(buf, src);		
		//io::read<EnvGame>(trg, buf);		
	}
	
	
}