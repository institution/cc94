#pragma once

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>


#include "core.hpp"
#include "prof.hpp"
#include "base.hpp"
#include "faction.hpp"

namespace col{

	using Auth = uint32;


	struct InvalidAction: Error {
		using Error::Error;
	};


	// game event like: "Colony Abc builds Warehouse"
	struct Msg{
		Faction const* nation{nullptr};
		Terr const* terr{nullptr};
		string msg{""};

		Msg(Faction const& p, Terr const& t, string const& msg):
			nation(&p), terr(&t), msg(msg)
		{}



	};

	using b2c = ext::b2<Coord>;

	/// Command result: 1 Ok, 0 Try next turn, -1 Not possible
	using Res = int8_t;
	Res const 
		ResErr = -1,
		ResCont = 0,
		ResOk = 1;
	

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


	using TurnNo = int32_t;

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
		TurnNo turn_no{0};
		TurnNo turn_limit{0}; // nonzero => end game when turn_no >= turn_limit
		
		Control current_control{ControlNone}; // current faction
		
		State state{StateEdit}; // runlevels: 0 - prepare, 1 - playing, 2 - ended; use in_progress to check

		// misc non game state
		TurnNo mod;
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
			clear_factions();
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
		void for_unit(Control cc, std::function<void(Unit const&)> block) {			
			for (auto & p: units) 
			{				
				if (has_vision(cc, p.second.get_terr())) {
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

		bool has_control(Control control, Unit const& unit) {
			return control == unit.get_control();
		}


		bool has_current_control() const 
		{
			return current_control != ControlNone;			
		}
		
		Control get_current_control() const
		{
			return current_control;
			/*
			auto p = factions.find(current_control);
			if (p != factions.end()) {
				return (*p).second.get_control();
			}
			else {
				return ControlNone;
			}*/
		}


		void set_current_control(Control cc) {
			current_control = cc;
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

		Res load_cargo(Unit & u, Item item, Amount want_num) {
			auto & t = get_terr(u);
			auto & st = get_store(t);

			auto terr_num = st.get(item);
			Amount free_num = u.get_space_left();

			assert(free_num >= 0);
			
			auto mod_num = std::min({terr_num, free_num, want_num});

			unit_set_item(u, item, u.get(item) + mod_num);
			terr_set_item(t, item, terr_num - mod_num);
			
			return ResOk;
		}

		Res unload_cargo(Unit & u, Item item, Amount want_num) {
			auto & t = get_terr(u);
			auto & st = get_store(t);

			auto unit_num = u.get(item);

			auto mod_num = std::min({unit_num, want_num});

			unit_set_item(u, item, unit_num - mod_num);
			terr_set_item(t, item, st.get(item) + mod_num);
			
			return ResOk;
		}

		
		Faction * get_current_faction() {
			auto c = get_current_control();
			if (c != ControlNone) {
				return & get<Faction>(c);
			}
			return nullptr;			
		}


		Res move_cargo(Unit & u, Item item, Amount num) {
			if (num > 0) {
				return load_cargo(u, item, num);
			}
			else if (num < 0)
				return unload_cargo(u, item, -num);
			else {
				return ResOk;  // zero moved
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

			for (auto& wp: c.get_cont<F>()) 
			{
				auto v = wp.get_proditem();
				if (v != ItemNone) { // may produce None
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

		
		bool can_construct(Build const& b) const {
			return b.get_proditem() == ItemHammers;			
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
			
			auto control = get_control(t);
			
			if (control != ControlNone and st.get(ItemFood) >= 100) 
			{
				st.sub(ItemFood, 100);				
				put(
					t,
					create<Unit>(get<UnitType>(1), control)
				);
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


		bool has_vision(Control c, Terr const& t) {
			return t.get_vision(c);
		}

		


		void reset(Coords const& dim) {
			// clear_all
			// resize
			// fill with dessert ocean
			clear_units();
			clear_colonies();

			resize({dim[0],dim[1]});

		}

		
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

		bool run_map_task(Unit &u, int8_t time_cost) {
			/*
			 */
			if (time_cost > TIME_UNIT * 8) {
				set_error("this task will never finish");
				return 0;
			}

			auto time_left = u.time_left;
			
			if (time_left < time_cost) {
				u.time_left = 0;
				return check(time_left, time_cost);
			}
			else {
				//if (verbose) {
				//	print("check no need\n");
				//}
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


		/*int get_transport_space(Terr const& dest, Control control) const {
			int space = 0;
			for (auto& p: dest.units) {
				if (p->get_control() == control) {
					space += p->get_space_left();
					if (p->transported) {
						space -= p->get_size();
					}
				}
			}
			return space;
		}*/

		bool can_load(Unit const& transport, Unit const& passenger) const 
		{
			assert(0);
		}

		Time get_movement_cost(Terr const& dest, Terr const& orig, Travel const&) const;


		


		bool has_tools(Unit const& u) {
			return u.get_item1() == ItemTools and u.get_num1() >= 20;
		}

		void use_tools(Unit & u) {
			// special hack for pioniers only
			u.set_type(get<UnitType>(u.get_type_id() - 1));
		}

		string error_message;
		
		bool has_error() const {
			return error_message.size() > 0;
		}
		
		void set_error(string const& s) {
			error_message = s;
		}
		
		string read_error() {
			auto s = error_message;
			error_message = "";
			return s;
		}

		Unit & get_unit(Unit::Id id) {
			return get<Unit>(id);
		}


		/// return movment cost in time units (TimeInf -- movment impossible)
		Time get_move_cost(Terr const& dest, Terr const& orig, Unit const& u);
		
		

		
		Unit * find_transport(Terr const& terr, Unit const& passenger) 
		{
			for (auto * transport: terr.get_units())
			{
				if (transport->can_add(passenger)) {
					return transport;
				}
			}
			return nullptr;			
		}


		

		
		Res move_unit(Unit & u, int8_t dx, int8_t dy) {
			/// Move/board unit on adjacent square/ship

			if (dx == 0 and dy == 0) {
				// already here - no problem
				return ResOk;
			}

			// direction check
			if (!(-1 <= dx and dx <= 1 and -1 <= dy and dy <= 1)) {
				set_error("invalid direction given");
				return ResErr;
			}

			// unit checks
			if (u.get_speed() == 0) {
				set_error("this unit cannot move (speed = 0)");
				return ResErr;
			}

			auto & orig = get_terr(u);
			auto dest_coords = get_coords(orig) + Coords(dx,dy);

			if (!in_bounds(dest_coords)) {
				set_error("destination outside of map");
				return ResErr;
			}

			auto & dest = get_terr(dest_coords);

			Unit * transport = nullptr;
			if (not compatible(u.get_travel(), dest.get_travel())) 
			{
				transport = find_transport(dest, u);
				
				if (transport == nullptr) {			
					set_error("cannot travel through this terrain");
					return ResErr;
				}
			}
			
			
			// dynamic check
			
			auto dc = get_control(dest);
			if (dc != ControlNone and dc != u.get_control()) {
				set_error("destination occupied by enemy");
				return ResErr;
			}

			// execute

			// cost to move
			Time time_cost = TimeInf;

			if (transport) {
				time_cost = TimeUnit;
			}
			else {
				time_cost = get_move_cost(dest, orig, u);
			}
			
			if (time_cost <= 0) {
				ext::fail("time_cost == 0???");
			}


			if (run_map_task(u, time_cost)) {
				t_move(dest, u);
				update_vision(u);   // only main unit, transported units dont have vision 
				return ResOk; // success
			}
			return ResCont; // try next turn

		}

		Res move_unit(Unit & u, Dir dir) {
			auto v = dir2vec(dir);
			return move_unit(u, v[0], v[1]);
		}
		
		Res move_unit(Unit & u, v2c v) {
			return move_unit(u, v[0], v[1]);
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



		void kill(Unit & u) {
			// this function will erase from container you are currently iterating over
			// TODO: delayed removal?
			free(u);

			Core::remove<Unit>(get_id(u));


			// wow! that was radical
		}

		Res order_attack(Unit & attacker, Dir dir, bool exec=1) {
			/* Order attack
			 */

			if (dir == DirS) {
				throw Error("cannot attack self");
			}

			// get terr
			auto& orig = get_terr(attacker);
			auto dest_coords = get_coords(orig) + dir2vec(dir);
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
					return ResOk; // command executed
				}
				else {
					record_action();
					return ResCont; // try next turn
				}
			}
			else {
				return ResErr;
			}
		}


		Res destroy(Unit & u, Phys const& feat) 
		{
			// unit checks
			if (!has_tools(u)) {
				set_error("need tools");
				return ResErr;
			}

			auto& t = get_terr(u);

			// terrain checks
			if (!compatible(t.get_travel(), TravelLand)) {
				set_error("can clear on land only");
				return ResErr;
			}

			if (!t.has_phys(feat)) {
				set_error("no feat to clear");
				return ResErr;
			}

			if (run_map_task(u, get_improv_cost(t))) {
				use_tools(u);
				t.sub_phys(feat);
				return ResOk;
			}
			return ResCont;
		}




		Res improve(Unit & u, Phys const& feat) {
			auto unit_id = get_id(u);

			if (!(feat & PhysAlterable)) {
				set_error("only road,plow and forest can be altered");
				return ResErr;
			}

			// unit checks
			if (!has_tools(u)) {
				set_error("need tools");
				return ResErr;
			}

			auto& t = get_terr(u);

			// terrain checks
			if (!compatible(t.get_travel(), TravelLand)) {
				set_error("can build on land only");
				return ResErr;
			}

			if (t.has_phys(feat)) {
				set_error("improvment already exists");
				return ResErr;
			}

			if (feat == PhysPlow and t.has_phys(PhysForest)) {
				set_error("cannot plow forest; clear first");
				return ResErr;
			}

			if (run_map_task(u, get_improv_cost(t))) {
				use_tools(u);
				t.add_phys(feat);
				return ResOk;
			}
			else {
				return ResCont;
			}
		}




		Res a_destroy(Unit & u, Phys const& feat) {

			if (!(feat & PhysAlterable)) {
				set_error("only road,plow and forest can be altered");
				return ResErr;
			}

			// unit checks
			if (!has_tools(u)) {
				set_error("need tools");
				return ResErr;
			}

			auto& t = get_terr(u);

			// terrain checks
			if (!compatible(t.get_travel(), TravelLand)) {
				set_error("not on land");
				return ResErr;
			}

			if (!t.has_phys(feat)) {
				set_error("no feat here");
				return ResErr;
			}

			if (run_map_task(u, get_improv_cost(t))) {
				use_tools(u);
				t.sub_phys(feat);
				return ResOk;
			}
			else {
				return ResCont;
			}
		}


		// calc some kind of score
		float get_result(Control pid) {

			// % of units
			float score = 0, total = 0;
			for (auto& p: units) {
				if (p.second.get_control() == pid) {
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

		
		/// How much of 'item can be produced by 'unit in 'build
		Amount get_prod(Build const& build, Unit const& unit, Item const& item) const;
		
		/// How much of raw materials needed to produce 'item will be consumed by 'unit in 'build
		Amount get_cons(Build const& build, Unit const& unit, Item const& item) const;

		/// How much of 'item can be produced by 'unit on 'terr
		Amount get_prod(Terr const& terr, Unit const& unit, Item const& item) const;


		/// Can colony be build on 'terr
		bool is_colonizable(Terr const& terr) const {
			auto alt = terr.get_alt();
			return 
				not terr.has_colony() 
				and AltFlat <= alt and alt <= AltHill;			
		}
		
		
		
		/// Is 'unit capable of building colony
		bool is_colonist(Unit const& unit) {
			return unit.get_type().id <= UnitTypeCavalry;
			// return compatible(unit.get_travel(), TravelLand)
		}

		
		
		Res colonize(Unit &u, string const& name = "") 
		{
			if (not is_colonist(u)) {
				set_error("this unit cannot colonize");
				return ResErr;				
			}

			auto& t = get_terr(u);

			if (not is_colonizable(t)) {
				set_error("cannot colonize here");
				return ResErr;				
			}

			auto time_cost = TIME_UNIT;
			if (run_map_task(u, time_cost)) {
				init_colony(t);
				return ResOk;  // ok
			}
			return ResCont;  // try next turn
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




		Dir get_dir_to_from(Terr const& to, Terr const& from) {
			return vec2dir(get_coords(to) - get_coords(from));
		}

		Control get_control(Terr const& terr) const {
			if (terr.units.size()) {
				return terr.units.at(0)->get_control();
			}
			return ControlNone;
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

		bool has_control(Terr const& terr, Control control) const {
			auto c = get_control(terr);
			return c != ControlNone and c == control;
		}


		// order
		Res set_proditem_field(Terr & terr, Field::Id field_id, Item const& item) {
			if (has_field(terr, field_id))
			{
				get_field(terr, field_id).set_proditem(item);
				return ResOk;
			}
			set_error("cannot set proditem: no such field");
			return ResErr;
		}






		bool has_field(Terr const& terr, uint8_t field_id) const {
			return terr.has_colony();
		}
		
		Field const& get_field(Terr const& terr, uint8_t field_id) const {			
			return terr.get_colony().fields.at(field_id);			
		}

		Field & get_field(Terr & terr, uint8_t field_id) {
			return terr.get_colony().fields.at(field_id);			
		}
		
		Res work_none(Unit & u) {
			if (u.workplace) {
				u.workplace->sub(u);
				u.workplace = nullptr;
			}			
			return ResOk;
		}






		// build - unit
		void t_move(Workplace & b, Unit & u) {
			work_none(u);

			b.add(u);
			u.workplace = &b;
		}

		void put(Build & b, Unit & u) {
			work_none(u);
			b.add(u);
			u.workplace = &b;
		}


		void put(Terr & t, Colony & c) {
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
		


		void update_vision(Unit const& u) {
			if (not u.has_control()) {
				return;
			}
			
			auto p = get_coords(u);
			auto id = u.get_control();
			
			if (0 < id and id <= 32) {
				uint32_t vid = 1 << (id - 1);
				
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
						get_terr({i,j}).mark_vision(vid);
					}				
				}
				
			}
			else {
				print(std::cerr, "WARNING: update_vision: id=%||; not in range\n", id);
			}
		}
		
		


		void put(Terr & t, Unit & u) {
			t.add(u);
			u.terr = &t;
			update_vision(u);
		}
		
		bool has_build(Terr & terr, Build::Id build_id) {
			return terr.has_colony() and 0 <= build_id and build_id <= 15;
		}
		
		Build & get_build(Terr & terr, int build_id) {
			return terr.get_colony().builds.at(build_id);			
		}

		Res work_field(Field::Id field_id, Unit & u) {
			Terr & terr = get_terr(u);
			if (has_field(terr, field_id))
			{
				auto& f = get_field(terr, field_id);
				return work_workplace(f, u);
			}
			else {
				set_error("cannot work: no such field");
				return ResErr;
			}
		}

		Res work_build(Build::Id build_id, Unit & u) {
			Terr & terr = get_terr(u);
		
			if (has_build(terr, build_id)) {
				auto& f = get_build(terr, build_id);
				return work_workplace(f, u);
			}
			else {
				set_error("no such building here");
				return ResErr;
			}
		}






		
		Res work_workplace(Workplace & wp, Unit & u) 
		{			
			if (wp.get_space_left() >= u.get_size()) {
				t_move(wp, u);
				return ResOk;
			}
			else {
				set_error("no space left");
				return ResErr;				
			}
		}


		Terr& get_colony_field(Terr const& ct, int num) {
			return get_terr(dir2vec(num) + get_coords(ct));
		}



		Res ready() 
		{
			if (factions.size() == 0) {
				current_control = ControlNone;
				turn();
			}
			else {
				do {
				
					current_control += 1;	
					
					if (current_control > 31) {
						// TURN HERE
						turn();     
						current_control = 0;
					}
					
				} while (factions.find(current_control) == factions.end());
			}
			
			return ResOk;
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


		Amount get_base_prod(Terr const& terr, Item const& item) const;
		
		

		void set_turn(uint32 turn_no) {
			this->turn_no = turn_no;
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



		

		void set_owner(Unit::Id & uid, Control control) {
			units.at(uid).set_control(control);
			++mod;
		}

		Faction & get_faction(Faction::Id id) {
			return get<Faction>(id);
		}


		

	};

	
	




}

