#include "env.h"


namespace col {


	Env::State const
		Env::StateEdit = 0,
		Env::StatePlay = 1,
		Env::StateExit = 2;


	int Env::get_movement_cost(Terr const& dest, Terr const& orig, Travel const& travel) const {

		if (travel == TravelSea) {
			if (dest.alt == AltSea) {
				return TIME_UNIT;
			}
			else if (dest.has_colony() and !orig.has_colony()) {
				return TIME_UNIT;
			}
			else {
				return -1;
			}
		}
		else if (travel == TravelLand) {
			if (dest.alt != AltSea) {
				if (dest.has_phys(PhysRoad) and orig.has_phys(PhysRoad)) {
					return TIME_UNIT / 3;
				}
				else if (dest.has_phys(PhysRiver) and orig.has_phys(PhysRiver)) {
					return TIME_UNIT / 3;
				}
				else {
					return TIME_UNIT * get_roughness(dest);
				}
			}
			else {
				return -1;
			}
		}
		else {
			throw Critical("Unknown travel type");
		}
	}

	int Env::get_roughness(Terr const& t) const {
		// return -- 1 or 2
		auto alt = t.get_alt();
		auto biome = t.get_biome();

		if (alt == AltHill or alt == AltMountain) {
			return 2;
		}
		else if (biome == BiomeSwamp or biome == BiomeMarsh or biome == BiomeArctic) {
			return 2;
		}
		else if (t.has_phys(PhysForest) or t.has_phys(PhysRiver)) {
			return 2;
		}
		else {
			return 1;
		}

	}

	void Env::produce(Store & st, Workplace & wp) {
		auto const& proditem = wp.get_proditem();
		auto const& consitem = wp.get_consitem();

		Amount food_sup = st.get(ItemFood);

		if (proditem != ItemNone) {
			// from workers
			ProdCons nom;
			for (auto * u: wp.get_units()) {
				if (u->time_left == TIME_UNIT) {
					auto uprod = wp.get_prod(proditem, u->get_prod(proditem));
					auto ucons = wp.get_cons(proditem, u->get_prod(proditem));
					
					
					if (proditem == ItemFood or proditem == ItemFish) {
						food_sup += uprod;
					}
					
					if (food_sup >= 2) {
						nom.prod += uprod;
						nom.cons += ucons;
							
						u->time_left = 0;
						food_sup -= 2;
					}
					else {
						// TODO: message: workers strike due to lack of food
					}
				}
			}

			// limited by resources
			ProdCons lim;
			if (nom.cons) {
				assert(consitem != ItemNone);
				lim.cons = std::min(nom.cons, st.get(consitem));
				lim.prod = (nom.prod * lim.cons) / nom.cons;
			}
			else {
				lim = nom;
			}

			// apply prod
			if (wp.task) {
				assert(proditem == ItemHammers);
				wp.task.add(lim.prod);
			}
			else {
				if (proditem == ItemFish) {
					st.add(ItemFood, lim.prod);
				}
				else {
					st.add(proditem, lim.prod);
				}
			}
			
			// apply cons
			if (lim.cons) {
				assert(consitem != ItemNone);
				st.sub(consitem, lim.cons);
			}
			
			st.set(ItemFood, food_sup);
			
		}
	} // produce




	void Env::resolve_construction(Terr & terr, Workplace & f) {
		auto & c = terr.get_colony();

		if (f.task and f.task.get() >= f.task.cap()) {

			switch (f.task.what->get_class()) {
				case Class::UnitType:
				{
					UnitType const& ut = *static_cast<UnitType const*>(f.task.what);

					auto nat = get_control(terr);

					if (nat == nullptr) {
						if (verbose) {
							print("unit cannot be created because lack of control; pos=%||\n", get_coords(terr));
						}
					}
					else {
						// TODO: may need to equip with tools

						init(ut, terr, *nat);
						f.task.reset();
					}

					break;
				}
				case Class::BuildType:
				{
					BuildType const& bt = *static_cast<BuildType const*>(f.task.what);
					auto *b = c.select_place_for(bt);
					if (b) {

						// TODO: may need to equip with tools

						b->morph(bt);
						f.task.reset();

						// TODO: add message info

						// special effects
						// warehouse constructed
						if (bt.id == 16) {
							c.max_storage += 100;
						}


					}
					else {
						// TODO: cannot build - add message to user

					}
					break;
				}
				default:
					assert(0);
					break;
			}

		}


	}


	void Env::init_colony(Terr & t) {
		auto& c = create<Colony>();
		init(t, c);

		BuildType::Id const tree1 = 45;
		BuildType::Id const tree2 = 44;
		BuildType::Id const tree3 = 43;
		BuildType::Id const coast = 46;
		BuildType::Id const carpenters_shop = 36;
		BuildType::Id const fence = 17;
		BuildType::Id const church = 38;
		BuildType::Id const town_hall = 10;

		// init buildings
		set_build(c, 0, carpenters_shop);
		set_build(c, 1, tree1);
		set_build(c, 2, tree3);
		set_build(c, 3, tree1);
		set_build(c, 4, tree1);

		set_build(c, 5, tree1);
		set_build(c, 6, tree1);
		set_build(c, 7, tree1);
		set_build(c, 8, tree1);
		set_build(c, 9, tree2);

		set_build(c, 10, tree2);
		set_build(c, 11, tree2);
		set_build(c, 12, town_hall);
		set_build(c, 13, coast);
		set_build(c, 14, fence);

		// init fields
		for (auto dir: ALL_DIRS) {
			auto dest = get_coords(t) + vec4dir(dir);
			if (in_bounds(dest)) {
				c.add_field(Field(get_terr(dest)));
			}
		}

		//notify_effect(t, inter::init_colony(get_id(t)));
	}


	bool Env::can_make(Build const& build, BuildType const& bt) const {
		if (bt.get_cost() > 0) {
			auto facttype_id = build.get_type().id;
			switch (facttype_id) {
				case BuildCarpentersShop:
				case BuildLumberMill:
					return true;
					break;
			}
		}
		return false;
	}


	bool Env::can_make(Build const& build, UnitType const& ut) const {
		if (ut.get_cost() > 0) {
			auto facttype_id = build.get_type().id;
			switch (facttype_id) {
				case BuildCarpentersShop:
				case BuildLumberMill:
					if (ut.get_travel() == TravelLand) {
						return true;
					}
					break;
				case BuildShipyard:
					if (ut.get_travel() == TravelSea) {
						return true;
					}
					break;
			}
		}
		return false;
	}

	void Env::equip(Unit & u, UnitType & ut) {
		if (u.get_nation() != get_current_nation()) {
			throw Error("not your unit");
		}

		Terr & t = get_terr(u);
		Store & st = get_store(t);

		if (u.get_base() != ut.get_base()) {
			throw Error("incompatible types");
		}

		// recover
		st.add(u.get_item1(), u.get_num1());
		st.add(u.get_item2(), u.get_num2());

		if (    st.get(ut.get_item1()) >= ut.get_num1()
			and st.get(ut.get_item2()) >= ut.get_num2()   )
		{

			//notify_effect(t, inter::add_item(get_id(t), u.get_item1(), u.get_num1()));
			//notify_effect(t, inter::add_item(get_id(t), u.get_item2(), u.get_num2()));

			// consume
			sub_item(t, ut.get_item1(), ut.get_num1());
			sub_item(t, ut.get_item2(), ut.get_num2());

			// change type
			morph_unit(u, ut);
		}
		else {
			// rollback
			st.sub(u.get_item1(), u.get_num1());
			st.sub(u.get_item2(), u.get_num2());

			throw Error("not enough equipment");
		}
	}
}


