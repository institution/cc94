#ifndef ENV_H
#define ENV_H

#include "col.h"
#include "objs.h"
#include "csv.h"

namespace col{
	using Tile = uint8;
	using Tiles = boost::multi_array<Tile, 2>;
	
	// 0 - key not set/unknown
	using IconKey = uint32;
	using PlayerKey = uint32;
	
	using Icons = map<Icon::Id, Icon>;
	using Players = map<Player::Id, Player>;
		
	using UnitTypes = map<UnitType::Id, UnitType>;

	UnitTypes load_unit_types();

	
	template <typename C>
	struct RangeXY {
		using baseiter = typename C::const_iterator;
		
		struct Iter{
			baseiter p;
			
			Iter(baseiter p_): p(p_) {
			}
			
			bool operator!= (const Iter& other) {
				return p != other.p;
			}

			
			Iter& operator++() {
				++p;
				return *this;
			}

			const typename C::mapped_type & operator*() {
				 return (*p).second;
			}
		};
		
		baseiter p_begin;
		baseiter p_end;
		Coord x, y;
		
		RangeXY(const C &c, Coord x, Coord y) {
			this->p_begin = c.cbegin();
			this->p_end = c.cend();				
			this->x = x;
			this->y = y;
		}
		
		Iter begin() const	{
			return Iter(p_begin);
		}

		Iter end() const {
			return Iter(p_end);
		}

	};
	
	struct Env{
		Coord w, h;
		
		Tiles terr;
		Icons icons;

		// uint cur_x, cur_y;  

		
		uint32 mod;

		Players players;   // ls of player id

		Player* curr_player;		
		uint32 turn_no;
		
		UnitTypes uts;
		
		Env() {
			mod = 0;			
			uts = load_unit_types();
		}
		
		
		void turn() {
			auto p = players.find(curr_player->id + 1);
			if (p != players.end()) {
				curr_player = &(*p).second;
			}
			else {
				curr_player = &players.at(0);
				++turn_no;
			}			
			++mod;
		}
		
		
		Icon::Id create_icon(UnitType::Id type_id, Player::Id player_id, Coord x, Coord y) {
			Icon::Id id = icons.size();
			icons[id] = Icon(
				id,
				uts.at(type_id),
				x,y,
				players.at(player_id)
			);
			++mod;
			return id;		
		}
		
		void destroy_icon(Icon::Id id) {
			icons.erase(id);
			++mod;
		}
		
		
		void move(Icon::Id id, int8 dx, int8 dy) {
			auto &icon = icons.at(id);
			icon.x += dx;
			icon.y += dy;
			++mod;
		}
		
		void attack(const IconKey &id, int8 dx, int8 dy) {
			auto &c = icons.at(id);			
			auto &d = get_icon_at(c.x+dx, c.y+dy);
			
			auto attack = c.type->attack;
			auto combat = d.type->combat;
			
			assert(attack > 0);
			
			auto r = roll(0, attack + combat);
			cout << "combat " << r << endl;
			if (r <= attack) {
				destroy_icon(d.id);
				c.x += dx;
				c.y += dy;
			}
			else {
				destroy_icon(c.id);
			}
			++mod;
		}
		

		
		
		
		
		Icon& get_icon_at(Coord x, Coord y) {
			for (auto &p: icons) {				
				auto &c = p.second;
				if (c.x == x && c.y == y) {
					return c;
				}
			}
			throw runtime_error("no icon at location");
		}
		
		RangeXY<Icons> ats(Coord x, Coord y) const {
			return RangeXY<Icons>(icons, x, y);			
		}
		
		
		
		
		
		
		//Icon::Id create_icon(istream &f) {
		//	Icon icon(read_obj<Icon>(*this, f));
		//	env.icons[icon.id] = icon;
		//	return icon.id;
		//}
				
				
		Icon& get(Icon::Id id) {
			return icons.at(id);
		}
		
		
		void set_owner(const IconKey &icon_id, const PlayerKey &player_id) {
			icons.at(icon_id).player = &players.at(player_id);
			++mod;			
		}
		
		void add_player(const Player &t) {
			//assert(icon.id > 0);
			cout << "Adding player: " << t.id << endl;
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
		
		
		
		

	};
	

	
	namespace io {
		template <typename C>
		typename C::mapped_type* read_ptr(C &cs, istream &f) {
			return &cs.at(read<typename C::key_type>(f));
		}

		template <typename T>
		T read_obj(Env &env, istream &f);

		template <typename T>
		size_t write_obj(ostream &f, T const &t);
	}
	

}

#endif