#ifndef ENV_H
#define ENV_H

#include "col.h"
#include "objs.h"

namespace col{

	
	
	
	using Tiles = boost::multi_array<uint8, 2>;
	
	// 0 - key not set/unknown
	using IconKey = uint32;
	
	using Icons = map<IconKey, Icon>;
	
	struct Env{


		uint w, h;
		
		Tiles terr;
		Icons icons;

		// uint cur_x, cur_y;  

		uint turn;

		vector<Player> players;   // ls of player id

		uint16 curr_player;		
		uint32 turn_no;
		
		IconKey next_key() {
			return icons.size() + 1;
		}
		
		void add(const Icon &icon) {
			assert(icon.id > 0);
			icons[icon.id] = icon;
		}
		
		Icon& get(const IconKey &id) {
			return icons[id];
		}
		
		void del(const IconKey &id) {
			icons.erase(id);
		}
		

	};
	
	Env read_env(istream &f);
	
	void write_env(ostream &f, const Env &env);


}

#endif