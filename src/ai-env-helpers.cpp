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

	}
	
}
