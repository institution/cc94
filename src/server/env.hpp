#pragma once

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>


#include "core.hpp"
#include "inter.hpp"
#include "prof.hpp"
#include "base.hpp"

namespace col{

	using Auth = uint32;


	struct InvalidAction: Error {
		using Error::Error;
	};


	// game event like: "Colony Abc builds Warehouse"
	struct Msg{
		Nation const* nation{nullptr};
		Terr const* terr{nullptr};
		string msg{""};

		Msg(Nation const& p, Terr const& t, string const& msg):
			nation(&p), terr(&t), msg(msg)
		{}



	};



	inline bool compatible(Travel const& x, Travel const& y) {
		return x & y;
	}


	template<typename RET, typename THIS, typename ARG>
	RET & get_defender_impl(THIS & self, ARG & t) {
		if (! t.units.size()) {
			throw Error("no defending unit");
		}
		return *(t.units)[0];
	}

	using ErrorMsg = char const*;


	struct Env;

	string write_string(Env const& env, Unit const& x);
	void read_string(Env & env, Unit & x, string const& data);

	template<class Archive>
    void write(Archive & ar, Env const& env);
    
    template<class Archive>
    void read(Archive & ar, Env & env);

	struct ProdCons{Amount prod{0}, cons{0};};

	Prof get_unit_occupation(Unit const& u);


	Prof get_prof_by_item(Item item);
	bool is_expert(Unit const& unit, Item const& item);


	

	struct Env: Core {
	
		/** 
			Has 2 kinds of interface
			restricted - availble to players - actions allowed by game rules
			full - availble when map editing
			
			interface should be enforced by Agent himself?
			
		**/

		using State = uint8_t;
		
		static
		State const 
			StateEdit,
			StatePlay,
			StateExit;

		// turn system
		uint32 turn_no{0};
		uint32 turn_limit{0}; // nonzero => end game when turn_no >= turn_limit
		Nation::Id cpid{0}; // current nation
		State state{StateEdit}; // runlevels: 0 - prepare, 1 - playing, 2 - ended; use in_progress to check

		// misc non game state
		uint32 mod;
		boost::function<int (int range)> roll;

		// opts
		int8_t verbose{0};
		uint32_t action_count{0};
		
		State get_state() const {
			return state;
		}
		
		void set_state(State state) {
			this->state = state;
		}



		void clear_all() {
			clear_units();
			clear_colonies();
			clear_nations();
		}

		void clear_misc() {
			turn_no = 0;
			turn_limit = 0;
			state = StateEdit;
			//cpid = -1;
			mod++;
		}


		/*void update_terr(Terr::Id id, Alt alt, Biome biome, Phys phys) {
		
		}
		
		void update_unit(Unit::Id id, ) {
		
		}*/

		/// for visible unit
		void for_unit(Nation const& nation, std::function<void(Unit const&)> block) {			
			for (auto & p: units) 
			{				
				if (has_vision(nation, p.second.get_terr())) {
					block(p.second);
				}
			}
		}
		
		void for_unit(std::function<void(Unit &)> block) {
			for (auto & p: units) 
			{				
				block(p.second);
			}
		}


		Coords get_dim() const { return Coords(w,h); }

		int get_turn_no() const {
			return turn_no;
		}

		bool has_control(Nation const& nat, Unit const& unit) {
			return nat.id == unit.nation->id;
		}



		void apply(inter::start a) {
			start();
			//notify_effect(a);
		}

		void apply(inter::activate a) {

		}


		
		
		Nation const* get_current_nation_p() const {
			if (!in_progress()) {
				return nullptr;
			}
			return &nations.at(cpid);
		}

		Nation const& get_current_nation() const {
			if (!in_progress()) {
				throw Error("no current nation: game in regress");
			}
			return nations.at(cpid);
		}
		
		Nation & get_current_nation() {
			return const_cast<Nation&>(
				static_cast<Env const*>	(this) -> get_current_nation() );
		}

		Env& set_current_nation(Nation const& p) {
			auto it = nations.find(p.id);
			if (it == nations.end()) {
				throw Error("no such nation");
			}
			cpid = p.id;
			return *this;
		}


		void start() {
			//if (nations.size() < 1) {
			//	throw Error("need at least one nation to start game");
			//}

			state = StatePlay;


			cpid = 0;
			while (nations.find(cpid) == nations.end()) {
				if (cpid > 100) {   // TODO: pfff...
					throw Error("UNEXPECTED ERROR: cannot find nation");
				}
				++cpid;
			}


		}


		//Storage & get_store(Unit & x) { return x.store; }
		Storage & get_store(Colony & x) { return x.store; }
		Storage & get_store(Terr & x) { return x.get_colony().store; }

		//Storage const& get_store(Unit const& x) const { return x.store; }
		Storage const& get_store(Colony const& x) const { return x.store; }
		Storage const& get_store(Terr const& x) const { return x.get_colony().store; }



		void unit_set_item(Unit & u, Item item, Amount num) {
			u.set(item, num);
		}

		void terr_set_item(Terr & t, Item item, Amount num) {
			get_store(t).set(item, num);
		}

		void load_cargo(Unit & u, Item item, Amount want_num) {
			auto & t = get_terr(u);
			auto & st = get_store(t);

			auto terr_num = st.get(item);
			Amount free_num = u.get_space_left();

			if (free_num < 0) {
				throw Error("ykhm; free_space=%||", free_num);
			}

			auto mod_num = std::min({terr_num, free_num, want_num});

			unit_set_item(u, item, u.get(item) + mod_num);
			terr_set_item(t, item, terr_num - mod_num);
		}

		void unload_cargo(Unit & u, Item item, Amount want_num) {
			auto & t = get_terr(u);
			auto & st = get_store(t);

			auto unit_num = u.get(item);

			auto mod_num = std::min({unit_num, want_num});

			unit_set_item(u, item, unit_num - mod_num);
			terr_set_item(t, item, st.get(item) + mod_num);
		}


		void apply(inter::load_cargo const& a) {
			auto & u = get<Unit>(a.unit_id);

			if (a.num > 0) {
				load_cargo(u, a.item, a.num);
			}
			else if (a.num < 0)
				unload_cargo(u, a.item, -a.num);
			else {

			}

		}


		explicit
		Env(int verbose = 1): Core(), verbose(verbose) {
			set_random_gen(roll::roll1);
			clear_misc();
		}
		
		explicit
		Env(Coords dim, Terr const& def): Core(), verbose(1) {
			set_random_gen(roll::roll1);
			clear_misc();
			resize(dim);
			fill(def);
		}


		Env& set_random_gen(boost::function<int (int range)> const& func) {
			roll = func;
			return *this;
		}



		bool in_progress() const {
			return state == StatePlay;
		}





		/*
		void activate_all() {
			for (auto& p: nations) {
				if (auto u = p.second.get_player()) {
					u->apply_inter(inter::activate(), *this);
				}
			}
		}*/



		
		
		// Execute production on given storage and workplace
		// * food consumed by workers are substracted from storage (no food = no production)
		// * workers APs are used (time left <- zero)
		// * produced items are added to store
		// * consumed items are substracted from store
		// * progress is increased on workplace task
		void produce(Store & st, Workplace & wp);
		

		
		void resolve_builds(Terr & terr);
		void resolve_teaching(Store & st, Workplace & b, Terr & t);



		template <class F, int i, int j>
		void resolve(Colony & c) {
			// resolve production of items inside [i,j) range
			
			// basket sort fields/builds by produced item
			array<vector<F*>, j-i> ws;

			for (auto& wp: c.get_cont<F>()) {
				auto v = wp.get_proditem();
				if (v) { // may produce None
					if (v < i or j <= v) {
						throw Error("produced item out of range");
					}
					ws.at(v-i).push_back(&wp);
				}
			}

			// produce items in order; items produced earlier can be used by produced later
			// iterate by items
			for (size_t item_id = i; item_id < j; ++item_id) {
				for (F* wp: ws.at(item_id-i)) {					
					produce(get_store(c), *wp);
				}
			}


			// teaching
			resolve_builds(c.get_terr());
			
		}

		
		

		void turn(Terr & t) {

			if (!t.has_colony()) {
				return;
			}

			auto& st = get_store(t);
			auto& c = t.get_colony();
			auto p = get_control(t);

			// resolve production
			resolve<Field, 1, 10>(c);
			resolve<Build, 10, 22>(c);

			
			
			// construction
			for (auto& b: c.builds) {
				resolve_construction(t, b);
			}
			for (auto& f: c.fields) {
				resolve_construction(t, f);
			}
			

			// food consumption/starvation
			/*vector<Unit*> starve;
			for (auto * u: t.units) {
				if (u->is_working() and u.time_left == TIME_UNIT) {
					// consume food
					if (st.get(ItemFood) >= 2) {
						st.sub(ItemFood, 2);
					}
					else {
						// starvation
						starve.push_back(u);
					}
					
					u.time_left = 0;
				}
			}*/

			// destroy starved units
			/*for (Unit* u: starve) {
				free(*u);
				units.erase(get_id(*u));
			}
			starve.clear();
			*/

			// new colonists creation
			if (p and st.get(ItemFood) >= 100) {
				st.sub(ItemFood, 100);
				
				init(get<UnitType>(1), t, *p);
				

			}

			// decay of non-storageable resources
			st.set(ItemHammers, 0);
			st.set(ItemBell, 0);
			st.set(ItemCross, 0);

			// decay resources above storage limit
			for (auto& e: st) {
				if (e.amount > c.get_max_storage()) {
					e.amount = c.get_max_storage();
				}
			}

		}

		void turn() {
			/*
			 turn order:

			 [global]
				increase turn counter
				
			 [per tile]
				time reset on units
				colony production
				building construction
				food consumption/starvation
				new colonists creation
				items decay (due to non-storageable nature or lack of storage space)
			*/

			// time progress
			turn_no++;
			//notify_effect(inter::set_turn(turn_no));

			clear_vision();

			// time reset on units and vision recalc
			for (auto& item: units) {
				Unit & u = item.second;
				
				u.time_left = TIME_UNIT;				
				update_vision(u);
			}

			// all colonies
			for (auto& p: colonies) {
				turn(get_terr(p.second));
			}

			if (turn_limit > 0 and turn_no >= turn_limit) {
				state = StateExit;
			}

			++mod;
		}



		void equip(Unit & u, UnitType & ut);


		bool consume_food(Unit & u) {
			// return -- true if food consumed, false when no more food available
			Terr & t = get_terr(u);
			Storage st = get_store(t);
			if (st.get(ItemFood) >= 2) {
				st.sub(ItemFood, 2);
				return 1;
			}
			else {
				return 0;
			}
		}


		/*Terr const& get_visible_terr(Nation const& f, Coords c) {
			// TODO
		}*/


		bool has_vision(Nation const& f, Terr const& t) {
			return t.get_vision(f.id);
		}

		


		void reset(Coords const& dim) {
			// clear_all
			// resize
			// fill with dessert ocean
			clear_units();
			clear_colonies();

			resize({dim[0],dim[1]});

		}

		/*void apply(inter::load const& a) {

			std::istringstream ar(a.data);

			read(ar, *this);

			//notify_effect(a);
		}*/

		Env & fill(Terr const& t) {

			for (Coord j = 0; j < h; ++j) {
				for (Coord i = 0; i < w; ++i) {
					auto& x = get_terr(Coords(i,j));
					x.biome = t.biome;
					x.phys = t.phys;
					x.alt = t.alt;
				}
			}

			return *this;
		}


		bool check(int L, int M) {
			int r = roll(M); // 0, M-1
			if (verbose) {
				print("check %||/%|| -> %||\n", L, M, r + 1);
			}
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
				if (verbose) {
					print("check no need\n");
				}
				u.time_left -= time_cost;
				return 1;
			}
		}


		void order() {
			//

		}

		int get_roughness(Terr const& t) const;

		int get_improv_cost(Terr const& t) const {
			return 3 * get_roughness(t) * TIME_UNIT;
		}


		void record_action() {
			action_count += 1;
		}


		int get_transport_space(Terr const& dest, Nation const& nation) const {
			int space = 0;
			for (auto& p: dest.units) {
				if (p->get_nation() == nation) {
					space += p->get_space_left();
					if (p->transported) {
						space -= p->get_size();
					}
				}
			}
			return space;
		}

		int get_movement_cost(Terr const& dest, Terr const& orig, Travel const&) const;


		bool has_transport(Terr const& dest, Unit const& u) const {
			return get_transport_space(dest, u.get_nation()) >= u.get_size();
		}


		bool has_tools(Unit const& u) {
			return u.get_item1() == ItemTools and u.get_num1() >= 20;
		}

		void use_tools(Unit & u) {
			// special hack for pioniers only
			u.set_type(get<UnitType>(u.get_type_id() - 1));
		}


		bool move_unit(int8 dx, int8 dy, Unit & u) {
			/// Move/board unit on adjacent square/ship
			

			// unit checks
			if (u.get_nation() != get_current_nation()) {
				throw Error("not your unit");
			}

			if (u.get_speed() == 0) {
				throw Error("this unit cannot move (speed = 0)");
			}

			// direction check
			if (!(-1 <= dx and dx <= 1 and -1 <= dy and dy <= 1)) {
				throw Error("invalid coords");
			}

			if (dx == 0 and dy == 0) {
				throw Error("already here");
			}

			auto & orig = get_terr(u);
			auto dest_coords = get_coords(orig) + Coords(dx,dy);

			if (!in_bounds(dest_coords)) {
				throw Error("destination outside of map");
			}

			auto & dest = get_terr(dest_coords);

			bool transported = 0;

			if (!compatible(u.get_travel(), dest.get_travel())) {
				if (has_transport(dest, u)) {
					transported = 1;
				}
				else {
					throw Error("cannot travel through this terrain");
				}
			}

			// dynamic check
			Nation * cp = get_control(dest);
			if (cp and cp->id != get_current_nation().id) {
				throw Error("destination occupied by enemy");
			}

			// execute

			// cost to move
			int time_cost = -1;

			if (transported or u.transported) {
				// board/unboard from/to land
				time_cost = TIME_UNIT;
			}
			else {
				time_cost = get_movement_cost(dest, orig, u.get_travel()) / u.get_speed();
			}

			if (time_cost <= 0) {
				throw Error("time_cost == 0???");
			}


			if (run_map_task(u, time_cost)) {
				t_move(dest, u);
				update_vision(u);   // only main unit, transported units dont have vision 

				/*
				notify_effect(orig,
					inter::take_unit(get_id(u))
				);

				notify_effect(dest,
					inter::set_unit(get_id(u), write_string(*this, u))
				);
				*/

				u.transported = transported;

				auto space = u.get_space_left();
				for (auto& p: orig.units) {
					if (space < 100) break;
					if (p->transported) {
						auto & u2 = *p;
						t_move(dest, u2);

						/*
						notify_effect(orig,
							inter::take_unit(get_id(u2))
						);

						notify_effect(dest,
							inter::set_unit(get_id(u2), write_string(*this, u2))
						);
						*/

						space -= 100;
					}
				}

				record_action();

				return 1; // success
			}

			record_action();
			return 0; // try next turn

		}




		bool has_defender(Coords const& pos) const {
			return get_terr(pos).has_units();
		}

		bool has_defender(Terr const& terr) const {
			return terr.has_units();
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

		//Terr & get_dest(Unit &u, Dir) {
		//	return get_terr(get_coords(u) + vec4dir(dir));
		//}



		void kill(Unit & u) {
			// this function will erase from container you are currently iterating over
			// TODO: delayed removal?
			free(u);

			Core::remove<Unit>(get_id(u));


			// wow! that was radical
		}

		bool order_attack(Unit & attacker, Dir::t const& dir, bool exec=1) {
			/* Order attack
			 */

			if (dir == Dir::S) {
				throw Error("cannot attack self");
			}

			// get terr
			auto& orig = get_terr(attacker);
			auto dest_coords = get_coords(orig) + vec4dir(dir);
			if (!in_bounds(dest_coords)) {
				throw Error("target outside of map");
			}
			auto & dest = get_terr(dest_coords);

			if (!compatible(attacker.get_travel(), dest.get_travel())) {
				throw Error("this unit cannot attack accross that terrain");
			}

			if (attacker.get_attack() == 0) {
				throw Error("this unit has no offensive capability");
			}

			if (!has_defender(dest)) {
				throw Error("no-one to attack");
			}

			auto& defender = get_defender(dest);

			// cost to approach enemy and fight = 1t
			auto time_cost = TIME_UNIT;

			// execute
			if (exec) {
				if (run_map_task(attacker, time_cost)) {
					// fight
					auto attack_base = attacker.get_attack();
					auto attack = attack_base * (1.0 + 0.5);  // attack bonus
					auto combat_base = defender.get_combat();
					auto combat = combat_base * (1.0 + get_defense_bonus(dest, defender.get_travel()));

					if (verbose) {
						print(" -------- combat -------- \n");
						print("attack = %|| + %||\n", attack_base, attack_base * 0.5);
						print("combat = %|| + %||\n", 
							combat_base, 
							combat_base * get_defense_bonus(dest, defender.get_travel())
						);
						print("odds   %|| : %||\n", attack * 10, combat*10);
					}

					if (check(attack*10, (attack + combat)*10)) {
						kill(defender);
					}
					else {
						kill(attacker);
					}

					record_action();
					return 1; // command executed
				}
				else {
					record_action();
					return 0; // try next turn
				}
			}
			else {
				return -1;
			}
		}


		bool destroy(Unit & u, Phys const& feat) {
			// unit checks
			if (u.get_nation() != get_current_nation()) {
				throw InvalidAction("not your unit");
			}

			if (!has_tools(u)) {
				throw InvalidAction("need tools");
			}

			auto& t = get_terr(u);

			// terrain checks
			if (!compatible(t.get_travel(), TravelLand)) {
				throw InvalidAction("can clear on land only");
			}

			if (!t.has_phys(feat)) {
				throw InvalidAction("no feat to clear");
			}

			if (run_map_task(u, get_improv_cost(t))) {
				use_tools(u);
				t.sub_phys(feat);
				return 1;
			}
			return 0;
		}




		void improve(Unit & u, Phys const& feat) {
			auto unit_id = get_id(u);

			if (!(feat & PhysAlterable)) {
				throw InvalidAction("only road,plow and forest can be altered");
			}

			// unit checks
			if (u.get_nation() != get_current_nation()) {
				throw InvalidAction("not your unit");
			}

			if (!has_tools(u)) {
				throw InvalidAction("need tools");
			}

			auto& t = get_terr(u);

			// terrain checks
			if (!compatible(t.get_travel(), TravelLand)) {
				throw InvalidAction("can build on land only");
			}

			if (t.has_phys(feat)) {
				throw InvalidAction("improvment already exists");
			}

			if (feat == PhysPlow and t.has_phys(PhysForest)) {
				throw InvalidAction("cannot plow forest; clear first");
			}

			if (run_map_task(u, get_improv_cost(t))) {
				use_tools(u);
				t.add_phys(feat);
			}
		}




		bool a_destroy(Unit & u, Phys const& feat) {

			if (!(feat & PhysAlterable)) {
				throw Error("only road,plow and forest can be altered");
			}

			// unit checks
			if (u.get_nation() != get_current_nation()) {
				throw Error("not your unit");
			}

			if (!has_tools(u)) {
				throw Error("need tools");
			}

			auto& t = get_terr(u);

			// terrain checks
			if (!compatible(t.get_travel(), TravelLand)) {
				throw Error("not on land");
			}

			if (!t.has_phys(feat)) {
				throw Error("no feat here");
			}

			if (run_map_task(u, get_improv_cost(t))) {
				use_tools(u);
				t.sub_phys(feat);
				return 1;
			}
			return 0;
		}


		// calc some kind of score
		float get_result(Nation::Id pid) {

			// % of units
			float score = 0, total = 0;
			for (auto& p: units) {
				if (p.second.get_nation().id == pid) {
					score += 1;
				}
				total += 1;
			}

			if (total) {
				return score/total;
			}
			else {
				return score;
			}

		}


		void kill(Colony & c) {
			c.terr->colony = nullptr;
			c.terr = nullptr;
		}

		void burn_colony(Terr & t) {
			assert(t.has_colony());

			auto& col = t.get_colony();
			kill(col);
			auto id = col.id;
			Core::remove<Colony>(id);
		}

		void set_build(Colony & c, int i, BuildType::Id const& id) {
			c.set_build(i, Build(get<BuildType>(id)));
		}


		void set_tp(Unit & u, int tp) {
			u.time_left = tp;
		}

		/*
		void apply(inter::set_tp const& e) {
			auto & u = get<Unit>(e.unit_id);
			set_tp(u, e.num);
			//notify_effect(get_terr(u), e);
		}
		 */

		/// How much of 'item can be produced by 'unit in 'build
		Amount get_prod(Build const& build, Unit const& unit, Item const& item) const;
		
		/// How much of raw materials needed to produce 'item will be consumed by 'unit in 'build
		Amount get_cons(Build const& build, Unit const& unit, Item const& item) const;

		/// How much of 'item can be produced by 'unit on 'terr
		Amount get_prod(Terr const& terr, Unit const& unit, Item const& item) const;



		bool colonize(Unit &u, string const& name) {

			if (u.get_nation() != get_current_nation()) {
				throw Error("not your unit");
			}

			if (!compatible(u.get_travel(), TravelLand)) {
				throw Error("not land-based unit");
			}

			auto& t = get_terr(u);

			if (t.has_colony()) {
				throw Error("already colonized");
			}

			auto time_cost = TIME_UNIT;
			if (run_map_task(u, time_cost)) {

				init_colony(t);

				return 1;  // ok
			}
			return 0;  // try next turn
		}

		void apply(inter::build_colony const& a) {
			colonize(get<Unit>(a.unit_id), "");
		}


		bool can_make(Build const& build, BuildType const& bt) const;
		bool can_make(Build const& build, UnitType const& ut) const;

		void construct(Build & build, BuildType const& mk) {			
			if (can_make(build, mk)) {
				build.task.reset(&mk);
			}
			else {
				// TODO: error
				assert(0);
			}
		}
			
		void construct(Build & build, UnitType const& mk) {
			if (can_make(build, mk)) {
				build.task.reset(&mk);
			}
			else {
				// TODO: error
				assert(0);
			}
						
		}
		
		void apply(inter::construct_build const& a) {
			auto& t = get_terr(a.terr_id);
			
			construct(
				t.get_colony().get_build(a.build_id),
				get<BuildType>(a.buildtype_id)
			);			
		}
		
		void apply(inter::construct_unit const& a) {
			auto& t = get_terr(a.terr_id);
			
			construct(
				t.get_colony().get_build(a.build_id),
				get<UnitType>(a.unittype_id)
			);			
		}

		void put_build(Terr & t, int slot, BuildType & btype) {
			t.get_colony().builds.at(slot) = Build(btype);
		}

		void put_build(Terr & t, int slot, BuildType::Id const& btype_id) {
			put_build(t, slot, get<BuildType>(btype_id));
		}


		//vector<BuildType*> const& get_allowed_builds(Terr const& t, int slot_id) {
		//	vector<BuildType*> abts;
		//	for ()
		//}


		float get_defense_bonus(Terr const& terr, Travel const& trav) {
			if (trav == TravelLand) {
				return get_land_defense_bonus(terr);
			}
			else if (trav == TravelSea) {
				return get_sea_defense_bonus(terr);
			}
			else {
				throw runtime_error("aaa");
			}
		}

		float get_land_defense_bonus(Terr const& terr) {
			auto const& alt = terr.get_alt();

			if (alt == AltMountain) {
				return 1.00;
			}
			else if (alt == AltHill) {
				return 0.50;
			}
			else if (terr.has_phys(PhysForest)) {
				return 0.50;
			}
			return 0.0;
		}

		float get_sea_defense_bonus(Terr const& terr){
			return 0.0;
		}




		Dir::type get_dir_to_from(Terr const& to, Terr const& from) {
			return dir4vec(get_coords(to) - get_coords(from));
		}

		Nation const* get_control(Terr const& terr) const {
			if (terr.units.size()) {
				return &terr.units.at(0)->get_nation();
			}
			return nullptr;
		}

		Nation * get_control(Terr const& terr) {
			if (terr.units.size()) {
				return &terr.units.at(0)->get_nation();
			}
			return nullptr;
		}


		int consume_part(Terr & t, Item const& item, Amount const& num) {
			Store & st = get_store(t);
			auto real = std::min(st.get(item), num);
			st.sub(item, real);
			return real;
		}

		bool consume_all(Terr & t, Item const& item, Amount const& num) {
			Store & st = get_store(t);
			if (st.get(item) >= num) {
				st.sub(item, num);
				return true;
			}
			return false;
		}


		
		void resolve_construction(Terr & terr, Workplace & f);

		bool has_control(Terr const& terr, Nation const& p) const {
			Nation const* c = get_control(terr);
			return c and *c == p;
		}


		// order
		void set_proditem_field(Nation const& p, Terr & terr, Field::Id field_id, Item const& item) {
			get_field(terr, field_id, p).set_proditem(item);
		}


		void set_proditem_field(Terr & terr, Field::Id field_id, Item const& item) {
			get_field(terr, field_id).set_proditem(item);
		}





		Field const& get_field(Terr const& terr, int field_id) const {
			if (terr.has_colony()) {
				return terr.get_colony().fields.at(field_id);
			}
			else {
				throw Error("no colony here");
			}
		}

		Field & get_field(Terr & terr, int field_id) {
			if (terr.has_colony()) {
				return terr.get_colony().fields.at(field_id);
			}
			else {
				throw Error("no colony here");
			}
		}

		Field const& get_field(Terr const& terr, int const& field_id, Nation const& p) const {
			if (has_control(terr, p)) {
				return get_field(terr, field_id);
			}
			else {
				throw Error("this nation has no control here");
			}
		}

		Field & get_field(Terr & terr, int const& field_id, Nation const& p) {
			if (has_control(terr, p)) {
				return get_field(terr, field_id);
			}
			else {
				throw Error("this nation has no control here");
			}
		}

		void work_none(Unit & u) {
			if (u.workplace) {
				u.workplace->sub(u);
				u.workplace = nullptr;
			}			
		}






		// build - unit
		void t_move(Workplace & b, Unit & u) {
			work_none(u);

			b.add(u);
			u.workplace = &b;
		}


		void init(Terr & t, Colony & c) {
			assert(t.colony == nullptr);
			assert(c.terr == nullptr);
			t.set_colony(c);
			c.terr = &t;
		}



		// Terr <-
		template <typename X>
		void t_move(Terr & t, X & x) {
			x.terr->sub(x);
			t.add(x);
			x.terr = &t;
		}

		template <typename X>
		void take(X & x) {
			x.terr->sub(x);
			x.terr = nullptr;
		}

		template <typename X>
		void put(Terr & t, X & x) {
			t.add(x);
			x.terr = &t;
		}
		
		


		void clear_vision() {
			auto dim = get_dim();
			
			for (int j = 0; j < dim[1]; j++)
			{
				for (int i = 0; i < dim[0]; i++)
				{
					get_terr({i,j}).clear_vision();
				}
			}			
		}
		
		Nation::Id get_control_id(Unit const& u) const { return u.get_nation().id; }


		void update_vision(Unit const& u) {
			auto p = get_coords(u);
			auto id = get_control_id(u);
			Coord r = 2;
			auto dim = get_dim();
			
			auto i0 = std::max<Coord>(0, p[0]-r);
			auto i1 = std::min<Coord>(p[0]+r, dim[0]-1);
			
			auto j0 = std::max<Coord>(0, p[1]-r);
			auto j1 = std::min<Coord>(p[1]+r, dim[1]-1);
			
			for (Coord j = j0; j <= j1; ++j)
			{
				for (Coord i = i0; i <= i1; ++i)
				{
					get_terr({i,j}).mark_vision(id);
				}				
			}
		}


		void init(Terr & t, Unit & u) {
			t.add(u);
			u.terr = &t;
			update_vision(u);
		}
		
		void init(UnitType const& ut, Terr & t, Nation & nat) {
			init(t, create<Unit>(ut, nat));			
		}
		


		Build & get_build(Terr & terr, int build_id) {
			if (terr.has_colony()) {
				return terr.get_colony().builds.at(build_id);
			}
			else {
				throw Error("no colony here");
			}
		}

		Build & get_build(Terr & terr, int build_id, Nation const& p) {
			if (has_control(terr, p)) {
				return get_build(terr, build_id);
			}
			else {
				throw Error("this nation has no control here");
			}
		}



		void work_field(Field::Id field_id, Unit & u) {
			Terr & terr = get_terr(u);
			auto& f = get_field(terr, field_id, u.get_nation());
			work_workplace(f, u);
			//notify_effect(terr,	inter::work_field(field_id, get_id(u)));
		}

		void work_build(Build::Id build_id, Unit & u) {
			Terr & terr = get_terr(u);
			auto& f = get_build(terr, build_id, u.get_nation());
			work_workplace(f, u);
			//notify_effect(terr,	inter::work_build(build_id, get_id(u)));
		}



		void apply(inter::work_build const& a) {
			work_build(a.build_id, get<Unit>(a.unit_id));
		}

		void apply(inter::work_field const& a) {
			work_field(a.field_id, get<Unit>(a.unit_id));
		}

		void apply(inter::work_none const& a) {
			work_none(get<Unit>(a.unit_id));
		}





		void apply(inter::prod_field const& a) {
			auto & n = get_current_nation();
			auto & t = get_terr(a.terr_id);
			set_proditem_field(n, t, a.field_id, a.item_id);
			//notify_effect(t, a);
		}

		void apply(inter::prod_build const& a) {
			throw Error("not yet used");
			/*auto & n = get_current_nation();
			auto & t = get_terr(a.terr_id);
			set_proditem_build(n, t, a.build_id, a.item_id);
			notify_effect(t, a);*/
		}





		
		void work_workplace(Workplace & wp, Unit & u) {
			if (&u.get_workplace() == &wp) {

			}
			if (wp.get_space_left() >= u.get_size()) {
				t_move(wp, u);
			}
			else {
				throw Error("no space left");
			}
		}


		Terr& get_colony_field(Terr const& ct, int num) {
			return get_terr(vec4dir(num) + get_coords(ct));
		}



		bool ready(Nation const& p) {

			if (get_current_nation().id != p.id) {
				throw Error("not your turn");
			}


			++cpid;
			while (nations.find(cpid) == nations.end()) {
				if (cpid > 10) {
					turn();     // TURN HERE
					cpid = 0;
				}
				else {
					++cpid;
				}
			}

			record_action();

			/*if (auto u = get_current_nation().get_player()) {
				u->apply_inter(inter::activate(), *this);
			}*/


			return 1;
		}




		Workplace const& get_workplace_by_index(Unit const& unit, int num) const {
			auto const& terr = get_terr(unit);
			if (num < 0) {
				throw Error("invalid place index");
			}
			else if (num < 16) {
				if (!terr.has_colony()) {
					throw Error("no colony");
				}
				auto const& col = terr.get_colony();
				return col.builds.at(num);
			}
			else if (num < 25) {
				return get_field(terr, num - 16);
			}
			else {
				throw Error("invalid place index");
			}
		}

		void init_colony(Terr & t);




		void set_turn(uint32 turn_no) {
			this->turn_no = turn_no;
		}

		void set_current_nation(Nation::Id const& nation_id) {
			set_current_nation(get<Nation>(nation_id));
		}

		void set_biome(Terr::Id terr_id, Biome const& biome) {
			get<Terr>(terr_id).set_biome(biome);
		}

		void set_alt(Terr::Id terr_id, Alt const& alt) {
			get<Terr>(terr_id).set_alt(alt);
		}

		void set_phys(Terr::Id terr_id, Phys const& phys) {
			get<Terr>(terr_id).set_phys(phys);
		}



		void apply(inter::ready const& a) {
			ready(get_current_nation());
		}

		void toogle_board(Unit & u) {
			u.transported = !u.transported;
			//notify_effect(get_terr(u), inter::toogle_board(get_id(u)));
		}

		void apply(inter::toogle_board const& a) {
			toogle_board(get<Unit>(a.unit_id));
		}



		void apply(inter::move_unit const& a) {
			move_unit(a.dx, a.dy, get<Unit>(a.unit_id));
		}


		void apply(inter::improve const& a) {
			improve(get<Unit>(a.unit_id), a.phys_id);
		}


		void apply(inter::destroy const& a) {
			a_destroy(get<Unit>(a.unit_id), a.phys_id);
		}

		void sub_item(Terr & t, Item const& item, Amount num) {
			Store & st = get_store(t);
			auto& c = t.get_colony();
			st.sub(item, num);
			//notify_effect(t, inter::set_item(get_id(t), item, st.get(item)));
		}

		void morph_unit(Unit & u, UnitType const& ut) {
			u.set_type(ut);
			//notify_effect(get_terr(u), inter::morph_unit(get_id(u), get_id(ut)));
		}







		void set_owner(const Unit::Id &icon_id, const Nation::Id &nation_id) {
			units.at(icon_id).nation = &nations.at(nation_id);
			++mod;
		}

		Nation& get_nation(Nation::Id const& id) {
			return nations.at(id);
		}

		const Nation& get_nation(const Nation::Id &id) const {
			auto p = nations.find(id);
			if (p != nations.end()) {
				return (*p).second;
			}
			else {
				throw std::runtime_error(format("no nation with id=%||", id));
			}
		}

		void del_nation(const Nation::Id &id) {
			nations.erase(id);
			++mod;
		}

		Nation::Id next_key_nation() {
			return nations.size();
		}

		void apply_inter(inter::Any const& a, uint32 auth);


		

	};

	
	

	struct Apply: boost::static_visitor<void>{
		Env & env;
		Apply(Env & e): env(e) {}


		template <class T>
		void operator()(T const& t) {
			env.apply(t);
		}

	};


	inline void Env::apply_inter(inter::Any const& a, uint32 auth) {
		if (auth != get_current_nation().auth) {
			throw Error("not your turn");
		}

		Apply ap(*this);
		boost::apply_visitor(ap, a);
	}




}

