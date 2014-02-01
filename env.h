#ifndef ENV_H
#define ENV_H

#include "col.h"
#include "objs.h"

namespace col{

	using Terrain = boost::multi_array<uint8, 2>;
	
	struct Env{


		uint w, h;
		Terrain terr;
		vector<Icon> icons;

		// uint cur_x, cur_y;  

		uint turn;

		vector<Player> players;   // ls of player id

		uint16 curr_player;		
		uint32 turn_no;

	};
	
	Env read_env(istream &f);
	
	void write_env(ostream &f, const Env &env);


}

#endif