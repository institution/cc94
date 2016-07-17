#include "control.hpp"

namespace col{
	
	string get_control_name(Control cc) 
	{ 
		switch (cc) {
			case ControlEnglandCol: return "England";
			case ControlFranceCol: return "France";
			case ControlSpainCol: return "Spain";
			case ControlNetherlandsCol: return "Netherlands";
		}
		return "NoName";
	}
	
	uint8_t get_control_flagicon(Control cc) 
	{
		switch (cc) {
			case ControlEnglandCol: return 119;
			case ControlFranceCol: return 120;
			case ControlSpainCol: return 121;
			case ControlNetherlandsCol: return 122;
		}
		return 118;  // no idea what that is
	}


	


	ostream& operator<<(ostream & o, Control cc) {
		o << format("Control(%|| %||)", cc, get_control_name(cc));		
		return o;
	}


	
	
}
