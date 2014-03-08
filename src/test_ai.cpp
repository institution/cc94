#include <iostream>
#include <map>
#include <boost/format.hpp>

#include "ai.h"
#include "roll.h"
#include "ox.h"

			
using namespace std;
using namespace boost;
using namespace ox;




struct OXSim {
	
	using PlayerId = typename OX::PlayerId;
	
	OX game;
	OX &base;
	
	OX::PlayerId pid;
	
	OXSim(OX &g, OX::PlayerId const& p): game(g), base(g), pid(p) {}
	
	void reset() {
		game = base;
	}
	
	struct Action {
		using PlayerId = OX::PlayerId;
		PlayerId player_id;
		
		Action(PlayerId pid): player_id(pid) {}
		
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
		
		virtual Action* new_copy() = 0;
	};
	
	struct Move: Action {
		int i;
		
		Move(PlayerId pid, int ii): Action(pid) {
			i = ii;
		}
		
		void exec(OX &game) {
			game.move(player_id, i);
		}
		
		bool eq(Action & a)  {
			Move& b = dynamic_cast<Move&>(a);
			return i == b.i;
		}
		
		Action* new_copy() {
			return new Move(*this);
		}
		
		virtual ostream& dump(ostream& out) {
			out << "Move('" << player_id << "'," << i << ")";
		}
		
		
	};
	
	struct Start: Action {
		Start(PlayerId pid): Action(pid) {}
		
		void exec(OX &game) {
			
		}		

		bool eq(Action & a)  {
			Start& b = dynamic_cast<Start&>(a);
			return true;
		}
		
		Action* new_copy() {
			return new Start(*this);
		}
		
		virtual ostream& dump(ostream& out) {
			out << "Start('" << player_id << "')";
		}
		
	};	
	
	
	void exec(unique_ptr<Action> &a) {
		a->exec(this->game);		
	}
	
	void exec_real(Action &a) {
		a.exec(this->base);
	}
	
	float get_result(PlayerId const& pid) {
		return game.get_result(pid);
	}
	
	unique_ptr<Action> create_random_action() {		
		if (!game.end) {			
			int i = roll::roll(0, game.free);
						
			int j = 0;
			while (j < 9) {
				if (game.bs[j] == '.') {
					//cout << "i = "<< i << endl;
					if (i == 0) break;
					--i;
				}
				++j;
			}			
			return unique_ptr<Action>(new Move(game.player, j));
			
		}
		else {
			return unique_ptr<Action>(nullptr); 
		}
	}
	
};



	

ostream& operator<<(ostream &out, OXSim::Action &a) {
	return a.dump(out);	
}

void dump(OXSim::Action &a) {
	cout << a << endl;
}




int main()
{
	/*{
		OX t;
		t.move('O', 1);
		dump(t);
		t.end_turn('O');

		t.move('X', 2);
		dump(t);
		t.end_turn('X');
	}*/
	
	roll::seed();
	
	OX game(0);	
		
	using NodeType = tree::Node<OXSim::Action>;
	
	unique_ptr<OXSim::Action> tmp(new OXSim::Start('X'));
	
	
	NodeType *root = new NodeType(tmp);
	

	while (1) {
		for (int i=0; i<90*2; ++i) {

			//cout << "dumping tree before step: " << endl;
			//dump(root, 8);
			//cout << endl;

			OXSim gw(game, 'O');

			//cout << "after reset:" << endl;
			//dump(gw.game);

			//cout << "step" << endl;
			mcts::step(root, gw, 0);

			//cout << "game state after play: " << endl;
			//dump(gw.game);
			//cout << endl;


		}

		cout << "dumping tree: " << endl;
		dump(root, 2);

		// select preferred move
		NodeType *node = mcts::preferred_node(root);
		cout << "preferred move: " << *node->action << endl;
		
		OXSim::Action &action = *node->action;
		
		
		OXSim tmp(game, 'O');
		tmp.exec_real(action);
		
		
		cout << "CURRENT GAME STATE: " << endl;
		dump(game);
		
		
		mcts::apply(root, action);
		
		
		
		//cout << "dumping tree after extract: " << endl;
		//dump(root, 20);

		cout << "enter any letter: " << endl;
		char x;
		cin >> x;
		
	}
	
	delete root;
	
	return 0;
}


