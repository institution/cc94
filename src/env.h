#ifndef ENV_H
#define ENV_H


#include "col.hpp"
#include "base.h"
#include "objs.h"
#include "player.h"
#include "csv.h"
#include "terr.h"
#include "build.h"
#include "unit.h"
#include "roll.h"
#include "error.h"
#include <signal.h>




namespace col{
	using Id = uint32; 
	
	using TerrTypes = unordered_map<TerrType::Id, TerrType>;
	using BuildTypes = unordered_map<BuildType::Id, BuildType>;
	using UnitTypes = unordered_map<UnitType::Id, UnitType>;
	
	using Terrs = boost::multi_array<Terr, 2>;	
	using Units = unordered_map<Unit::Id, Unit>;
	using Builds = unordered_map<Build::Id, Build>;
	using Colonies = unordered_map<Colony::Id, Colony>;	
	using Players = unordered_map<Player::Id, Player>;
		
	
	
	inline bool compatible(Travel const& x, Travel const& y) {
		return x | y;
	}
	
	
	template<typename RET, typename THIS, typename ARG>
	RET & get_defender_impl(THIS & self, ARG & t) {
		if (! t.units.size()) {
			throw Error("no defending unit");
		}
		return *(t.units)[0];
	}

	
	struct Env{
		
		// mod
		shared_ptr<TerrTypes> tts;
		shared_ptr<BuildTypes> bts; 
		shared_ptr<UnitTypes> uts; 
				
		// state		
		Players players;
		Units units;
		Builds builds;
		Colonies colonies;
		Coord w, h;
		Terrs terrs;
		uint32 turn_no;
		uint32 next_id;
		
		// misc
		uint32 mod;
		boost::function<int (int range)> roll;
		
		
		
		Env(): terrs(Coords(0,0), boost::fortran_storage_order()) {
			
			next_id = 0;
			mod = 0;
			turn_no = 0;
			w = h = 0;
			
			units.reserve(10);
			
			
			
			tts = make_shared<TerrTypes>();
			bts = make_shared<BuildTypes>();
			uts = make_shared<UnitTypes>();
			
			set_random_gen(roll::roll1);
		}
		
		Env& set_random_gen(boost::function<int (int range)> const& func) {
			roll = func;
			return *this;
		}
		

		
		template<class Type>
		void loads(string const& fn) {
						
			auto& ts = get_cont<Type>();

			auto ls = read_conf(fn);
			auto p = ls.begin();
			auto e = ls.end();
			
			++p; // skip header
			// load line by line
			while (p != e) {
				auto& xs = *p;
				if (xs.size() > 1) {
					// id must be in second column
					auto id = stoi(xs[1]);
					ts.emplace(piecewise_construct,
						forward_as_tuple(id), 
						forward_as_tuple(xs)
					);
				}			
				++p;
			}

		}
		
			
		template <typename T>
		unordered_map<typename T::Id, T>& get_cont();
		
		template <typename T>
		unordered_map<typename T::Id, T> const& get_cont() const;
				
		
		//template<class T>
		uint32 get_next_id() {
			next_id = next_id + 1;
			return next_id;
		}
		
		
		template<class T, class... A>
		T& create(A&&... args) { 
			uint32 id = get_next_id();
			auto& ts = get_cont<T>();
			auto p = ts.emplace(piecewise_construct,
				forward_as_tuple(id), 
				forward_as_tuple(id, std::forward<A>(args)...)
			).first;
			
			return (*p).second;
		}
		
		template <typename T>
		void destroy(typename T::Id const& id) {
			get_cont<T>().erase(id);			
		}
			
		template <typename T>
		T& get(typename T::Id const& id) {
			return get_cont<T>().at(id);
		}
		
		template <typename T>
		T const& get(typename T::Id const& id) const {
			return get_cont<T>().at(id);
		}
		
		template <typename T>
		bool exist(typename T::Id const& id) {
			return get_cont<T>().count(id);
		}
		
				
		
		
		
		void turn() {
			// time progress
			++turn_no;
			
			// all units - new movment, work
			for (auto& item: units) {
				cerr << "tuer: " << item.second << endl;
				turn(item.second);
			}
			++mod;
		}
		
		Item get_material(Item const& item) {
			switch (item) {
				case Item::Rum: return Item::Sugar;
				case Item::Cigars: return Item::Tobacco;
				case Item::Cloth: return Item::Cotton;
				case Item::Coats: return Item::Furs;
				case Item::Hammers: return Item::Lumber;
				case Item::Tools: return Item::Ore;
				case Item::Muskets: return Item::Tools;				
			}
			return Item::None;
		}
				
		void turn(Unit &u) {
			
			u.time_left = 6;
			
			if (u.workplace != nullptr) {
				auto& t = ref_terr(u);
				auto& c = t.get_colony();
								
				auto const& yield = u.workplace->get_yield(u.workitem, false);
				auto const& needitem = get_material(u.workitem);
				
				uint16 prod;
				if (needitem != Item::None) {
					prod = std::min(yield, c.get(needitem));
					c.sub({needitem, prod});
				}
				else {
					prod = yield;
				}
				
				c.add({u.workitem, prod});
				
				u.time_left = 0;
			}
			
			/*if (supply != -1) {
				if (supply) {
					supply -= 1;
				}
				else {				
					hp -= 1;				
				}
			}*/
						
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
			loc.set_colony(obj);
			obj.place = &loc;
		}
		void move_out(Terr &loc, Colony &obj) {
			assert(loc.colony == &obj);
			loc.colony = nullptr;
			obj.place = nullptr;
		}
		
		// colony - build
		void move_in(Colony &loc, Build &obj) {
			loc.builds.push_back(&obj);
			obj.place = &loc;
		}
		
		void move_out(Colony &loc, Build &obj) {
			auto& xs = loc.builds;
			xs.erase(remove(xs.begin(), xs.end(), &obj), xs.end());
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
		
		bool in_bounds(Coords const& p) const;
		
		template <Dir D>
		bool in_bound(Coords const& p) const;
		
		template <Dir D>
		bool on_bound(Coords const& p) const;
		
		
		Coords get_coords(Terr const& t) const {
			
			// offset = x*w + y
			
			uint32 offset = uint32(&t - &terrs[0][0]);
			return Coords(offset % w, offset / w);
		}
		
		Coords get_coords(Unit const& u) const {
			return get_coords(get_terr(u));
		}
		
		
		Terr & ref_terr(Coords const& z) {
			return terrs(z);
		}
		
		Terr & ref_terr(Placeable const& x) {
			if (x.place->place_type() != PlaceType::Terr) {				
				throw Error("unit not in field");
			}			
			return *static_cast<Terr*>(x.place);
		}
		
		Terr const& get_terr(Coords const& z) const {
			return terrs(z);
		}
		
		Terr const& get_terr(Placeable const& u) const {			
			if (u.place->place_type() != PlaceType::Terr) {				
				throw Error("unit not in field");
			}			
			return *static_cast<Terr const*>(u.place);
		}
				
		Env & set_terr(Coords const& z, Terr const& t) {
			terrs(z) = t;
			return *this;
		}
				
		Env & resize(Coords const& dim) {
			//terrs.resize(boost::extents[dim[1]][dim[0]]);
			w = dim[0]; h = dim[1];
			terrs.resize(Coords(w,h));
			return *this;
		}
		
		Env & fill(Terr const& t) {
			
			for (Coord j = 0; j < h; ++j) {
				for (Coord i = 0; i < w; ++i) {
					auto& x = ref_terr(Coords(i,j));
					x.biome = t.biome;
					x.phys = t.phys;
				}
			}
			
			return *this;
		}
		
		
		Unit& icon4id(Unit::Id const& id) {
			return units.at(id);			
		}
		
		
		
	/*
		void produce(Unit const& unit, Item const& item, Dir const& dir) {
			
			
		}
		
		
		void construct(Unit const& unit) {
	
		}
		
		void move(Unit const& unit, Build const& build) {
			
		}
			*/

		bool check(int L, int M) {
			int r = roll(M); // 0, M-1
			cerr << str(format("check %||/%|| -> %||\n") % L % M % (r+1));
			return r < L;
			
		}
		
		bool run_map_task(Unit &u, uint8 const& time_cost) {
			/* 
			 */
			
			auto time_left = u.time_left;
			if (time_left < time_cost) {
				u.time_left = 0;
				return check(time_left, time_cost);
			}
			else {
				cerr << "check no need" << endl;
				u.time_left -= time_cost;
				return 1;
			}
		}
		
		Code build_road(Unit &u) {
			/* Build road on land terrain square (cost ~ 1.5 turns)
			 */
			
			// unit checks
			if (u.place->place_type() != PlaceType::Terr) {				
				return -1; // unit not on terrain
			}
			if (!compatible(u.get_travel(), LAND)) {
				return -2; // must be land based unit
			}
						
			Terr& t = *static_cast<Terr*>(u.place);
			// terrain checks
			if (!compatible(t.get_travel(), LAND)) {
				return -3; // can build on land only
			}
			if (t.has(Phys::Road)) {
				return -4; // road already exists
			}
			
			// execute
			if (run_map_task(u, TIME_UNIT * 2)) {
				t.add(Phys::Road); 
			}			
			return OK;
		}
		
		
		
		bool order_move(Unit &u, Dir const& dir) {
			/* Move unit
			 */
			
			if (dir == Dir::S) {
				throw Error("already here");
			}
			
			// get terr
			auto& orig = ref_terr(u);
			auto& dest = ref_terr(get_coords(orig) + vec4dir(dir));
			
			if (!compatible(u.get_travel(), dest.get_travel())) {
				throw Error("cannot travel through this terrain");
			}
			
			// cost to move = 1t
			auto time_cost = TIME_UNIT;
						
			// execute
			if (run_map_task(u, time_cost)) {
				move_out(orig, u);
				move_in(dest, u);
				return 1; // success
			}
			else {
				return 0; // try next turn
			}
			
		}
		
		
		bool has_defender(Coords const& pos) const {
			return get_terr(pos).has_units();
		}
		
		
		
		Unit const& get_defender(Terr const& t) const {
			return get_defender_impl<Unit const&>(*this, t);			
		}
		
		Unit & get_defender(Terr const& t) {
			return get_defender_impl<Unit &>(*this, t);			
		}
		
		//Unit & get_defender(Terr const& t) {
		//	return const_cast<Unit &>( get_defender(t) );
		//}
		
		
		
		Unit const* get_defender_if_any(Terr const& t) const {
			if (!t.units.size()) {
				return nullptr;
			}
			return t.units[0];
		}
		
		
		bool order_attack(Unit & attacker, Dir const& dir) {
			/* Order attack
			 */
			
			if (dir == Dir::S) {
				throw Error("cannot attack self");
			}
			
			// get terr
			auto& orig = ref_terr(attacker);
			auto& dest = ref_terr(get_coords(orig) + vec4dir(dir));
			
			if (!compatible(attacker.get_travel(), dest.get_travel())) {
				throw Error("this unit cannot attack accross that terrain");
			}
			
			if (attacker.get_attack() == 0) {
				throw Error("this unit has no offensive capability");
			}
			
			auto& defender = get_defender(dest);
			
			// cost to approach enemy and fight = 1t
			auto time_cost = TIME_UNIT;
						
			// execute
			if (run_map_task(attacker, time_cost)) {
				// fight
				auto attack_base = attacker.get_attack();
				auto attack = attack_base;
				auto combat_base = defender.get_combat();
				auto combat = combat_base;

				if (check(attack, attack + combat)) {
					move_out(dest, defender);
					destroy<Unit>(defender.id);
				}
				else {
					move_out(orig, attacker);
					destroy<Unit>(attacker.id);
				}
				
				return 1; // command executed
			}
			else {
				return 0; // try next turn
			}
			
		}
		
		
		bool plow_field(Unit& u) {
			
			// unit check
			if (!compatible(u.get_travel(), LAND)) {
				throw Error("need land based unit to perform this action");				
			}
			
			auto& t = ref_terr(u);
			
			// terrain checks
			if (!compatible(t.get_travel(), LAND)) {
				throw Error("can build on land only");
			}
			
			if (t.has(Phys::Plow)) {
				throw Error("improvment already exists");
			}
			
			// execute
			if (run_map_task(u, TIME_UNIT * 2)) {
				t.add(Phys::Plow); 
			}			
			
			return OK;			
		}
		
		bool clear_forest(Unit const& u) {
			uint8 time_cost = TIME_UNIT * 3;  // 3t
			
			
		}
		
		
		bool plant_forest() {
			
		}
		
		
		bool colonize(Unit &u, string const& name) {
			
			auto& t = ref_terr(u);
			
			auto time_cost = TIME_UNIT;
			
			if (run_map_task(u, time_cost)) {
				// create colony
				auto& c = create<Colony>(name);
				move_in(t, c);
				
				// move unit into colony?
				// TODO
						
				return 1;  // ok
			}
			else {
				return 0;  // try next turn
			}
		}
		
		
		bool can_attack_move(Unit const& u, Dir const& dir) {
			// square empty
			//auto& ut = *(u.type);
			//auto dest = u.pos + vec4dir(dir);
			//auto tt = terr(dest);
			
			//if (!(ut.movement_type & tt.get_movement_type())) {
			//	cout << "cannot move - noncompatible type" << endl;
			//	return false;
				// throw runtime_error("cant move - noncompatible type");
			//}
			// compatible terrain type
				
			/*
			moves_left = u.time_left * ut.movement;
			
			moves_need = tt.get_movement_cost(ut.movement_type)
			
			moves_left < moves_need
				u.time_left -= moves_need / ut.movement
			*/
			
			//if (ut.movement < tt.get_movement_cost(ut.movement_type) + u.movement_used) {
			//	cout << "cannot move - not enough moves" << endl;
			//	return false;
				// throw runtime_error("cant move - not enough moves");
			//}
			// enough movement
			/*
			auto x = get_icon_at(dest);
			if (x != nullptr && x->player == u.player) {
				cout << "cannot move: square already occupied" << endl;
				return false;				
				//throw std::runtime_error("cannot move: square already occupied");
			}*/
			// dest square empty or occupied by enemy
			
			return true;			
		}
		
		
		
		void attack_move(Player::Id const& pid, Unit::Id const& iid, Dir const& dir) {
			
			/*
			Unit *p = & icon4id(iid);
			
			if (!can_attack_move(*p, dir)) {
				cout << "!!!!!!!!" << endl;
				throw std::runtime_error("cannot attack move");			
			}
			
			auto dest = p->pos + vec4dir(dir);
			auto q = get_icon_at(dest);
			auto tt = get_terr(dest);
			
			if (q == nullptr) {
				// move
				
				//p->movement_used += tt.get_movement_cost(p->get_movement_type());
				p->pos = dest;
			}
			else {
				//attack
				auto attack_base = p->type->attack;
				auto attack = attack_base;
				auto combat_base = q->type->combat;
				//auto combat = combat_base + uint8(0.25 * tt.get_defensive_value(q->get_movement_type()) * combat_base);

				assert(attack > 0);

				//auto r = roll(attack + combat);

				// cout << "combat " << r << endl;
				
				if (r <= attack) {
					//p->movement_used = p->type->movement;
					p->pos = dest;

					destroy<Unit>(q->id);
				}
				else {
					destroy<Unit>(q->id);
				}
			

			}
			*/
				
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
				
		
		void set_owner(const Unit::Id &icon_id, const Player::Id &player_id) {
			units.at(icon_id).player = &players.at(player_id);
			++mod;			
		}
				
		Player& get_player(Player::Id const& id) {
			return players.at(id);
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
		
		
		template<class Archive>
		void save(Archive & ar, uint const& version) const;
		
		template<class Archive>
		void load(Archive & ar, uint const& version);
		
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		

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
	
	
	
	

	template <> inline
	Terr & Env::get<Terr>(Terr::Id const& id) {
		return ref_terr(id);
	}

	template <> inline
	Terr const& Env::get(Terr::Id const& id) const {
		return get_terr(id);
	}
	
	template <>	inline Units& Env::get_cont<Unit>() {
		return units;
	}
	
	template <>	inline Builds& Env::get_cont<Build>() {
		return builds;
	}
	
	template <>	inline Colonies& Env::get_cont<Colony>() {
		return colonies;
	}
	
	template <>	inline Players& Env::get_cont<Player>() {
		return players;
	}
	
	template <>	inline UnitTypes& Env::get_cont<UnitType>() {
		return *uts;
	}
	
	template <>	inline TerrTypes& Env::get_cont<TerrType>() {
		return *tts;
	}
	
	template <>	inline BuildTypes& Env::get_cont<BuildType>() {
		return *bts;
	}

	template <>	inline Units const& Env::get_cont<Unit>() const {
		return units;
	}
	
	template <>	inline Builds const& Env::get_cont<Build>() const {
		return builds;
	}
	
	template <>	inline Colonies const& Env::get_cont<Colony>() const {
		return colonies;
	}
	
	template <>	inline Players const& Env::get_cont<Player>() const {
		return players;
	}
	
	template <>	inline UnitTypes const& Env::get_cont<UnitType>() const {
		return *uts;
	}
	
	template <>	inline TerrTypes const& Env::get_cont<TerrType>() const {
		return *tts;
	}
	
	template <>	inline BuildTypes const& Env::get_cont<BuildType>() const {
		return *bts;
	}
	
	
	
	template <> inline
	bool Env::in_bound<Dir::A>(Coords const& p) const {
		return 0 <= p[0];
	}
	
	template <> inline
	bool Env::in_bound<Dir::D>(Coords const& p) const {
		return p[0] < w;
	}
	
	template <> inline
	bool Env::in_bound<Dir::W>(Coords const& p) const {
		return 0 <= p[1];
	}
	
	template <> inline
	bool Env::in_bound<Dir::X>(Coords const& p) const {
		return p[1] < h;
	}
	
	template <> inline
	bool Env::on_bound<Dir::A>(Coords const& p) const {
		return 0 == p[0];
	}
	
	template <> inline
	bool Env::on_bound<Dir::D>(Coords const& p) const {
		return p[0] == w - 1;
	}
	
	template <> inline
	bool Env::on_bound<Dir::W>(Coords const& p) const {
		return 0 == p[1];
	}
	
	template <> inline
	bool Env::on_bound<Dir::X>(Coords const& p) const {
		return p[1] == h - 1;
	}
	
	
	inline
	bool Env::in_bounds(Coords const& p) const {
		return in_bound<Dir::A>(p) and in_bound<Dir::D>(p)
		   and in_bound<Dir::W>(p) and in_bound<Dir::X>(p);
	}
		
	
}

#endif