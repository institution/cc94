#include "runner.hpp"

#include "col.hpp"
#include "env.hpp"
#include "console.hpp"
#include "client/serialize2.hpp"
#include "../ext/format.hpp"
#include "serialize3.hpp"
#include "layout.hpp"



namespace col{

	





	void Runner::run(std::string const& main_rc, int verbose) 
	{

		// add master agent
		this->add_agent<col::Console>(env, this, main_rc);

		// wait for agents
		threads.at(0).join();
		f_stop = true;		
	}
	
	
	
	
	
}
