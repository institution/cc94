#include "env.hpp"



struct VisEnv {
	// env visible to agent during game

	Env & env;
	
	
	
	
	
	map_dim
	map:
		tile
			visible

	
	
	
	visible/nonvisible(blank)
	
	
	board[][]
		terr
			pos
			biome
			alt
			phys
			colony^ | none
			units[]^
		
	unittypes[] (static)
	buildtypes[] (static)

	units[]
		unit
			type
			terr^
			workfield^ | workbuild^ | none
			store
				slot
					item
					amount
			spec
			
	
	colonies[]	
		colony
			terr^
			name
			store
				slot
					item
					amount
			field[]
				proditem
				
			build[]
				type			
				make_unit_type^ | make_build_type^ | none			
				progress
				units^[]
					
			
			
				
			
		
		
		
	
	
	
	
		

	


};
