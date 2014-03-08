#include <iostream>
#include <map>
#include <boost/format.hpp>

#include "ai.h"
#include "roll.h"
  

			
using namespace std;
using namespace boost;

struct OX;

void dump(OX const& ox);


struct OX{
	
	struct InvalidMove {
		
	};
	
	using PlayerId = char;
	
	// 012
	// 345
	// 678
	char bs[9];
	char player;
	int free;
	map<char,float> score;
	bool end;
	int verbose;
	
	OX(int verbosee = 0) {
		for (auto &b: bs) {
			b = '.';
		}
		player = 'O';
		free = 9;
		
		score['O'] = 0;
		score['X'] = 0;
		
		end = false;
		verbose = verbosee;
	}
	
	
	char get(int x, int y) {
		return bs[x + y*3];
	}
	
	bool bound(int x, int y) {
		return 0 <= x and x < 3 and 0 <= y and y < 3;
	}
	
	void move(PlayerId pid, int i) {
		int x = i%3;
		int y = i/3;
		
		if (verbose)
			cout << format("OX::move(%|| to %||,%||)\n") % pid % x % y;
		
		assert(pid == player);
		assert(0 <= i);
		assert(i < 9);
		assert(bs[i] == '.');
		assert(!end);
		
		bs[i] = player;
		--free;
		
		if (player == 'O') {
			player = 'X';
		}
		else {
			player = 'O';
		}
		
		
		auto r = check_pos(x, y);
		if (r) {
			score[player] = 1;
			end = true;
			if (verbose) {
				dump(*this);
				cout << format("OX And the winner is: '%||'\n") % player;
			}
		
		}
		else		
		if (free == 0) {
			score['O'] = 0.5;
			score['X'] = 0.5;
			end = true;
			
			if (verbose) {
				dump(*this);
				cout << format("OX And the winner is: TIE\n");
			}
		}
		
		
			
		
	}
	
	bool check_pos(int x, int y) {
		if (1 + check_dir(x,y,0,-1) + check_dir(x,y,0,1) >= 3) return 1;
		if (1 + check_dir(x,y,-1,0) + check_dir(x,y,1,0) >= 3) return 1;
		if (1 + check_dir(x,y,-1,-1) + check_dir(x,y,1,1) >= 3) return 1;
		if (1 + check_dir(x,y,1,-1) + check_dir(x,y,-1,1) >= 3) return 1;
		return 0;
	}
	
	int check_dir(int x, int y, int dx, int dy) {
		int i = x + dx;
		int j = y + dy;
		int l = 0;
		
		while (bound(i,j) and (get(i, j) == player)) {
			++l;
			
			i += dx;
			j += dy;
		}
		return l;
	}
	
	
	float get_result(PlayerId p) {
		if (verbose > 1) 
			cout << format("OS::get_result('%||') -> %|| \n") % p % score.at(p);
		
		return score.at(p);
	}
	
	
};

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

ostream& operator<<(ostream &out, OX const& g) {
	out << g.bs[0] << g.bs[1] << g.bs[2] << "\n"
	    << g.bs[3] << g.bs[4] << g.bs[5] << "   player: '" << g.player << "'\n"
	    << g.bs[6] << g.bs[7] << g.bs[8] << "\n";
}

void dump(OX const& ox) {
	cout << ox;
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
		for (int i=0; i<1000; ++i) {

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


