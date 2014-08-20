#ifndef ENV_H
#define ENV_H

#include <signal.h>
#include <list>

#include "col.hpp"
#include "base.h"
#include "objs.h"
#include "nation.h"
#include "csv.h"
#include "terr.h"
#include "build.h"
#include "unit.h"
#include "roll.h"
#include "error.h"
#include "field.h"
#include "inter.h"


namespace col{


	struct Player{

		virtual void activate() = 0;
		virtual bool apply_inter(inter::Any const& a) = 0;

		//template <class t>
		//virtual void receive(T const& t) = 0;
		// CONTROL FLOW
		// 1: master -> ai: activate
		//    ai -> master: sends commands
		//    ai -> master: ready
		//    goto 1 or 2
		// 2: master -> ai: win/lose

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

	using Msgs = vector<Msg>;



	using BuildTypes = unordered_map<BuildType::Id, BuildType>;
	using UnitTypes = unordered_map<UnitType::Id, UnitType>;

	using Terrs = boost::multi_array<Terr, 2>;
	using Units = unordered_map<Unit::Id, Unit>;
	//using Builds = unordered_map<Build::Id, Build>;
	using Colonies = unordered_map<Colony::Id, Colony>;
	// using Nations = unordered_map<Nation::Id, Nation>;
	using Nations = unordered_map<Nation::Id, Nation>;



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



	struct Env {

		Msgs msgs;

		void add_message(Nation const& p, Terr const& t, string const& m) {
			msgs.push_back(Msg(p,t,m));
		}

		void clear_messages() {
			msgs.clear();
		}

		// const
		shared_ptr<BuildTypes> bts;
		shared_ptr<UnitTypes> uts;

		// state
		Nations nations;
		Colonies colonies;
		Units units;

		Nation::Id cpid;

		// map state
		Coord w, h;
		Terrs terrs;

		// global state
		uint32 turn_no;
		uint32 next_id;
		uint8 state; // runlevels: 0 - prepare, 1 - playing, 2 - ended; use in_progress to check
		uint32 turn_limit; // nonzero => end game when turn_no >= turn_limit

		// misc non game state
		uint32 mod;
		boost::function<int (int range)> roll;

		int verbose{0};
		int action_count{0};


		void clear() {
			units.clear();
			colonies.clear();
			nations.clear();

			resize({0,0});

			next_id = 0;
			turn_no = 0;
			state = 0;

			cpid = -1;

			mod++;
		}

		void connect(Nation::Id pid, Player & u) {
			// connect nation(game nation) with AI or Human
			get<Nation>(pid).set_player(&u);
		}

		int get_turn_no() const {
			return turn_no;
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
			if (nations.size() < 1) {
				throw Critical("need at least one nation to start game");
			}

			state = 1;
			cpid = 0;
			while (nations.find(cpid) == nations.end()) {
				if (cpid > 100) {   // TODO: pfff...
					throw Critical("UNEXPECTED ERROR: cannot find nation");
				}
				++cpid;
			}
		}


		Unit::Id get_id(Unit const& u) const {
			return u.id;
		}



		explicit
		Env(int verbose=0): terrs(Coords(0,0), boost::fortran_storage_order()) {

			this->verbose = verbose;

			bts = make_shared<BuildTypes>();
			uts = make_shared<UnitTypes>();

			set_random_gen(roll::roll1);

			turn_limit = 0;

			clear();

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
		T const& get(typename T::Id const& id) const {
			auto it = get_cont<T>().find(id);
			if (it == get_cont<T>().end()) {
				throw Error("no such object");
			}
			return (*it).second;
		}

		template <typename T>
		T & get(typename T::Id const& id) {
			return const_cast<T&> (
				static_cast<Env const*> (this) -> get<T>(id) );
		}

		template <typename T>
		bool exist(typename T::Id const& id) {
			return get_cont<T>().count(id);
		}

		bool in_progress() const {
			return state == 1;
		}



		bool ready(Nation const& p, bool exec=1) {

			if (get_current_nation().id != p.id) {
				if (exec) {
					throw Critical("not your turn");
				}
				else {
					throw Error("not your turn");
				}
			}

			if (exec) {
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

				if (auto u = get_current_nation().get_player()) {
					u->activate();
				}
			}

			return 1;
		}



		void activate_all() {
			for (auto& p: nations) {
				if (auto u = p.second.get_player()) {
					u->activate();
				}
			}
		}






		void produce(Colony & c, Field & wp, Unit & u) {
			auto const& proditem = wp.get_proditem();

			auto prodnum = get_terr(wp).get_yield(proditem, false);

			if (proditem == ItemFish) {
				c.add(ItemFood, prodnum);
			}
			else {
				c.add(proditem, prodnum);
			}

			u.time_left = 0;
		}

		void produce(Colony & c, Build & wp, Unit & u) {
			auto const& proditem = wp.get_proditem();
			auto const& consitem = wp.get_consitem();

			int base = get_yield(u, proditem);
			int prodnum = wp.get_prod() * base;
			int consnum = wp.get_cons() * base;

			int real_cons = std::min(consnum, int(c.get(consitem)));
			int real_prod = float(float(prodnum) * float(real_cons) / float(consnum));

			c.sub(consitem, real_cons);
			add_prod(c, proditem, real_prod);

			u.time_left = 0;
		}

		template<typename F, int i, int j>
		void resolve(Colony & c) {
			// sort fields by produced item
			array<vector<F*>, j-i> ws;

			for (auto& wp: c.get_cont<F>()) {
				int v = wp.get_proditem().get_value();
				if (v) { // may produce None
					if (v < i or j <= v) {
						throw Critical("produced item out of range");
					}
					ws.at(v-i).push_back(&wp);
				}
			}

			// produce items in order; items produced earlier can be used by produced later
			// iterate by items
			for (size_t item_id = i; item_id < j; ++item_id) {
				for (F* wp: ws.at(item_id-i)) {
					for (auto& u: wp->units) {
						produce(c, *wp, *u);
					}
				}
			}
		}

		void turn(Terr & t) {

			if (!t.has_colony()) {
				return;
			}

			auto& c = t.get_colony();
			auto p = get_control(t);

			// resolve production
			resolve<Field, 1, 10>(c);
			resolve<Build, 10, 21>(c);

			// construction
			for (auto& b: c.builds) {
				construction_consume(t, b);
			}

			// food consumption/starvation
			size_t i = 0;
			while (i < t.units.size()) {
				Unit &u = *t.units.at(i);
				if (u.is_working()) {
					// consume food
					if (c.has(ItemFood, 2)) {
						c.sub(ItemFood, 2);
					}
					else {
						// starvation
						// this will remove element from current iteration !!!
						kill(u);
						// do not iterate
						continue;
					}
				}
				++i;
			}

			// new colonists creation
			if (p and c.get(ItemFood) >= 100) {
				c.sub(ItemFood, 100);
				auto& u = create<Unit>(get<UnitType>(101), *p);
				init(u,t);
				add_message(*p, t, "population growth");
			}

			// decay of non-storageable resources
			c.set(ItemHammers, 0);
			c.set(ItemBell, 0);
			c.set(ItemCross, 0);

			// decay resources above storage limit
			for (auto& p: c.storage) {
				auto& item = p.first;
				auto& value = p.second;
				if (value > c.get_max_storage()) {
					value = c.get_max_storage();
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


			// clear old messages
			clear_messages();  // <- replacve with effects and let clients collect messages from them (TODO)

			// time progress
			++turn_no;

			// time reset on units
			for (auto& item: units) {
				// cerr << "turn calc for: " << item.second << endl;
				Unit & u = item.second;
				u.time_left = TIME_UNIT;
			}

			// all colonies
			for (auto& p: colonies) {
				turn(get_terr(p.second));
			}

			if (turn_limit > 0 and turn_no >= turn_limit) {
				state = 2;
			}

			++mod;
		}


		int get_yield(Unit const& u, Item const& item) const {
			return 3;
		}

		void add_prod(Colony & c, Item const& item, int num) {
			c.add(item, num);
		}

		int get_prodnum(Field const& f) const {
			return get_terr(f).get_yield(f.get_proditem(), false);
		}

		void equip(Unit & u, UnitType & ut) {
			if (u.get_nation() != get_current_nation()) {
				throw Error("not your unit");
			}

			Terr & t = get_terr(u);
			Colony & c = t.get_colony();

			if (u.get_base() != ut.get_base()) {
				throw Error("incompatible types");
			}

			// recover
			c.add(u.get_item1(), u.get_num1());
			c.add(u.get_item2(), u.get_num2());

			if (c.has(ut.get_item1(), ut.get_num1())
				and c.has(ut.get_item2(), ut.get_num2()))
			{
				// consume
				c.sub(ut.get_item1(), ut.get_num1());
				c.sub(ut.get_item2(), ut.get_num2());

				// change type
				u.set_type(ut);
			}
			else {
				// rollback
				c.sub(u.get_item1(), u.get_num1());
				c.sub(u.get_item2(), u.get_num2());

				throw Error("not enough equipment");
			}
		}


		bool consume_food(Unit & u) {
			// return -- true if food consumed, false when no more food available
			Terr & t = get_terr(u);
			if (t.get_colony().get(ItemFood) >= 2) {
				t.get_colony().sub(ItemFood, 2);
				return 1;
			}
			else {
				return 0;
			}
		}





		bool has_vision(Nation const& p, Terr const& t) {
			return 1;
		}


		void send(Nation const& p, inter::Any const& a) {
			if (auto u = p.get_player()) {
				u->apply_inter(a);
			}
		}


		void notify_effect(Terr const& t, inter::Any const& a) {
			for (auto& p: nations) {
				if (has_vision(p.second, t)) {
					send(p.second, a);
				}
			}
		}



		bool in_bounds(Coords const& p) const;

		template <Dir::t D>
		bool in_bound(Coords const& p) const;

		template <Dir::t D>
		bool on_bound(Coords const& p) const;



		Coords get_coords(Terr const& t) const {

			// offset = x*w + y

			uint32 offset = uint32(&t - &terrs[0][0]);
			return Coords(offset % w, offset / w);
		}


		Coords get_coords(Terr const& t, Terr const& base) const {
			return get_coords(t) - get_coords(t);
		}

		Coords get_coords(Unit const& u) const {
			return get_coords(get_terr(u));
		}

		Terr const& get_terr(Coords const& z) const {
			return terrs(z);
		}

		Terr & get_terr(Coords const& z) {
				return terrs(z);
		}


		template <typename T>
		Terr const& get_terr(T const& u) const {
			if (u.terr) {
				return *u.terr;
			}
			else {
				throw Error("not in field");
			}
		}


		template <typename T> inline
		Terr & get_terr(T const& u) {
			if (u.terr) {
				return *u.terr;
			}
			else {
				throw Error("not in field");
			}
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
					auto& x = get_terr(Coords(i,j));
					x.biome = t.biome;
					x.phys = t.phys;
					x.alt = t.alt;
				}
			}

			return *this;
		}


		Unit& icon4id(Unit::Id const& id) {
			return units.at(id);
		}



	/*
		void produce(Unit const& unit, Item const& item, Dir::t const& dir) {


		}


		void construct(Unit const& unit) {

		}

		void move(Unit const& unit, Build const& build) {

		}
			*/

		bool check(int L, int M) {
			int r = roll(M); // 0, M-1
			if (verbose) {
				cerr << format("check %||/%|| -> %||\n", L, M, r + 1);
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
				if (verbose)
					cerr << "check no need" << endl;
				u.time_left -= time_cost;
				return 1;
			}
		}


		void order(){
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


		bool move_board(int8 dx, int8 dy, Unit & u) {
			/* Move/board unit on adjacent square/ship
			 */

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
				throw Critical("time_cost == 0???");
			}


			if (run_map_task(u, time_cost)) {
				t_move(dest, u);
				u.transported = transported;

				auto space = u.get_space_left();
				for (auto& p: orig.units) {
					if (!space) break;
					if (p->transported) {
						t_move(dest, *p);
						space -= 1;
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
			if (u.terr) {
				u.terr->sub(u);
				u.terr = nullptr;
			}
			if (u.build) {
				u.build->sub(u);
				u.build = nullptr;
			}
			if (u.field) {
				u.field->sub(u);
				u.field = nullptr;
			}
			u.type = nullptr;
			u.nation = nullptr;

			units.erase(get_id(u));

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
						cout << " -------- combat -------- \n";
						cout << "attack = " << attack_base << " + " << attack_base * 0.5 << endl;
						cout << "combat = " << combat_base << " + " << combat_base * get_defense_bonus(dest, defender.get_travel()) << endl;
						cout << "odds   " << attack*10 << " : " << combat*10 << endl;
					}

					if (check(attack*10, (attack + combat)*10)) {
						kill(defender);
						destroy<Unit>(defender.id);
					}
					else {
						kill(attacker);
						destroy<Unit>(attacker.id);
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
				throw Error("not your unit");
			}

			if (!has_tools(u)) {
				throw Error("need tools");
			}

			auto& t = get_terr(u);

			// terrain checks
			if (!compatible(t.get_travel(), LAND)) {
				throw Error("can clear on land only");
			}

			if (!t.has(feat)) {
				throw Error("no feat to clear");
			}

			if (run_map_task(u, get_improv_cost(t))) {
				use_tools(u);
				t.sub(feat);
				return 1;
			}
			return 0;
		}



		bool improve(Unit & u, Phys const& feat) {


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
			if (!compatible(t.get_travel(), LAND)) {
				throw Error("can build on land only");
			}

			if (t.has(feat)) {
				throw Error("feat already exists");
			}

			if (run_map_task(u, get_improv_cost(t))) {
				use_tools(u);
				t.add(feat);
				return 1;
			}
			return 0;
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
			if (!compatible(t.get_travel(), LAND)) {
				throw Error("not on land");
			}

			if (!t.has(feat)) {
				throw Error("no feat here");
			}

			if (run_map_task(u, get_improv_cost(t))) {
				use_tools(u);
				t.sub(feat);
				return 1;
			}
			return 0;
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
			destroy<Colony>(id);
		}

		void set_build(Colony & c, int i, BuildType::Id const& id) {
			c.set_build(i, Build(get<BuildType>(id)));
		}



		void init_colony() {

		}


		void set_tp(Unit & u, int tp) {
			u.time_left = tp;
		}

		bool apply(inter::set_tp const& e) {
			auto & u = get<Unit>(e.unit_id);
			set_tp(u, e.num);
			notify_effect(get_terr(u), e);
			return 1;
		}






		bool colonize(Unit &u, string const& name) {

			if (u.get_nation() != get_current_nation()) {
				throw Error("not your unit");
			}

			if (!compatible(u.get_travel(), LAND)) {
				throw Error("not land-based unit");
			}

			auto& t = get_terr(u);

			if (t.has_colony()) {
				throw Error("already colonized");
			}

			auto time_cost = TIME_UNIT;
			if (run_map_task(u, time_cost)) {
				// create colony
				auto& c = create<Colony>(name);
				init(c, t);

				BuildType::Id const TREE_1 = 45;
				BuildType::Id const TREE_2 = 44;
				BuildType::Id const TREE_3 = 43;
				BuildType::Id const COAST = 46;
				BuildType::Id const CARPENTERS_SHOP = 36;
				BuildType::Id const FENCE = 17;
				BuildType::Id const CHURCH = 38;
				BuildType::Id const TOWN_HALL = 10;

				// init buildings
				set_build(c, 0, CARPENTERS_SHOP);
				set_build(c, 1, TREE_1);
				set_build(c, 2, TREE_3);
				set_build(c, 3, TREE_1);
				set_build(c, 4, TREE_1);

				set_build(c, 5, TREE_1);
				set_build(c, 6, TREE_1);
				set_build(c, 7, TREE_1);
				set_build(c, 8, TREE_1);
				set_build(c, 9, TREE_2);

				set_build(c, 10, TREE_2);
				set_build(c, 11, TREE_2);
				set_build(c, 12, TOWN_HALL);
				set_build(c, 13, COAST);
				set_build(c, 14, FENCE);

				// init fields
				for (auto dir: ALL_DIRS) {
					auto dest = get_coords(t) + vec4dir(dir);
					if (in_bounds(dest)) {
						c.add_field(Field(get_terr(dest)));
					}
				}

				return 1;  // ok
			}
			return 0;  // try next turn
		}

		bool apply(inter::build_colony const& a) {
			return colonize(get<Unit>(a.unit_id), "");
		}


		void colony_construct(Colony & colony, BuildType::Id type_id, int slot) {
			auto& type = get<BuildType>(type_id);
			colony.construct_building(type, slot);
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
			if (trav == LAND) {
				return get_land_defense_bonus(terr);
			}
			else if (trav == SEA) {
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
			else if (terr.has(PhysForest)) {
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


		int consume_part(Terr & t, Item const& item, int const& num) {
			int real = std::min(int(t.get_colony().get(item)), num);
			t.get_colony().sub(item, real);
			return real;
		}

		bool consume_all(Terr & t, Item const& item, int const& num) {
			if (int(t.get_colony().get(item)) >= num) {
				t.get_colony().sub(item, num);
				return true;
			}
			return false;
		}


		void construction_consume(Terr & t, Build & b) {
			if (b.under_construction()) {
				// add hammers return overflow
				auto& c = t.get_colony();

				b.hammers += consume_part(t, ItemHammers, b.get_cost_hammers() - b.hammers);

				if (b.hammers == b.get_cost_hammers() ) {
					if (consume_all(t, ItemTools, b.get_cost_tools())) {
						b.morph();

						// warehouse constructed
						if (b.get_type().id == 16) {
							c.max_storage += 100;
						}

						add_message(*get_control(t), t, "building constructed");
					}
				}
			}


		}


		bool has_control(Terr const& terr, Nation const& p) const {
			Nation const* c = get_control(terr);
			return c and *c == p;
		}


		// order
		void set_proditem_field(Nation const& p, Terr & terr, int field_id, Item const& item) {
			get_field(terr, field_id, p).set_proditem(item);
		}


		void set_proditem_field(Terr & terr, int field_id, Item const& item) {
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
			if (u.build) {
				u.build->sub(u);
				u.build = nullptr;
			}
			else if (u.field) {
				u.field->sub(u);
				u.field = nullptr;
			}
		}

		// build - unit
		void t_move(Build & b, Unit & u) {
			work_none(u);

			b.add(u);
			u.build = &b;
		}

		// field - unit
		void t_move(Field & b, Unit & u) {
			work_none(u);

			b.add(u);
			u.field = &b;
		}


		void init(Colony & c, Terr & t) {
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


		void init(Unit & u, Terr & t) {
			t.add(u);
			u.terr = &t;
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



		void work_field(int field_id, Unit & u) {
			Terr & terr = get_terr(u);
			auto& f = get_field(terr, field_id, u.get_nation());
			work_workplace(f, u);
		}

		void work_build(int build_id, Unit & u) {
			Terr & terr = get_terr(u);
			auto& f = get_build(terr, build_id, u.get_nation());
			work_workplace(f, u);
		}

		template<typename T>
		void work_workplace(T & wp, Unit & u) {
			if (&u.get_workplace<T>() == &wp) {

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


		Workplace const& get_workplace_by_index(Unit const& unit, int num) const {
			auto const& terr = get_terr(unit);
			if (num < 0) {
				throw Critical("invalid place index");
			}
			else if (num < 16) {
				if (!terr.has_colony()) {
					throw Critical("no colony");
				}
				auto const& col = terr.get_colony();
				return col.builds.at(num);
			}
			else if (num < 25) {
				return get_field(terr, num - 16);
			}
			else {
				throw Critical("invalid place index");
			}
		}





		bool apply(inter::ready const& a) {
			ready(get_current_nation());
			return 1;
		}


		bool apply(inter::move_board const& a) {
			return move_board(a.dx, a.dy, get<Unit>(a.unit_id));
		}


		bool apply(inter::improve const& a) {
			return improve(get<Unit>(a.unit_id), a.phys_id);
		}


		bool apply(inter::destroy const& a) {
			return a_destroy(get<Unit>(a.unit_id), a.phys_id);
		}

		bool apply(inter::init_colony const& a) {
			return 1;
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

		bool apply(inter::echo const& a) {
			return 1;
		}

		bool apply_inter(inter::Any const& a);


		template<class Archive>
		void save(Archive & ar, uint const& version) const;

		template<class Archive>
		void load(Archive & ar, uint const& version);

		BOOST_SERIALIZATION_SPLIT_MEMBER()


	};



	struct Apply: boost::static_visitor<bool>{
		Env & env;
		Apply(Env & e): env(e) {}


		template <class T>
		bool operator()(T const& t) {
			return env.apply(t);
		}

	};






	inline bool Env::apply_inter(inter::Any const& a) {
		Apply ap(*this);
		return boost::apply_visitor(ap, a);
	}






	template <> inline
	Terr & Env::get<Terr>(Terr::Id const& id) {
		return get_terr(id);
	}

	template <> inline
	Terr const& Env::get(Terr::Id const& id) const {
		return get_terr(id);
	}

	template <>	inline Units& Env::get_cont<Unit>() {
		return units;
	}

	template <>	inline Colonies& Env::get_cont<Colony>() {
		return colonies;
	}

	template <>	inline Nations& Env::get_cont<Nation>() {
		return nations;
	}

	template <>	inline UnitTypes& Env::get_cont<UnitType>() {
		return *uts;
	}

	template <>	inline BuildTypes& Env::get_cont<BuildType>() {
		return *bts;
	}

	template <>	inline Units const& Env::get_cont<Unit>() const {
		return units;
	}

	template <>	inline Colonies const& Env::get_cont<Colony>() const {
		return colonies;
	}

	template <>	inline Nations const& Env::get_cont<Nation>() const {
		return nations;
	}

	template <>	inline UnitTypes const& Env::get_cont<UnitType>() const {
		return *uts;
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