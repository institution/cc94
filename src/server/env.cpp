#include "env.hpp"


namespace col {


	Env::State const
		Env::StateEdit = 0,
		Env::StatePlay = 1,
		Env::StateExit = 2;

	/// Speciality name
	Prof get_prof_by_item(Item item)
	{
		switch (item) {
			case ItemNone: return ProfNone;
			case ItemFood: return ProfFarmer;
			case ItemFish: return ProfFisherman;
			case ItemSugar: return ProfSugarPlanter;
			case ItemTobacco: return ProfTobaccoPlanter;
			case ItemCotton: return ProfCottonPlanter;
			case ItemFurs: return ProfFurTrapper;
			case ItemLumber: return ProfLumberjack;
			case ItemOre: return ProfOreMiner;
			case ItemSilver: return ProfSilverMiner;
			case ItemHorses: return ProfNone;
			case ItemRum: return ProfDistiller;
			case ItemCigars: return ProfTobaconist;
			case ItemCloth: return ProfWeaver;
			case ItemCoats: return ProfFurTrader;
			case ItemTradeGoods: return ProfNone;
			case ItemTools: return ProfBlacksmith;
			case ItemMuskets: return ProfGunsmith;
			case ItemCannons: return ProfGunsmith;
			case ItemHammers: return ProfCarpenter;
			case ItemCross: return ProfPreacher;
			case ItemBell: return ProfStatesman;
		}
		return ProfNone;
	}

	/// What 'unit is doing
	Prof get_unit_occupation(Unit const& u)
	{
		
		if (u.is_working()) {
			return get_prof_by_item(u.get_workplace().get_proditem());
		}
		else {
			if (u.get_travel() == TravelLand) {
				auto const& t = u.get_type();
				if (t.get_item1() == ItemTools) {
					return ProfPioneer;
				}
				else if (t.get_item1() == ItemMuskets) {
					return ProfSoldier;
				}
				else if (t.get_item2() == ItemHorses) {
					return ProfScout;
				}
			}
		}
		return ProfNone;
	}

	/// Is 'unit expert in production of 'item
	bool is_expert(Unit const& unit, Item const& item) {
		auto req = get_prof_by_item(item);
		return req != ProfNone and req == unit.get_prof();		
	}


	

	/// How much of 'item can be produced by 'unit in 'build
	Amount Env::get_prod(Build const& build, Unit const& unit, Item const& item) const {
		if (build.get_proditem() == item) {
			return build.get_prod() * (is_expert(unit, item) + 1);
		}
		else {
			return 0;
		}		
	}
	
	/// How much of raw materials needed to produce 'item will be consumed by 'unit in 'build
	Amount Env::get_cons(Build const& build, Unit const& unit, Item const& item) const {
		if (build.get_proditem() == item) {
			return build.get_cons() * (is_expert(unit, item) + 1);
		}
		else {
			return 0;
		}		
	}

	/// How much of 'item can be produced on 'terr by Standard Colonist
	Amount Env::get_base_prod(Terr const& terr, Item const& item) const {
		/* 		 
		    Why this is so complicated; games are supposed to have easy to understand rules
		  ballance should be achieved otherwise (negative feedback possibly)
		 TODO: simplify in AC spirit; yields should be devised from climate parameters; 			
		*/
		
		int ret = 0;
		int forest = terr.has_phys(PhysForest);
		int plow = terr.has_phys(PhysPlow) * 2;
		int river = terr.has_phys(PhysRiver);
		auto alt = terr.get_alt();
		auto biome = terr.get_biome();
		
		switch(item) {
			case ItemFish:
				if (alt == AltSea) {
					ret = 4;					
				}
				break;
			case ItemLumber:
				if (alt >= AltFlat) {
					if (forest) {
						switch (biome) {
							case BiomePlains:
							case BiomeSavannah:
								ret = 6;
								break;
							case BiomeTundra:
							case BiomePrairie:
							case BiomeGrassland:
							case BiomeSwamp:
							case BiomeMarsh:
								ret = 4;
								break;
							case BiomeDesert:
								ret = 2;
								break;
						}
					}					
				}
				break;
			case ItemOre:
				if (alt > AltFlat) {
					ret = 4;
				}
				else if (alt == AltFlat) {
					switch (biome) {
						case BiomeTundra:
						case BiomeSwamp:
						case BiomeMarsh:
						case BiomeDesert:
							ret = 2 - forest;
							break;
					}
				}
				break;
			case ItemFurs:
				if (alt == AltFlat) {
					if (forest) {
						switch (biome) {
							case BiomeTundra:
							case BiomePlains:
								ret = 3;
								break;
							case BiomePrairie:
							case BiomeGrassland:
							case BiomeMarsh:
							case BiomeSavannah:
							case BiomeDesert:
							case BiomeArctic:
								ret = 2;
								break;
							case BiomeSwamp:
								ret = 1;
								break;
						}
					}
				}
				break;
			case ItemFood:
				if (alt == AltSea) {
					ret = 4;
				}
				else if (alt == AltFlat) {
					switch (biome) {
						case BiomePlains:
							ret = 5 - forest + plow;
							break;
						case BiomeSavannah:
							ret = 4 - forest + plow;
							break;
						case BiomeTundra:
						case BiomePrairie:
						case BiomeGrassland:
						case BiomeMarsh:
						case BiomeSwamp:
							ret = 3 - forest + plow;
							break;
						case BiomeDesert:
							ret = 2 - forest + plow;
							break;
					}
				}
				else if (alt == AltHill) {
					ret = 2 + plow;
				}
				break;
			case ItemCotton:
				if (alt == AltFlat) {
					switch (biome) {
						case BiomePrairie:
							ret = 3 - forest + plow;
							break;
						case BiomePlains:
							ret = 2 - forest + plow;
							break;
						case BiomeDesert:
							ret = 1 - forest + plow;
							break;
					}
				}
				break;
			case ItemTobacco:
				if (alt == AltFlat) {
					switch (biome) {
						case BiomeGrassland:
							ret = 3 - forest + plow;
							break;
						case BiomeMarsh:
							ret = 2 - forest + plow;
							break;
					}
				}
				break;
			case ItemSugar:
				if (alt == AltFlat) {
					switch (biome) {
						case BiomeSavannah:
							ret = 3 - forest + plow;
							break;
						case BiomeSwamp:
							ret = 2 - forest + plow;
							break;
					}
				}
				break;
			case ItemSilver:
				if (alt == AltMountain) {
					ret = 1;
				}
				break;
		}
		return ret;	
	}
	
	
	/// How much of 'item can be produced by 'unit on 'terr
	Amount Env::get_prod(Terr const& terr, Unit const& unit, Item const& item) const {
		int expert = is_expert(unit, item) + 1;
		return get_base_prod(terr, item) * expert;
	}




	

	
	/// movment cost from to 'dest from 'orig for particular 'unit (TimeInf -- movment impossible)
	Time Env::get_move_cost(Terr const& dest, Terr const& orig, Unit const& u) 
	{	
		if (compatible(u.get_travel(), dest.get_travel())) 
		{
			return get_movement_cost(dest, orig, u.get_travel()) / u.get_speed();	
		}
		else {
			return TimeInf;
		}
	}

	/// movment cost for specific travel mode
	Time Env::get_movement_cost(Terr const& dest, Terr const& orig, Travel const& travel) const {

		if (travel == TravelSea) {
			if (dest.alt == AltSea) {
				return TimeUnit;
			}
			else if (dest.has_colony() and !orig.has_colony()) {
				return TimeUnit;
			}
			else {
				return TimeInf;
			}
		}
		else if (travel == TravelLand) {
			if (dest.alt != AltSea) {
				if (dest.has_phys(PhysRoad) and orig.has_phys(PhysRoad)) {
					return TimeUnit / 3;
				}
				else if (dest.has_phys(PhysRiver) and orig.has_phys(PhysRiver)) {
					return TimeUnit / 3;
				}
				else {
					return TimeUnit * get_roughness(dest);
				}
			}
			else {
				return TimeInf;
			}
		}
		else {
			throw Error("Unknown travel type");
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


	void Env::resolve_builds(Terr & terr) {

		assert(terr.has_colony());
		
		auto & c = terr.get_colony();

		for (auto & b: c.builds) {
			if (b.type) {
				switch (b.get_type_id()) {
					case BuildSchoolhouse:						
					case BuildCollege:						
					case BuildUniversity:
						resolve_teaching(get_store(c), b, terr);
						break;						
				}
			}
		}
	}


	void Env::resolve_teaching(Store & st, Workplace & b, Terr & t)
	{
		auto food_sup = st.get(ItemFood);
		
		for (auto * teacher: b.units) {
			auto tprof = teacher->get_prof();

			// may teach?
			if (tprof != ProfNone
			and food_sup >= 2
			and teacher->time_left == TimeUnit)
			{
				// search for student, TODO: look for most advanced student and teach him
				for (auto * student: t.units) {

					// may learn?
					if (student->get_prof() == ProfNone
					and get_unit_occupation(*student) == tprof)
					{
						// teaching cost time and food
						teacher->time_left = 0;
						food_sup -= 2;
						
						student->learn(tprof, 2);
						// done, next teacher
						break;
					}			
				}
			}		
		}
		
		st.set(ItemFood, food_sup);
	}
	

	void Env::produce(Store & st, Workplace & wp) {
		auto const& proditem = wp.get_proditem();
		auto const& consitem = wp.get_consitem();

		Amount food_sup = st.get(ItemFood);

		if (proditem != ItemNone) {
			// from workers
			ProdCons nom;
			for (auto * u: wp.get_units()) {
				if (u->time_left == TimeUnit) {
					auto uprod = wp.get_prod(*this, *u, proditem);
					auto ucons = wp.get_cons(*this, *u, proditem);
					
					
					if (proditem == ItemFood or proditem == ItemFish) {
						food_sup += uprod;
					}
					
					if (food_sup >= 2) {
						nom.prod += uprod;
						nom.cons += ucons;

						// working cost time and food
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
				case ClassUnitType:
				{
					UnitType const& ut = *static_cast<UnitType const*>(f.task.what);

					auto nat = get_control(terr);

					if (nat == ControlNone) {
						if (verbose) {
							print("unit cannot be created because lack of control; pos=%||\n", get_coords(terr));
						}
					}
					else {
						// TODO: may need to equip with tools
						put(terr, create<Unit>(ut, nat));
						f.task.reset();
					}

					break;
				}
				case ClassBuildType:
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
		put(t, c);

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
		for (auto dir: AllDirs) {
			auto dest = get_coords(t) + dir2vec(dir);
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

	void Env::equip(Unit & u, UnitType & ut) 
	{
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


