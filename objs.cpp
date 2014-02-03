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
			icon.player_id = read<uint32>(f);
			return icon;
		}

		template <>
		size_t write<Icon>(ostream &f, const Icon &icon) {
			size_t l = 0;
			l += write<uint32>(f, icon.id);
			l += write<uint16>(f, icon.type_id);
			l += write<uint16>(f, icon.x);
			l += write<uint16>(f, icon.y);
			l += write<uint32>(f, icon.player_id);
			return l;
		}

	}

	ostream& operator<<(ostream &out, const Icon &obj) {
		cout << "Icon(" << "id"<<"="<<obj.id<<','<<"type_id"<<"="<<obj.type_id<<','<<"x"<<"="<<obj.x<<','<<"y"<<"="<<obj.y<<','<<"player_id"<<"="<<obj.player_id << ")";
	}

	
	namespace io {

		template <>
		Color read<Color>(istream &f) {
			Color color;
			color.r = read<uint8>(f);
			color.g = read<uint8>(f);
			color.b = read<uint8>(f);
			return color;
		}

		template <>
		size_t write<Color>(ostream &f, const Color &color) {
			size_t l = 0;
			l += write<uint8>(f, color.r);
			l += write<uint8>(f, color.g);
			l += write<uint8>(f, color.b);
			return l;
		}
	}
	
	
	ostream& operator<<(ostream &out, const Color &color) {
		cout << "Color("<<color.r<<','<<color.g<<","<<color.b<<")";
	}
	
	namespace io {

		template <>
		Player read<Player>(istream &f) {
			Player player;
			player.id = read<uint32>(f);
			player.name = read<string>(f);
			player.color = read<Color>(f);
			return player;
		}

		template <>
		size_t write<Player>(ostream &f, const Player &player) {
			size_t l = 0;
			l += write<uint32>(f, player.id);
			l += write<string>(f, player.name);
			l += write<Color>(f, player.color);
			return l;
		}
	}

	ostream& operator<<(ostream &out, const Player &obj) {
		cout << "Player(" << "id"<<"="<<obj.id<<','<<"name"<<"="<<obj.name << ")";
	}

}