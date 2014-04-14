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
		virtual bool eq(Action & a) = 0;

		bool operator==(Action &a) {
			try {
				return this->eq(a);
			}
			catch (std::bad_cast) {
				return false;
			}
		}

		virtual Action* copy() = 0;
		virtual ostream& dump(ostream& out) = 0;

		
	};


	struct AttackMove: Action {
		Unit::Id iid;
		Dir dir;
		AttackMove(Player::Id const& pid, Unit::Id const& iid, Dir const& dir): Action(pid) {
			this->iid = iid;
			this->dir = dir;
		}
		
		bool eq(Action &a)  {
			AttackMove& b = dynamic_cast<AttackMove&>(a);
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
		
		bool eq(Action & a)  {
			EndTurn& b = dynamic_cast<EndTurn&>(a);
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

		bool eq(Action & a)  {
			Start& b = dynamic_cast<Start&>(a);
			return pid = a.pid;
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
