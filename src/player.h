#ifndef PLAYER_H
#define PLAYER_H

#include "objs.h"

namespace col{



	struct Player {
		using Id = uint32;

		Id id;
		string name;
		Color color;
		uint8 flag_id;

		Player(Id const& id, string const& name="", Color const& color={0,0,0}, uint32 const& flag_id=0):
			id(id),
			name(name),
			color(color),
			flag_id(flag_id)
		{}


		Player() = default;
		Player(Player&& p) = default;

		Player(Player const&) = delete;

		template<class A>
		void serialize(A & ar, uint const& version) {
			ar & id;
			ar & name;
			ar & color;
			ar & flag_id;
		}

		Player& set_name(string const& n) { name = n; return *this; }
		Player& set_color(Color const& c) { color = c; return *this; }
		Player& set_flag_id(uint8 const& fi) { flag_id = fi; return *this; }

		uint8 const& get_flag_id() const { return flag_id; }
		Color const& get_color() const { return color; }


	};

	inline bool operator==(Player const& self, Player const& other) {
		return self.id == other.id;
	}

	ostream& operator<<(ostream &out, const Player &obj);



}

#endif
