#include "field.h"
#include "terr.h"


namespace col{
	
	
	uint16 Field::get_yield(bool const& is_expert) const {
		/* 		 
		    Why this is so complicated; games are supposed to have easy to understand rules
		  ballance should be achieved otherwise (negative feedback possibly)
		 TODO: simplify in AC spirit; yields should be devised from climate parameters; 			
		*/
		auto& item = get_proditem();
		return terr->get_yield(item, is_expert)		;
	}

	
	
	
}
