#include "logic.h"

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




		Amount get_nominal_prod(Workplace const& fact, Item const& item) {
			Amount prod = 0;
			for (auto* u: fact.get_units()) {
				prod += fact.get_prod(item, u->is_expert(item));
			}
			return prod;
		}

		ProdCons get_nominal_prodcons(Workplace const& fact, Item const& item) {
			ProdCons x;
			x.prod = 0;
			x.cons = 0;
			for (auto* u: fact.get_units()) {
				x.prod += fact.get_prod(item, u->is_expert(item));
				x.cons += fact.get_cons(item, u->is_expert(item));
			}
			return x;
		}

		Storage get_colony_reg(Colony const& c) {

			Storage reg;

			for (auto const* f: c.get_workplaces()) {
				auto& p = f->get_proditem();
				auto& c = f->get_consitem();
				auto x = get_nominal_prodcons(*f, p);

				if (p != ItemNone) reg.add(p, x.prod);
				if (c != ItemNone) reg.sub(c, x.cons);
			}

			return reg;
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


		Unit const* get_unassigned_unit(Env const& env, Terr const& terr) {
			for (auto& u: terr.units) {
				if (!u->is_working()) return u;
			}
			return nullptr;
		}


		/*
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
		*/

		Unit * Memory::get_next_to_order(Env & env, Nation const& p, Unit * cur) const {
			return const_cast<Unit*>(
				get_next_to_order(
					static_cast<Env const&>(env),
					p,
					static_cast<Unit const*>(cur)
				)
			);
		}

		Unit const* Memory::get_next_to_order(Env const& env, Nation const& p, Unit const* cur) const {
			return find_unit(env, cur, [this,&p](Unit const& u) -> bool{
				return owned(u, p) and to_move(u) and get_order(u.id).code == '-';
			});
		}

		Unit * Memory::get_next_to_repeat(Env & env, Nation const& p, Unit * cur) const {
			return const_cast<Unit*>(
				get_next_to_repeat(
					static_cast<Env const&>(env),
					p,
					static_cast<Unit const*>(cur)
				)
			);
		}

		Unit const* Memory::get_next_to_repeat(Env const& env, Nation const& p, Unit const* cur) const {
			return find_unit(env, cur, [this,&p](Unit const& u) -> bool{
				auto code = get_order(u.id).code;
				return owned(u, p) and to_move(u) and code != '-' and code != ' ';
			});
		}



		Unit const* find_unit(Env const& env, Unit const* cur, function<bool(Unit const&)> func) {
			if (cur and func(*cur)) {
				return cur;
			}

			for (auto& p: env.get_cont<Unit>()) {
				Unit const& u = p.second;

				if (func(u)) {
					return &u;
				}
			}

			return nullptr;
		}

		Unit * find_unit(Env & env, Unit * cur, function<bool(Unit const&)> func) {
			return const_cast<Unit*>(
				find_unit(
					static_cast<Env const&>(env),
					static_cast<Unit const*>(cur),
					func
				)
			);
		}



	}

}
