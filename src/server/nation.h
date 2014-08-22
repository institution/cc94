#ifndef PLAYER43567_H
#define PLAYER43567_H

#include "objs.h"
#include "meta.hpp"

namespace col{

	struct Player;

	struct Nation {
		using Id = uint32;

		Id id;
		string name;
		Color color;
		uint8 flag_id;

		Player *player{nullptr};

		Nation(Id const& id, string const& name="", Color const& color={0,0,0}, uint32 const& flag_id=0):
			id(id),
			name(name),
			color(color),
			flag_id(flag_id)
		{}


		Nation() = default;
		Nation(Nation&& p) = default;

		Nation(Nation const&) = delete;

		~Nation() {
			assert(player == nullptr);
		}

		template<class A>
		void serialize(A & ar, uint const& version) {
			ar & id;
			ar & name;
			ar & color;
			ar & flag_id;
		}

		Nation& set_name(string const& n) { name = n; return *this; }
		Nation& set_color(Color const& c) { color = c; return *this; }
		Nation& set_flag_id(uint8 const& fi) { flag_id = fi; return *this; }

		uint8 const& get_flag_id() const { return flag_id; }
		Color const& get_color() const { return color; }

		Player * get_player() const { return player; }
		Nation & set_player(Player * player) { this->player = player; return *this; }



	};




	template<>
	struct Id<Nation>{
		using type = uint32;
	};




	inline bool operator==(Nation const& self, Nation const& other) {
		return self.id == other.id;
	}

	inline bool operator!=(Nation const& self, Nation const& other) {
		return self.id != other.id;
	}

	ostream& operator<<(ostream &out, const Nation &obj);



}

#endif
