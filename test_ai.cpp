#include <iostream>
#include "ai.h"
#include "roll.h"
  

			
using namespace std;

struct OX{
	
	using PlayerId = char;
	
	// 012
	// 345
	// 678
	char bs[9];
	char player;
	int moves;
	
	OX() {
		for (auto &b: bs) {
			b = '.';
		}
		player = 'O';
		moves = 1;
	}
	
	void end_turn() {
		assert(moves = 0);
		
		if (player == 'O') {
			player = 'X';
		}
		else {
			player = 'O';
		}
		moves = 1;
	}
		
	void move(int i) {
		assert(moves > 0);
		assert(0 <= i);
		assert(i < 9);
		
		bs[i] = player;
		--moves;
	}
	
	float get_result(PlayerId p) {
		
		// -
		if (bs[0] == bs[1] == bs[2] == p) return 1;
		if (bs[3] == bs[4] == bs[5] == p) return 1;
		if (bs[6] == bs[7] == bs[8] == p) return 1;
		// |
		if (bs[0] == bs[3] == bs[6] == p) return 1;
		if (bs[1] == bs[4] == bs[7] == p) return 1;
		if (bs[2] == bs[5] == bs[8] == p) return 1;
		// X
		if (bs[0] == bs[4] == bs[8] == p) return 1;
		if (bs[2] == bs[4] == bs[6] == p) return 1;
		
		return 0;
	}
	
	
	
};

struct OXWrap {
	
	using PlayerId = typename OX::PlayerId;
	
	OX &game;
	
	OXWrap(OX &g): game(g) {}
	
	struct Action {
		virtual void exec(OX &game) = 0;
		virtual bool eq(Action & a) = 0;
		
		bool operator==(Action &a) {
			try {
				return this->eq(a);
			}
			catch (std::bad_cast) {
				return false;
			}
		}
		
		virtual ostream& dump(ostream& out) = 0;
	};
	
	struct Move: Action {
		int i;
		Move(int i_) {
			i = i_;
		}
		
		void exec(OX &game) {
			game.move(i);
		}
		
		bool eq(Action & a)  {
			Move& b = dynamic_cast<Move&>(a);
			return i == b.i;
		}
		
		virtual ostream& dump(ostream& out) {
			out << "Move(" << i << ")";
		}
		
	};
	
	struct EndTurn: Action {
		void exec(OX &game) {
			game.end_turn();
		}		

		bool eq(Action & a)  {
			EndTurn& b = dynamic_cast<EndTurn&>(a);
			return true;
		}
		
		virtual ostream& dump(ostream& out) {
			out << "EndTurn()";
		}
		
	};	
	
	
	void exec(Action *a) {
		if (a == nullptr) {
			EndTurn().exec(this->game);
		}
		else {
			a->exec(this->game);
		}
	}
	
	float get_result(OX::PlayerId p) {
		return game.get_result(p);		
	}
	
	Action *create_random_action() {
		if (game.moves) {
			return new Move(roll::roll(0,9));
		}
		else {
			return nullptr; // endturn
		}
	}
	
};



	

ostream& operator<<(ostream &out, OXWrap::Action &a) {
	return a.dump(out);	
}


ostream& operator<<(ostream &out, OX const& g) {
	out << g.bs[0] << g.bs[1] << g.bs[2] << "\n"
	    << g.bs[3] << g.bs[4] << g.bs[5] << "\n"
	    << g.bs[6] << g.bs[7] << g.bs[8] << "\n";
}


int main()
{
	
	OX game;	
	OXWrap gw(game);
	
	col::MCTS<OXWrap> mt(gw, 'O');
	
	mt.think();
	pprint(mt.root);
	
	mt.think();
	pprint(mt.root);
	
	mt.think();
	pprint(mt.root);
	
	mt.think();
	pprint(mt.root);
	
	mt.think();
	pprint(mt.root);
	
	return 0;
}


