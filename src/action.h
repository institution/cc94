#ifndef ACTION2_H
#define ACTION2_H

namespace col {

	
	using Game = Env;
	
	struct Action {
		
		Player::Id pid;

		Action(Player::Id const& pid) {
			this->pid = pid;
		}

		virtual void exec(Game &game) const = 0;
		virtual bool unsafe_eq(Action const& a) const = 0;

		bool operator==(Action const& other) const {
			if (typeid(this) == typeid(other)) {
				// same type so can call unsafe_eq
				return unsafe_eq(other);					
			}
			else {
				return false;
			}
		}

		virtual Action* copy() = 0;
		virtual ostream& dump(ostream& out) = 0;

		
	};

	
	struct OrderMove: Action {
		Unit::Id uid;
		Dir dir;
		
		OrderMove(Player::Id const& pid, Unit::Id const& uid, Dir const& dir): 
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
		
		virtual ostream& dump(ostream& out) {
			out << "OrderMove('" << pid << ")";
		}
		
		void exec(Game &game) const {
			game.order_move(
				game.get<Unit>(uid),
				dir
			);
		}
		
	};

	struct AttackMove: Action {
		Unit::Id iid;
		Dir dir;
		AttackMove(Player::Id const& pid, Unit::Id const& iid, Dir const& dir): Action(pid) {
			this->iid = iid;
			this->dir = dir;
		}
		
		bool unsafe_eq(Action const& a) const {
			auto b = static_cast<AttackMove const&>(a);
			return pid == a.pid and iid == b.iid and dir == b.dir;
		}
		
		Action* copy() {
			return new AttackMove(*this);
		}
		
		virtual ostream& dump(ostream& out) {
			out << "AttackMove('" << pid << ")";
		}
		
		void exec(Game &game) const {
			game.attack_move(pid, iid, dir);
		}
		
	};

	struct EndTurn: Action {
		
		EndTurn(Player::Id const& pid): Action(pid) {}
		
		bool unsafe_eq(Action const& a) const {
			auto b = static_cast<EndTurn const&>(a);
			return pid == a.pid;
		}
		
		Action* copy() {
			return new EndTurn(*this);
		}
		
		virtual ostream& dump(ostream& out) {
			out << "EndTurn('" << pid << ")";
		}
		
		void exec(Game &game) const {
			game.end_turn(pid);
		}
	};
	
	
	struct Start: Action {
		Start(Player::Id const& pid): Action(pid) {}
		
		void exec(Game &game) const {
			
		}		

		bool unsafe_eq(Action const& a) const {
			auto b = static_cast<Start const&>(a);
			return pid == a.pid;
		}
		
		Action* copy() {
			return new Start(*this);
		}
		
		virtual ostream& dump(ostream& out) {
			out << "Start('" << pid << "')";
		}
		
	};	

	
	
}
#endif
