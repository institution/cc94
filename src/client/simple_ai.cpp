#include "simple_ai.hpp"


namespace simple_ai{

	
	/*struct Map{
		darray2<TerrInfo> map;
		
	};*/
	
	
	
	
	
	
	void SimpleAi::sync() {
		
		// list my units
		// Unit * u eng.get_units(nation_auth);
		
		
		env.for_unit(nation, [](Unit const& u){
			
		});
		
		
	}
	
	

	bool SimpleAi::step() {
		
		/*env.for_discovered_terr(nation, [](Terr const& t){
			
		});*/
		
		
		env.for_unit(nation, [](Unit const& u){		
			
		});
		
		
		
		
		if (env.in_progress()) {
			if (env.get_current_nation() == nation) {
				print("simple_ai: ready\n");
				env.ready(nation);
			}
		}
		
		return true;
	}
		
}
