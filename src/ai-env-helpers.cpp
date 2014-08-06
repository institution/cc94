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
		
		bool can_move(Env const& env, Player const& p, Unit const& u) {
			return u.get_player() == p and u.get_time_left() and !u.is_working();
		}
		
		Unit const* get_next_to_move(Env const& env, Player const& pl, Unit const* cur) {
			if (cur and can_move(env, pl, *cur)) {
				return cur;
			}
			
			for (auto& p: env.get_cont<Unit>()) {
				Unit const& u = p.second;
				if (can_move(env, pl, u)) {
					return &u;
				}				
			}
			
			return nullptr;
		}
		
		
		Unit::Id get_next_to_move_id(Env const& env, Player const& pl, Unit::Id cur_id) {
			if (cur_id and can_move(env, pl, env.get<Unit>(cur_id))) {
				return cur_id;
			}
			
			for (auto& p: env.get_cont<Unit>()) {
				Unit const& u = p.second;
				if (can_move(env, pl, u)) {
					return u.id;
				}				
			}
			
			return 0;
		}
		
	}
	
}
