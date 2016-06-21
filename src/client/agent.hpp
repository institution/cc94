#pragma once

#include "env.hpp"


namespace col{

	
	/// 'Agent is not necessarily player, for ex: during map editing
	struct Agent 
	{
		// minimal
		virtual Class get_class() const { return ClassNone; }
		virtual bool step() = 0;
		virtual ~Agent() {}
		
		
		
		// visualisation
		virtual string get_name() const { return "-"; }		
		//virtual Cmds const& get_unit_cmds(Unit::Id id);
		//virtual char get_unit_letter(Unit::Id id) { return '?'; }
		
	};
	
	
	
	
	
	
	
}




