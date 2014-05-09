#ifndef ACTION2_H
#define ACTION2_H

#include <sstream>
#include "env.h"

namespace col {


	using Game = Env;

	struct Action {

		Player::Id pid;

		Action(Player::Id const& pid): pid(pid) {

		}

		virtual void exec(Game &game, bool exec=1) const = 0;
		virtual bool unsafe_eq(Action const& a) const = 0;

		bool is_allowed(Game &game) const {
			try {
				this->exec(game, 0);
			}
			catch (Error const& e) {
				return 0;
			}
			return 1;
		}

		bool operator==(Action const& other) const {
			if (typeid(*this) == typeid(other)) {
				// same type so can call unsafe_eq
				return unsafe_eq(other);
			}
			else {
				return false;
			}
		}

		virtual Action* copy() = 0;

		virtual ostream& dump(ostream& out) const = 0;

	};

	inline
	string to_string(Action const& a) {
		std::stringstream s;
		a.dump(s);
		return s.str();
	}

	//struct BuildColony: Action {
	// BuildRoad
	// PlowFields
	// ClearForest
	// Assign
	// Sell
	// Load


	struct OrderMove: Action {
		Unit::Id uid;
		Dir::t dir;

		OrderMove(Player::Id const& pid, Unit::Id const& uid, Dir::t const& dir):
			Action(pid),
			uid(uid), dir(dir)
		{}


		bool unsafe_eq(Action const& a) const {
			auto b = static_cast<OrderMove const&>(a);
			return pid == a.pid and uid == b.uid and dir == b.dir;
		}

		Action* copy() {
			return new OrderMove(*this);
		}

		void exec(Game &game, bool exec=1) const {
			game.order_move(
				game.get<Unit>(uid),
				dir,
				exec
			);
		}

		virtual ostream& dump(ostream& out) const {
			out << "OrderMove("<<pid<<","<<uid<<","<<int(dir)<<")";
		}


	};


	struct OrderAttack: Action {
		Unit::Id uid;
		Dir::t dir;

		OrderAttack(Player::Id const& pid, Unit::Id const& uid, Dir::t const& dir):
			Action(pid),
			uid(uid), dir(dir)
		{}


		bool unsafe_eq(Action const& a) const {
			auto b = static_cast<OrderAttack const&>(a);
			return pid == a.pid and uid == b.uid and dir == b.dir;
		}

		Action* copy() {
			return new OrderAttack(*this);
		}

		void exec(Game &game, bool exec=1) const {
			game.order_attack(
				game.get<Unit>(uid),
				dir,
				exec
			);
		}

		virtual ostream& dump(ostream& out) const {
			out << "OrderAttack("<<pid<<","<<uid<<","<<int(dir)<<")";
		}


	};

	struct Ready: Action {

		Ready(Player::Id const& pid): Action(pid) {}

		bool unsafe_eq(Action const& a) const {
			auto b = static_cast<Ready const&>(a);
			return pid == a.pid;
		}

		Action* copy() {
			return new Ready(*this);
		}

		void exec(Game &game, bool exec=1) const {
			game.ready(game.get<Player>(pid), exec);
		}

		virtual ostream& dump(ostream& out) const {
			out << "Ready("<<pid<<")";
		}

	};


	struct Start: Action {
		Start(Player::Id const& pid): Action(pid) {}

		void exec(Game &game, bool exec=1) const {
			game.start(game.get<Player>(pid), exec);
		}

		bool unsafe_eq(Action const& a) const {
			auto b = static_cast<Start const&>(a);
			return pid == a.pid;
		}

		Action* copy() {
			return new Start(*this);
		}

		virtual ostream& dump(ostream& out) const {
			out << "Start("<<pid<<")";
		}

	};


	inline
	ostream& operator<<(ostream& o, Action const& a) {
		a.dump(o);
	}


}
#endif
