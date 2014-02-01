#ifndef OBJS_H
#define OBJS_H

#include <boost/format.hpp>
#include "col.h"
#include "iomm.h"

namespace col {
	using namespace iomm;

	/*[[[cog
	import meta;
	meta.make_rwtype('Icon',
		'uint32 id',
		'uint16 type',
		'uint16 x',
		'uint16 y',
	)
	meta.make_rwtype('Player',
		'uint32 id',
		'string name',	
	)
	]]]*/
struct Icon {
	uint32 id;
	uint16 type;
	uint16 x;
	uint16 y;

	Icon() {}

	Icon(uint32 id_, uint16 type_, uint16 x_, uint16 y_): id(id_), type(type_), x(x_), y(y_) {}

}; //Icon

inline Icon read_icon(istream &f) {
	Icon icon;
	icon.id = read_uint32(f);
	icon.type = read_uint16(f);
	icon.x = read_uint16(f);
	icon.y = read_uint16(f);
	return icon;
}

inline void write_icon(ostream &f, const Icon &icon) {
	write_uint32(f, icon.id);
	write_uint16(f, icon.type);
	write_uint16(f, icon.x);
	write_uint16(f, icon.y);
}
inline ostream& operator<<(ostream &out, const Icon &obj)	{	cout << "Icon(" << "id"<<"="<<obj.id<<','<<"type"<<"="<<obj.type<<','<<"x"<<"="<<obj.x<<','<<"y"<<"="<<obj.y << ")";}

struct Player {
	uint32 id;
	string name;

	Player() {}

	Player(uint32 id_, string name_): id(id_), name(name_) {}

}; //Player

inline Player read_player(istream &f) {
	Player player;
	player.id = read_uint32(f);
	player.name = read_string(f);
	return player;
}

inline void write_player(ostream &f, const Player &player) {
	write_uint32(f, player.id);
	write_string(f, player.name);
}
inline ostream& operator<<(ostream &out, const Player &obj)	{	cout << "Player(" << "id"<<"="<<obj.id<<','<<"name"<<"="<<obj.name << ")";}

	//[[[end]]]
	
}
#endif