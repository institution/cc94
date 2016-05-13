#include "logic.hpp"

namespace col{
	namespace logic{

		UnitTypes equip_to_types(Env const& env, Unit const& u) {
			UnitTypes rs;
			for (auto& p: env.get_cont<UnitType>()) {
				UnitType const& ut = p.second;
				if (u.get_base() == ut.get_base()) {
					rs.push_back(&ut);
				}
			}
			return rs;
		}


		Items const ALL_ITEMS = {
			ItemFood,ItemSugar,ItemTobacco,ItemCotton,ItemFurs,
			ItemLumber,ItemOre,ItemSilver,ItemHorses,
			ItemRum,ItemCigars,ItemCloth,ItemCoats,
			ItemTradeGoods,ItemTools,ItemMuskets,
			ItemHammers,ItemCross,ItemBell
		};


		Items const& get_all_items(Env const& env) {
			return ALL_ITEMS;
		}

		
		Terr * get_idle_colony(Env const& env, Nation const& nation) {
			for (auto &p: env.get_cont<Colony>()) {
				
				auto& c = p.second;
				
				for (auto& b: c.builds) {
					auto& terr = c.get_terr();
					
					if (is_constructor(b) and !b.task and b.has_units() 
						and env.get_control(terr) == &nation
					) {
						// idle
						return &terr;
					}
				}				
			}
			return nullptr;
		}
		
		/*Unit * get_idle_unit(Env const& env, Nation const& nation) {
			for (auto &p: env.get_cont<Unit>()) {
				
				auto& u = p.second;
				
				if (env.get_control(u) and u.get_time_left() > 0) {
					return &u;
				}
							
			}
			return nullptr;
		}*/
		
		/*Terr * get_idle_unit(Env const& env, Nation const& nation) {
			for (auto &p: env.get_cont<Unit>()) {
				
				auto& unit = p.second;
				
				if 
				
				for (auto& b: c.builds) {
					auto& terr = *c.get_terr();
					
					if (is_constructor(b) and !b.task and b.has_units() 
						and env.get_control(terr) == &nation
					) {
						// idle
						return &terr;
					}
				}				
			}
			return nullptr;
		}*/

		
		Amount get_nominal_prod(Env const& env, Workplace const& fact, Item const& item) {
			Amount prod = 0;
			for (auto* u: fact.get_units()) {
				prod += fact.get_prod(env, *u, item);
			}
			return prod;
		}
		
		ProdCons get_nominal_prodcons(Env const& env, Workplace const& fact, Item const& item) {
			ProdCons nom;
			for (auto* u: fact.get_units()) {
				nom.prod += fact.get_prod(env, *u, item);
				nom.cons += fact.get_cons(env, *u, item);
			}
			return nom;
		}
		

		void sim_produce(Env const& env, Workplace const& wp, Register & p, Register & c) {
			auto const& proditem = wp.get_proditem();
			auto const& consitem = wp.get_consitem();

			if (proditem != ItemNone) {
				for (auto * u: wp.get_units()) {					
					auto nprod = wp.get_prod(env, *u, proditem);
					auto ncons = wp.get_cons(env, *u, proditem);
						
					p.add(proditem, nprod);
					c.add(consitem, ncons);
					c.add(ItemFood, 2);					
				}
			}
		} 
		
		// Fill ballance registers
		void fill_colony_regs(Env const& env, Terr const& terr, Register & p, Register & c) {
			auto const& cc = terr.get_colony();
						
			for (auto const* f: cc.get_workplaces()) {
				sim_produce(env, *f, p, c);
			}
		}
		





		Unit const* get_unassigned_unit(Env const& env, Terr const& terr) {
			for (auto& u: terr.units) {
				if (!u->is_working()) return u;
			}
			return nullptr;
		}






	}

}
