#include "ai-env-helpers.h"



namespace col{

	namespace misc{

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


		Items get_proditems(Env const& env, Field const& f) {
			// return -- list of items avaliable for production (with non zero yields))
			Items rs;
			for (auto& it: get_all_items(env)) {
				if (f.get_terr().get_yield(it, false)) {
					rs.push_back(it);
				}
			}
			return rs;
		}
		
		int get_yield(Env const& env, Build const& b) {
			int num = 0;
			for (auto& u: b.units) {				
				int base = env.get_yield(*u, b.get_proditem());
				int prodnum = b.get_prod() * base;
				num += prodnum;
			}
			return num;			
		}
		
		Unit const* get_unassigned_unit(Env const& env, Terr const& terr) {			
			for (auto& u: terr.units) {
				if (!u->is_working()) return u;
			}
			return nullptr;
		}
		
		
		
		Unit const* get_next_to_move(Env const& env, Nation const& pl, Unit const* cur) {
			if (cur and owned(*cur, pl) and to_move(*cur)) {
				return cur;
			}
			
			for (auto& p: env.get_cont<Unit>()) {
				Unit const& u = p.second;
				if (owned(*cur, pl) and to_move(*cur)) {
					return &u;
				}				
			}
			
			return nullptr;
		}

		

		Unit * find_unit(Env & env, Unit * cur, function<bool(Unit const&)> func) {
		
			if (cur and func(*cur)) {
				return cur;
			}
			
			for (auto& p: env.get_cont<Unit>()) {
				Unit & u = p.second;
				
				if (func(u)) {
					return &u;
				}				
			}
			
			return nullptr;
		}
		
		Unit * Memory::get_next_unit(Env & env, Nation const& p, Unit * cur) const {
			return find_unit(env, cur, [this,&p](Unit const& u) -> bool{
				return owned(u, p) and to_move(u) and get_order(u.id).code != ' ';
			});
		}

		bool Memory::has_next_unit(Env const& env, Nation const& p) const {
			return find_unit(const_cast<Env&>(env), nullptr, [this,&p](Unit const& u) -> bool{
				return owned(u, p) and to_move(u) and get_order(u.id).code != ' ';
			});
		}



		
	}
	
}
