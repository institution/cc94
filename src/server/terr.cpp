#include "item.hpp"
#include "terr.hpp"
#include "item.hpp"

namespace col{



	
	Travel Terr::get_travel() const {
		if (alt == AltSea) {
			return TravelSea;
		}
		else {
			if (colony != nullptr) {
				return TravelSea | TravelLand;
			}
			else {
				return TravelLand;
			}
		}
		fail("ERROR: Terr::get_travel: Unknown travel type\n");		
	}
	
	



}
