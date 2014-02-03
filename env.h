#ifndef ENV_H
#define ENV_H

#include "col.h"
#include "objs.h"

namespace col{

	
	
	
	using Tiles = boost::multi_array<uint8, 2>;
	
	// 0 - key not set/unknown
	using IconKey = uint32;
	using PlayerKey = uint32;
	
	using Icons = map<IconKey, Icon>;
	using Players = map<PlayerKey, Player>;
		
	
	struct Env{


		uint w, h;
		
		Tiles terr;
		Icons icons;

		// uint cur_x, cur_y;  

		uint turn;
		
		uint32 mod;

		Players players;   // ls of player id

		uint16 curr_player;		
		uint32 turn_no;
		
		Env() {
			mod = 0;
		}
		
		IconKey next_key() {
			return icons.size() + 1;
		}
		
		void add(const Icon &icon) {
			//assert(icon.id > 0);
			icons[icon.id] = icon;
			++mod;
		}
		
		Icon& get(const IconKey &id) {
			return icons[id];			
		}
		
		void del(const IconKey &id) {
			icons.erase(id);
			++mod;
		}
		
		
		void add_player(const Player &t) {
			//assert(icon.id > 0);
			players[t.id] = t;
			++mod;
		}
		
		Player& get_player(const PlayerKey &id) {
			return players[id];			
		}
		
		void del_player(const PlayerKey &id) {
			players.erase(id);
			++mod;
		}
		
		void move(const IconKey &id, int8 dx, int8 dy) {
			auto p = icons.find(id);
			if (p != icons.end()) {
				(*p).second.x += dx;
				(*p).second.y += dy;
				++mod;
			}
		}
		
		
		
		
		

	};
	


}

#endif