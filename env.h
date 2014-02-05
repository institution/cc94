#ifndef ENV_H
#define ENV_H

#include "col.h"
#include "objs.h"
#include "csv.h"

namespace col{

	
	
	
	using Tiles = boost::multi_array<uint8, 2>;
	
	// 0 - key not set/unknown
	using IconKey = uint32;
	using PlayerKey = uint32;
	
	using Icons = map<IconKey, Icon>;
	using Players = map<PlayerKey, Player>;
		
	using IconTypes = map<IconType::Key, IconType>;

	IconTypes load_itypes();

	
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
		
		IconTypes its;
		
		Env() {
			mod = 0;
			
			its = load_itypes();
			
			
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
		
		void set_owner(const IconKey &icon_id, const PlayerKey &player_id) {
			auto ii = icons.find(icon_id);
			auto pp = players.find(player_id);
			if (ii != icons.end() && pp != players.end()) {
				Icon &icon = (*ii).second;
				icon.player_id = player_id;
				++mod;
			}
		}
		
		void add_player(const Player &t) {
			//assert(icon.id > 0);
			players[t.id] = t;
			++mod;
		}
		
		Player& get_player(const PlayerKey &id) {
			return players[id];			
		}
		
		const Player& get_player(const PlayerKey &id) const {
			auto p = players.find(id);
			if (p != players.end()) {
				return (*p).second;
			}
			else {
				throw std::runtime_error(str(format("no player with id=%||") % id));
			}
		}
		
		void del_player(const PlayerKey &id) {
			players.erase(id);
			++mod;
		}
		
		PlayerKey next_key_player() {
			return players.size();
		}
		
		void move(const IconKey &id, int8 dx, int8 dy) {
			auto p = icons.find(id);
			if (p != icons.end()) {
				Icon &icon = (*p).second;
				icon.x += dx;
				icon.y += dy;
				++mod;
			}
		}
		
		
		
		
		

	};
	


}

#endif