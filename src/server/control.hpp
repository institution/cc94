#pragma once

#include "objs.hpp"
#include "meta.hpp"

namespace col {


	
	
	using Control = uint8_t;
	Control const ControlNone = 255;
	Control const 
		ControlEnglandCol = 0,
		ControlEnglandMet = 1,
		ControlFranceCol = 2,
		ControlFranceMet = 3,
		ControlSpainCol = 4,
		ControlSpainMet = 5,
		ControlNetherlandsCol = 6,
		ControlNetherlandsMet = 7,
		ControlArwak = 8,
		ControlAztec = 9,
		ControlInca = 10,
		ControlTupi = 11,
		ControlCherokee = 12,
		ControlIroquois = 13,
		ControlSioux = 14,
		ControlApache = 15;
	

	string get_control_name(Control cc);	
	uint8_t get_control_flagicon(Control cc);
	
	ostream& operator<<(ostream & o, Control cc);


}
