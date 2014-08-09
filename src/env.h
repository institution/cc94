#ifndef ENV_H
#define ENV_H

#include <signal.h>
#include <list>

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
#include "field.h"




namespace col{



	// game event like: "Colony Abc builds Warehouse"
	struct Msg{
		Player const* player{nullptr};
		Terr const* terr{nullptr};
		string msg{""};

		Msg(Player const& p, Terr const& t, string const& msg):
			player(&p), terr(&t), msg(msg)
		{}



	};

	using Msgs = vector<Msg>;


	using Id = uint32;

	using BuildTypes = unordered_map<BuildType::Id, BuildType>;
	using UnitTypes = unordered_map<UnitType::Id, UnitType>;

	using Terrs = boost::multi_array<Terr, 2>;
	using Units = unordered_map<Unit::Id, Unit>;
	//using Builds = unordered_map<Build::Id, Build>;
	using Colonies = unordered_map<Colony::Id, Colony>;
	// using Players = unordered_map<Player::Id, Player>;
	using Players = unordered_map<Player::Id, Player>;


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



	struct Env{

		Msgs msgs;

		void add_message(Player const& p, Terr const& t, string const& m) {
			msgs.push_back(Msg(p,t,m));
		}

		void clear_messages() {
			msgs.clear();
		}

		// const
		shared_ptr<BuildTypes> bts;
		shared_ptr<UnitTypes> uts;

		// state
		Players players;
		Colonies colonies;
		Units units;

		Player::Id cpid;

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
			players.clear();

			resize({0,0});

			next_id = 0;
			turn_no = 0;
			state = 0;

			cpid = -1;

			mod++;
		}

		void connect(Player::Id pid, User & u) {
			// connect player(game nation) with AI or Human
			get<Player>(pid).set_user(&u);
		}

		int get_turn_no() const {
			return turn_no;
		}


		Player const* get_current_player_p() const {
			if (!in_progress()) {
				return nullptr;
			}
			return &players.at(cpid);
		}

		Player const& get_current_player() const {
			if (!in_progress()) {
				throw Error("no current player: game in regress");
			}
			return players.at(cpid);
		}

		Player & get_current_player() {
			return const_cast<Player&>(
				static_cast<Env const*>	(this) -> get_current_player() );
		}

		Env& set_current_player(Player const& p) {
			auto it = players.find(p.id);
			if (it == players.end()) {
				throw Error("no such player");
			}
			cpid = p.id;
			return *this;
		}


		void start() {
			state = 1;
			cpid = 0;
			while (players.find(cpid) == players.end()) {
				if (cpid > 10) {
					throw Critical("need at least one player to start game");
				}
				++cpid;
			}
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

		void ready(Player const& p, bool exec=1) {

			if (get_current_player().id != p.id) {
				if (exec) {
					throw Critical("not your turn");
				}
				else {
					throw Error("not your turn");
				}
			}

			if (exec) {
				++cpid;
				while (players.find(cpid) == players.end()) {
					if (cpid > 10) {
						turn();     // TURN HERE
						cpid = 0;
					}
					else {
						++cpid;
					}
				}

				record_action();

				if (auto u = get_current_player().get_user()) {
					u->activate();
				}
			}

		}

		void turn(Colony & c) {
			auto& t = get_terr(c);
			auto p = get_control(t);

			// new colonists creation
			if (p and c.get(ItemFood) >= 100) {

				c.sub(ItemFood, 100);
				c.add(ItemFood, 20);
				auto& u = create<Unit>(get<UnitType>(101), *p);
				init(u,t);
				add_message(*p, t, "population growth");
			}



			// construction
			for (auto& b: c.builds) {
				construction_consume(t, b);
			}

			// clear non-staragable resources
			c.set(ItemHammers, 0);
			c.set(ItemBell, 0);
			c.set(ItemCross, 0);

			// clear resources above max storage
			for (auto& p: c.storage) {
				auto& item = p.first;
				auto& value = p.second;
				if (value > c.get_max_storage()) {
					value = c.get_max_storage();
				}

			}

		}

		void turn() {
			// clear old messages
			clear_messages();

			// time progress
			++turn_no;

			// all units - new movment, work
			for (auto& item: units) {
				// cerr << "turn calc for: " << item.second << endl;
				turn(item.second);
			}

			// all colonies
			for (auto& p: colonies) {
				turn(p.second);
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

		void turn(Unit &u) {

			u.time_left = TIME_UNIT;

			if (u.order == Order::Space) {
				u.order = Order::Unknown;
			}

			if (u.build) {

				auto & t = get_terr(u);
				auto & c = t.get_colony();

				if (consume_food(u)) {

					auto const& wp = u.get_workplace<Build>();

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
				else {
					work_none(u);
				}
			}
			else if (u.field) {
				auto & t = get_terr(u);
				auto & c = t.get_colony();

				if (consume_food(u)) {
					auto const& wp = u.get_workplace<Field>();

					auto const& proditem = wp.get_proditem();

					auto prodnum = get_terr(wp).get_yield(proditem, false);

					switch (proditem) {
						case ItemFish:
							c.add(ItemFood, prodnum);
							break;
						default:
							c.add(proditem, prodnum);
							break;
					}


					u.time_left = 0;
				}
				else {
					work_none(u);
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
				cerr << str(format("check %||/%|| -> %||\n") % L % M % (r+1));
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

		/*
		int get_improvment_cost(Improv const& im) {
			// zero mean immposible to build
			//im.get_base_cost()
		}*/

		bool build_road(Unit &u, bool exec=1) {
			/* Build road on land terrain square (cost ~ 1.5 turns)
			 */

			// unit checks
			if (!compatible(u.get_travel(), LAND)) {
				throw Error("must be land based unit");
			}

			Terr& t = get_terr(u);

			// terrain checks
			if (!compatible(t.get_travel(), LAND)) {
				throw Error("can build on land only");
			}
			if (t.has(PhysRoad)) {
				throw Error("road already exists");
			}

			// execute
			if (exec)  {
				if (run_map_task(u, t.get_roughness())) {
					t.add(PhysRoad);
					return 1;
				}
				return 0;
			}
			return -1; // unspecified
		}


		void record_action() {
			action_count += 1;
		}


		int get_transport_space(Terr const& dest, Player const& player) const {
			int space = 0;
			for (auto& p: dest.units) {
				if (p->get_player() == player) {
					space += p->get_space_left();
					if (p->transported) {
						space -= p->get_size();
					}
				}
			}
			return space;
		}


		bool has_transport(Terr const& dest, Unit const& u) const {
			return get_transport_space(dest, u.get_player()) >= u.get_size();
		}

		bool order_move(Unit &u, Dir::t const& dir, bool exec=1) {
			/* Move unit
			 */

			// static check
			if (dir == Dir::S) {
				throw Error("already here");
			}

			auto & orig = get_terr(u);
			auto dest_coords = get_coords(orig) + vec4dir(dir);

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
			if (has_defender(dest)) {
				if (get_defender(dest).get_player().id != u.get_player().id) {
					throw Error("destination occupied by enemy");
				}
			}

			// execute

			if (exec) {

				// cost to move = 1t

				auto time_cost = dest.get_movement_cost() / u.get_speed();

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
				else {
					record_action();
					return 0; // try next turn
				}
			}
			else {
				return -1; // unspecified
			}
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
			u.terr->sub(u);
			u.terr = nullptr;
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


		bool plow_field(Unit& u, bool exec=1) {

			// unit check
			if (!compatible(u.get_travel(), LAND)) {
				throw Error("need land based unit to perform this action");
			}

			auto& t = get_terr(u);

			// terrain checks
			if (!compatible(t.get_travel(), LAND)) {
				throw Error("can build on land only");
			}

			if (t.has(PhysPlow)) {
				throw Error("improvment already exists");
			}

			// execute
			if (exec) {
				if (run_map_task(u, t.get_roughness())) {
					t.add(PhysPlow);
					return 1;
				}
				return 0;
			}
			return -1;
		}

		bool clear_forest(Unit const& u) {
			//uint8 time_cost = TIME_UNIT * 3;  // 3t


			return 0;
		}


		bool plant_forest() {

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


		bool colonize(Unit &u, string const& name, bool exec=1) {

			auto& t = get_terr(u);

			if (t.has_colony()) {
				throw Error("already colonized");
			}

			if (exec) {
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

					for (auto dir: ALL_DIRS) {
						auto dest = get_coords(t) + vec4dir(dir);
						if (in_bounds(dest)) {
							c.add_field(Field(get_terr(dest)));
						}
					}


					c.add(ItemFood, 10);

					return 1;  // ok
				}
				return 0;  // try next turn
			}
			return -1;
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

		Player const* get_control(Terr const& terr) const {
			if (terr.units.size()) {
				return &terr.units.at(0)->get_player();
			}
			return nullptr;
		}

		Player * get_control(Terr const& terr) {
			if (terr.units.size()) {
				return &terr.units.at(0)->get_player();
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


		bool has_control(Terr const& terr, Player const& p) const {
			Player const* c = get_control(terr);
			return c and *c == p;
		}


		// order
		void set_proditem_field(Player const& p, Terr & terr, int field_id, Item const& item) {
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

		Field const& get_field(Terr const& terr, int const& field_id, Player const& p) const {
			if (has_control(terr, p)) {
				return get_field(terr, field_id);
			}
			else {
				throw Error("this player has no control here");
			}
		}

		Field & get_field(Terr & terr, int const& field_id, Player const& p) {
			if (has_control(terr, p)) {
				return get_field(terr, field_id);
			}
			else {
				throw Error("this player has no control here");
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

		Build & get_build(Terr & terr, int build_id, Player const& p) {
			if (has_control(terr, p)) {
				return get_build(terr, build_id);
			}
			else {
				throw Error("this player has no control here");
			}
		}



		void work_field(int field_id, Unit & u) {
			Terr & terr = get_terr(u);
			auto& f = get_field(terr, field_id, u.get_player());
			work_workplace(f, u);
		}

		void work_build(int build_id, Unit & u) {
			Terr & terr = get_terr(u);
			auto& f = get_build(terr, build_id, u.get_player());
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

	template <>	inline Players& Env::get_cont<Player>() {
		return players;
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

	template <>	inline Players const& Env::get_cont<Player>() const {
		return players;
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