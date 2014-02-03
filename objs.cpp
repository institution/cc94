#include "objs.h"

namespace col {
	
	namespace io{
		template <>
		Icon read<Icon>(istream &f) {
			Icon icon;
			icon.id = read<uint32>(f);
			icon.type_id = read<uint16>(f);
			icon.x = read<uint16>(f);
			icon.y = read<uint16>(f);
			return icon;
		}

		template <>
		size_t write<Icon>(ostream &f, const Icon &icon) {
			size_t l = 0;
			l += write<uint32>(f, icon.id);
			l += write<uint16>(f, icon.type_id);
			l += write<uint16>(f, icon.x);
			l += write<uint16>(f, icon.y);
			return l;
		}

	}

	ostream& operator<<(ostream &out, const Icon &obj) {
		cout << "Icon(" << "id"<<"="<<obj.id<<','<<"type_id"<<"="<<obj.type_id<<','<<"x"<<"="<<obj.x<<','<<"y"<<"="<<obj.y << ")";
	}

	namespace io {

		template <>
		Player read<Player>(istream &f) {
			Player player;
			player.id = read<uint32>(f);
			player.name = read<string>(f);
			return player;
		}

		template <>
		size_t write<Player>(ostream &f, const Player &player) {
			size_t l = 0;
			l += write<uint32>(f, player.id);
			l += write<string>(f, player.name);
			return l;
		}
	}

	ostream& operator<<(ostream &out, const Player &obj) {
		cout << "Player(" << "id"<<"="<<obj.id<<','<<"name"<<"="<<obj.name << ")";
	}


}