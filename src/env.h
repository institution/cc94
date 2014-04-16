#ifndef ENV_H
#define ENV_H

#include "col.h"
#include "base.h"
#include "objs.h"
#include "csv.h"
#include "terr.h"
#include "build.h"
#include "unit.h"


namespace col{
	
	
	
	using TerrTypes = map<TerrType::Id, TerrType>;
	using BuildTypes = map<BuildType::Id, BuildType>;
	using UnitTypes = map<UnitType::Id, UnitType>;
	
	using Terrs = boost::multi_array<Terr, 2>;	
	using Builds = map<Build::Id, Build>;
	using Units = map<Unit::Id, Unit>;
		
	using Colonies = map<Colony::Id, Colony>;
	using Players = map<Player::Id, Player>;
		
	
	template<class Types>
	Types load(string const& fn) {
		using Type = typename Types::mapped_type;
		Types ts;

		auto ls = read_conf(fn);
		auto p = ls.begin();
		auto e = ls.end();
		
		++p; // skip header
		while (p != e) {
			// cout << "LOAD TERR TYPES: " << (*p).size() << endl;
			if ((*p).size() > 1) {
				auto t = Type(*p);
				ts[t.id] = t;

				//for (auto &v: *p) {
				//	cout << v << '|';
				//}
				//cout << endl;
			}			
			++p;
		}

		return ts;
	}
	
	//using UnitsByLoc = map<Coords, UnitsLst>;
	/*
	template <typename Cnt>
	struct Filtered {
		using baseiter = typename Cnt::const_iterator;
		using func_type = function<bool(Unit const&)>;
		
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

	
	
	struct Env{
		// terrain layer - const
		// game layer - forest, plow, building, unit, resource, unit
		
		// static
		Coord w, h;
		
		TerrTypes const* tts;
		BuildTypes const* bts; 
		UnitTypes const* uts; 
		
		
		// modifiable
		Terrs terrs;  
		Builds builds;		
		Units units;  
		

		uint32 mod;

		Colonies colonies;
		Players players;   // ls of player id

		uint32 turn_no;
		
		Env() {
			mod = 0;
			turn_no = 0;
			w = h = 0;
		}
			
		
		
		
		void turn() {
			// time progress
			++turn_no;
			
			// all units - new movment
			for (auto& item: units) {
				item.second.turn();
			}
			
			
			
			
			++mod;
		}
		
		void end_turn(Player::Id const& player_id) {
			// player declares no more actions
		}
		
		/*void switch_next_player() {
			
			if (curr_player == nullptr) {
				throw runtime_error("curr_player is nullish: cannot end turn");
			}
			
			auto p = players.find(curr_player->id + 1);
			if (p != players.end()) {
				curr_player = &(*p).second;
			}
			else {
				curr_player = &players.at(0);
				turn();
			}	

			++mod;
		}*/
		
		
		
		// terr - unit
		void move_in(Terr &loc, Unit &obj) {
			loc.units.push_back(&obj);		
			obj.place = &loc;
		}
		void move_out(Terr &loc, Unit &obj) {
			auto& us = loc.units;			
			us.erase(remove(us.begin(), us.end(), &obj), us.end());
			obj.place = nullptr;
		}
		
		// terr - colony
		void move_in(Terr &loc, Colony &obj) {
			assert(loc.colony == nullptr);
			loc.colony = &obj;
			obj.place = &loc;
		}
		void move_out(Terr &loc, Colony &obj) {
			assert(loc.colony == &obj);
			loc.colony = nullptr;
			obj.place = nullptr;
		}
		
		
		
		/*
		void move_in(Terr const& loc, Build const& obj) {
			 obj.place = &loc;
		}		
		void move_in(Unit const& loc, Unit const& obj) {
			 obj.place = &loc;
		}
		void move_in(Unit const& loc, Cargo const& obj)	{			 {
			 obj.place = &loc;
		}
		void move_in(Build const& loc, Unit const& obj) {
			//if (loc.get_free_space() >= obj.get_size()) {
			//	loc.sub_free_space(obj.get_size());
				obj.place = &loc;
			//	return NOT_ENOUGH_SPACE;
			//}
			//else {
			//	return OK;
			//}
		}
		void move_in(Build const& loc, Cargo const& obj) {
			 obj.place = &loc;
		}
		
		
		void move_out(Unit const& obj, Terr const& loc) {
			obj.place = nullptr;
		}
		void move_out(Build const& obj, Terr const& loc) {
			obj.place = nullptr;
		}
		
		void move_out(Unit const& obj, UNit const& loc) {
			
			obj.place = nullptr;
		}
		void move_out(Cargo const& obj, Unit const& loc) {
			obj.place = nullptr;
		}
		
		void move_out(Unit const& obj, Build const& loc) {
			obj.place = nullptr;
		}
		void move_out(Cargo const& obj, Build const& loc) {
			obj.place = nullptr;
		}
		*/
		
		
		Unit::Id create_icon(UnitType::Id type_id, Player::Id player_id, Coords xy) {
			Unit::Id id = units.size();
			
			if (uts->find(type_id) == uts->end()) {
				throw runtime_error(str(format("unit types: id not found: %||") % type_id));
			}
						
			units[id] = Unit(
				id,
				uts->at(type_id),
				xy,
				players.at(player_id)
			);
			++mod;
			return id;		
		}
		
		void destroy_icon(Unit::Id id) {
			units.erase(id);
			++mod;
		}
				
		Terr& terr(Vector2<Coord> const& z) {
			return terrs[z[1]][z[0]];
		}
		
		Terr const& terr(Vector2<Coord> const& z) const {
			return terrs[z[1]][z[0]];
		}
		
		
		//TerrId& terr_id(Coord x, Coord y) {
		//	return terrs[y][x];
		//}
		
		//TerrType& get_terr_type(Coords const& xy) {
		//	return tts.at(get_terr_id(xy[0], xy[1]));
		//}
		
		Unit const* get_icon_at(Coords const& xy) const {
			for (auto &p: units) {				
				auto &c = p.second;
				if (c.pos == xy) {
					return &c;
				}
			}
			return nullptr;			
		}
		
		
		
		Unit& icon4id(Unit::Id const& id) {
			return units.at(id);			
		}
		
		
		Unit* get_icon_at(Coords const& xy) {
			for (auto &p: units) {				
				auto &c = p.second;
				if (c.pos == xy) {
					return &c;
				}
			}
			return nullptr;
			
		}
		
		Unit& ref_icon_at(Coords const& pos) {
			auto p = get_icon_at(pos);
			if (p) {
				return *p;
			}
			throw runtime_error("no icon at location");
		}
		
		
	/*
		void produce(Unit const& unit, Item const& item, Dir const& dir) {
			
			
		}
		
		
		void construct(Unit const& unit) {
	
		}
		
		void move(Unit const& unit, Build const& build) {
			
		}
			*/

		
		bool colonize(Unit &u, string const& name) {
			
			// decrease unit time_left
			if (u.time_left < 6) {
				return 0;
			}
			u.time_left -= 6;			
			
			// unit terrain square
			assert(u.place);
			assert(typeid(u.place) == typeid(Terr));
			
			auto& t = *static_cast<Terr*>(u.place);
						
			// next colony uid
			Colony::Id id = colonies.size();
			
			// create colony
			//auto& cc = colonies.emplace(id, Colony(id, name)).second;
			
			colonies.emplace(id, Colony(id, name));
			
			// colony at terrain square
			//move_in(t, c);
			
			//move_in(cc, );
			
			// move unit into colony
			// TODO
			
			return 1;
		}
		
		bool can_attack_move(Unit const& u, Dir const& dir) {
			// square empty
			auto& ut = *(u.type);
			auto dest = u.pos + vec4dir(dir);
			auto tt = terr(dest);
			
			if (!(ut.movement_type & tt.get_movement_type())) {
				cout << "cannot move - noncompatible type" << endl;
				return false;
				// throw runtime_error("cant move - noncompatible type");
			}
			// compatible terrain type
				
			/*
			moves_left = u.time_left * ut.movement;
			
			moves_need = tt.get_movement_cost(ut.movement_type)
			
			moves_left < moves_need
				u.time_left -= moves_need / ut.movement
			*/
			
			if (ut.movement < tt.get_movement_cost(ut.movement_type) + u.movement_used) {
				cout << "cannot move - not enough moves" << endl;
				return false;
				// throw runtime_error("cant move - not enough moves");
			}
			// enough movement
			
			auto x = get_icon_at(dest);
			if (x != nullptr && x->player == u.player) {
				cout << "cannot move: square already occupied" << endl;
				return false;				
				//throw std::runtime_error("cannot move: square already occupied");
			}
			// dest square empty or occupied by enemy
			
			return true;			
		}
		
		
		
		void attack_move(Player::Id const& pid, Unit::Id const& iid, Dir const& dir) {
			Unit *p = & icon4id(iid);
			
			if (!can_attack_move(*p, dir)) {
				cout << "!!!!!!!!" << endl;
				throw std::runtime_error("cannot attack move");			
			}
			
			auto dest = p->pos + vec4dir(dir);
			auto q = get_icon_at(dest);
			auto tt = terr(dest);
			
			if (q == nullptr) {
				// move
				
				p->movement_used += tt.get_movement_cost(p->get_movement_type());
				p->pos = dest;
			}
			else {
				//attack
				auto attack_base = p->type->attack;
				auto attack = attack_base;
				auto combat_base = q->type->combat;
				auto combat = combat_base + uint8(0.25 * tt.get_defensive_value(q->get_movement_type()) * combat_base);

				assert(attack > 0);

				auto r = roll(0, attack + combat);

				// cout << "combat " << r << endl;

				if (r <= attack) {
					p->movement_used = p->type->movement;
					p->pos = dest;

					destroy_icon(q->id);
				}
				else {
					destroy_icon(p->id);
				}

			}
			
			++mod;			
		}
		
		
		//RangeXY<Units> all(Coord x, Coord y) const {
		//	return RangeXY<Units>(units, x, y);			
		//}
		
		
		
		
		
		
		//Unit::Id create_icon(istream &f) {
		//	Unit icon(read_obj<Unit>(*this, f));
		//	env.units[icon.id] = icon;
		//	return icon.id;
		//}
				
				
		Unit& get(Unit::Id id) {
			return units.at(id);
		}
		
		
		void set_owner(const Unit::Id &icon_id, const Player::Id &player_id) {
			units.at(icon_id).player = &players.at(player_id);
			++mod;			
		}
		
		void add_player(const Player &t) {
			//assert(icon.id > 0);
			cout << "Adding player: " << t.id << endl;
			players[t.id] = t;
			++mod;
		}
		
		Player& get_player(const Player::Id &id) {
			return players[id];			
		}
		
		const Player& get_player(const Player::Id &id) const {
			auto p = players.find(id);
			if (p != players.end()) {
				return (*p).second;
			}
			else {
				throw std::runtime_error(str(format("no player with id=%||") % id));
			}
		}
		
		void del_player(const Player::Id &id) {
			players.erase(id);
			++mod;
		}
		
		Player::Id next_key_player() {
			return players.size();
		}
		
		
		
		

	};
	

	
	namespace io {
		template <typename C>
		typename C::mapped_type const* read_ptr(C const& cs, istream &f) {
			auto key = read<typename C::key_type>(f);
			return &cs.at(key);
		}
		
		template <typename C>
		typename C::mapped_type* read_ptr(C &cs, istream &f) {
			auto key = read<typename C::key_type>(f);
			return &cs.at(key);
		}

		template <typename T>
		T read_obj(Env &env, istream &f);

		template <typename T>
		size_t write_obj(ostream &f, T const &t);
	}
	

}

#endif