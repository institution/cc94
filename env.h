#ifndef ENV_H
#define ENV_H

#include "col.h"
#include "objs.h"
#include "csv.h"

namespace col{
	using Terr = uint8;
	using Terrs = boost::multi_array<Tile, 2>;
	
	using Icons = map<Icon::Id, Icon>;
	using Players = map<Player::Id, Player>;
		
	using UnitTypes = map<UnitType::Id, UnitType>;

	using TerrTypes = map<TerrType::Id, TerrType>;
	
			
	TerrTypes load_terr_types();
	UnitTypes load_unit_types();

	using Coords = aga2::Mv1<Coord>;
	
	
	using IconsByLoc = map<Coords, IconsLst>;
	/*
	template <typename Cnt>
	struct Filtered {
		using baseiter = typename Cnt::const_iterator;
		using func_type = function<bool(Icon const&)>;
		
		Cnt const* cnt;
		func_type fun;
		
		struct Iter{
			baseiter p;
			func_type fun;
			
			Iter(baseiter p_, func_type fun) {
				//p(p_)
				this->fun = fun;
			}
			
			
			bool operator!= (const Iter& other) {
				return p != other.p;
			}
			
			Iter& operator++() {
				++p;
				return *this;
			}

			const typename Cnt::mapped_type & operator*() {
				 return (*p).second;
			}
		};
			
		Filtered(const Cnt &cnt, func_type fun) {
			this->cnt = &cnt;
			this->fun = fun;
		}
		
		Iter begin() const {
			return Iter(cnt->cbegin(), fun);
		}

		Iter end() const {
			return Iter(cnt->cend(), fun);
		}
		
	};
	*/
	
	/* Dir code is yx in mod 3-1 (2 -> -1)
	00 0  -1,-1
	01 1  -1,0 
	02 2  -1,1
	10 3  0,-1
	11 4  0,0
	12 5  0,1
	20 6  1,-1
	21 7  1,0
	22 8  1,1
	*/

	enum class Dir: int8{
		Q=0, W=1, E=2,
		A=3,      D=5,
		Z=6, X=7, C=8
	};
	
	V2<int8> vec4dir(Dir const& d) {
		auto n = static_cast<int8>(d);
		return V2<int8>((n % 3) - 1, (n / 3) - 1);
	}
	
	namespace action {
		struct Move {
			Icon::Id icon_id;
			Dir dir;
		};		
		
		struct Attack {
			Icon::Id icon_id;
			Dir dir;
		};
				
		struct Turn {

		};
	}
	
	struct Env{
		// terrain layer - const
		// game layer - forest, plow, building, unit, resource, unit
		
		Coord w, h;
		
		Terrs terr;  // static terr_id
		
		//IconsAt      // by loc
		Icons icons;  // by id

		// uint cur_x, cur_y;  

		
		uint32 mod;

		Players players;   // ls of player id

		Player* curr_player;		
		uint32 turn_no;
		
		UnitTypes uts; // static
		TerrTypes tts;
		
		Env() {
			mod = 0;			
			uts = load_unit_types();
		}
		
		
		void exec(action::Turn const& a) {
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
		
		TerrType& get_tt(Coord x, Coord y) {
			return terr_types.at(get_terr(c[0], c[1]));
		}
		
		uint8 get_move_type(Coords const& c) {
			auto tt = terr[c.y][c.x];
			tt
			return 1;
			
		}
		
		void exec(action::Move const& a) {
		
			auto &icon = icons.at(a.icon_id);
			auto v = vec4dir(a.dir);
			
			auto &tt = get_tt(icon.x + v[0], icon.y + v[1]);
			
			auto &ut = *(icon.type);
			
			if (!(ut.movement_type & tt.movement_type)) {
				throw runtime_error("cant move - noncompatible type");
			}
				
			if (ut.movement < tt.movement_cost + icon.movement_used)) {
				throw runtime_error("cant move - not enough moves");
			}
			
			icon.movement_used += tt.movement_cost;
			icon.x += v[0];
			icon.y += v[1];
			++mod;
			
		}
		
		void attack(const IconKey &id, int8 dx, int8 dy) {
			auto &c = icons.at(id);			
			auto &d = get_icon_at(c.x+dx, c.y+dy);
			
			auto &tt = get_tt(c.x + dx, c.y + dy);
			
			auto attack = c.type->attack;
			auto combat_base = d.type->combat;
			auto combat = combat_base + uint8(0.25 * tt.defensive * combat_base);
			
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
		
		//RangeXY<Icons> all(Coord x, Coord y) const {
		//	return RangeXY<Icons>(icons, x, y);			
		//}
		
		
		
		
		
		
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