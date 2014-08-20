#ifndef PLAYER43567_H
#define PLAYER43567_H

#include "objs.h"
#include "meta.hpp"

namespace col{

	struct User;

	struct Player {
		using Id = uint32;

		Id id;
		string name;
		Color color;
		uint8 flag_id;

		User *user{nullptr};

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

		User * get_user() const { return user; }
		Player & set_user(User * user) { this->user = user; return *this; }



	};




	template<>
	struct Id<Player>{
		using type = uint32;
	};




	inline bool operator==(Player const& self, Player const& other) {
		return self.id == other.id;
	}

	inline bool operator!=(Player const& self, Player const& other) {
		return self.id != other.id;
	}

	ostream& operator<<(ostream &out, const Player &obj);



}

#endif
