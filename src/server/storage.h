#ifndef STORAGE2934854_H
#define STORAGE2934854_H


#include "error.h"
#include "base.h"

namespace col {

	struct Storage{

		map<Item, Amount> storage;     // lame map
		Amount max_storage{0};

		Storage() = default;
		Storage(Amount max_storage): max_storage(max_storage) {}

		void set_max_storage(Amount max_storage) { max_storage = max_storage; }
		Amount get_max_storage() { return max_storage; }


		void add(Item const& item, Amount num) {
			auto key = item;
			if (storage.count(key)) {
				storage[key] += num;
			}
			else {
				storage.insert({key, num});
			}
		}

		void sub(Item const& item, Amount num) {
			auto key = item;
			if (num <= get(item)) {
				storage[key] -= num;
			}
			else {
				throw Error("out of item");
			}
		}

		void set(Item const& item, Amount num) {
			if (num == 0) {
				storage.erase(item);
			}
			else {
				storage[item] = num;
			}
		}


		Amount get(Item const& item) const {
			auto key = item;
			if (storage.count(key)) {
				return storage.at(key);
			}
			else {
				return 0;
			}
		}

		bool has(Item const& item, Amount num) const {
			return get(item) >= num;
		}

	};

}


#endif
