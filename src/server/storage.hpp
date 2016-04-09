#pragma once

#include "base.hpp"

namespace col {
	
	/*
	struct Slot {
		virtual void add(Item const& item, Amount num) { assert(0); }
		virtual void sub(Item const& item, Amount num) { assert(0); }
		virtual void set(Item const& item, Amount num) { assert(0); }
		virtual Amount get(Item const& item) const { assert(0); }
	};*/


	struct Register {
		struct Ca {
			Item item;
			Amount amount;
			Ca() = default;
			Ca(Item item, Amount amount): item(item), amount(amount) {}
		};
		
		using Cas = vector<Ca>;
		
		Cas cas;		
		
		Amount & operator[](Item const& item) {
			return find_mod(item).amount;
		}
		
		Amount operator[](Item const& item) const {
			return find_get(item).amount;
		}
		
		Ca & find_mod(Item const& item) {
			for (auto & ca: cas) {
				if (ca.item == item) {
					return ca;
				}
			}
			cas.push_back(Ca(item, 0));
			return cas[cas.size() - 1];
		}
		
		void add(Item const& item, Amount num) {
			find_mod(item).amount += num;
		}

		void sub(Item const& item, Amount num) {
			find_mod(item).amount -= num;
		}

		void set(Item const& item, Amount num) {
			find_mod(item).amount = num;
		}

		Ca find_get(Item const& item) const {
			for (auto & ca: cas) {
				if (ca.item == item) {
					return ca;
				}
			}
			return Ca(item, 0);
		}

		Amount get(Item const& item) const {
			return find_get(item).amount;			
		}
		
		Amount total() const {
			Amount am = 0;
			for (auto & ca: cas) {
				am += ca.amount;
			}
			return am;
		}
		
		void clear() {
			cas.clear();
		}
		
		Cas::iterator begin() {
			return cas.begin();
		}
		
		Cas::iterator end() {
			return cas.end();
		}
		
		Cas::size_type size() const {
			return cas.size();
		}
		
	};

	struct Storage: Register {

		
		Storage() = default;

		//Amount max_storage{0};
		//Storage(Amount max_storage): max_storage(max_storage) {}
		//void set_max_storage(Amount max_storage) { max_storage = max_storage; }
		//Amount get_max_storage() { return max_storage; }


		void add(Item const& item, Amount num) {
			Amount & has = (*this)[item];
			has += num;			
		}

		void sub(Item const& item, Amount num) {
			Amount & has = (*this)[item];			
			if (num <= has) {
				has -= num;
			}
			else {
				GFAIL("storage.sub: out of item");				
			}
		}

	};

	using Store = Storage;

}

