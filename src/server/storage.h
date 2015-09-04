#ifndef STORAGE2934854_H
#define STORAGE2934854_H


#include "error.h"
#include "base.h"

namespace col {
	
	/*
	struct Slot {
		virtual void add(Item const& item, Amount num) { assert(0); }
		virtual void sub(Item const& item, Amount num) { assert(0); }
		virtual void set(Item const& item, Amount num) { assert(0); }
		virtual Amount get(Item const& item) const { assert(0); }
	};*/

	struct Storage {

		std::unordered_map<Item, Amount> cargos;     // lame map

		Storage() = default;

		//Amount max_storage{0};
		//Storage(Amount max_storage): max_storage(max_storage) {}
		//void set_max_storage(Amount max_storage) { max_storage = max_storage; }
		//Amount get_max_storage() { return max_storage; }


		void add(Item const& item, Amount num) {
			auto key = item;
			if (cargos.count(key)) {
				cargos[key] += num;
			}
			else {
				cargos.insert({key, num});
			}
		}

		void sub(Item const& item, Amount num) {
			auto key = item;
			if (num <= get(item)) {
				cargos[key] -= num;
			}
			else {
				error("storage.sub: out of item");				
			}
		}

		void set(Item const& item, Amount num) {
			if (num == 0) {
				cargos.erase(item);
			}
			else {
				cargos[item] = num;
			}
		}


		Amount get(Item const& item) const {
			auto key = item;
			if (cargos.count(key)) {
				return cargos.at(key);
			}
			else {
				return 0;
			}
		}

	};

	using Store = Storage;

}


#endif
