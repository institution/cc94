#ifndef AI_H
#define AI_H

ATTACK_MOVE = 1



struct Action{
	virtual bool cycle() {
		// move state to next variant
				
		// return false on full cycle
	}
};


bool cycle_dir_coord(uint8 &x) {
	x = ((x + 2) % 3) - 1; // 0 -> 1 -> -1 -> 0
	return x != 0;
}

struct AttackMove: public Action{
	Vector2<uint8> dir;
	
	AttackMove() {
		dir = {0,0};
		p = d.begin();		
	}
	
	bool cycle() {
		while (p != dir.end()) {
			// move state to next variant
			if (cycle_dir_coord(*p)) {
				return true;
			}
			p = ++p;
		}		
		p = d.begin();   // reset cycle		
		return false;    // return false on full cycle (on {0,0}))
	}
	
};
		
struct Node {
	Env env;
		
	
	
	Node(const Env &org): env(org) {
		actions = 
	}
	
	void init_actions() {
		
	}
	
	bool is_terminal() {
		return false;
	}
	
	float hvalue() {
		
	}
	
	float inf() {
		return 10000;
	}
	
	bool cycle_right() {		
		while (curr_action_p != end_p) {
			if ((*curr_action_p).cycle()) {
				return 1;				
			}
			curr_action_p = ++curr_action_p;
		}
		return 0;
	}
	
};

struct Ai{
	const Env &srv;
	
	Ai(const Env &envproxy): srv(envproxy) {
		
	}
	
	void run() {
		minimax(origin, depth, TRUE)
	}
	
	
	void minimax(node, depth, maximizing_player) {
		
		if (depth == 0 or node.is_terminal()) {
			return node.hvalue();
		}
				
		if (maximizing_player) {
			auto best_value = node.hval();
			auto best_node = node.copy();
		
			while(node.cycle_right()) {				
				val = minimax(node.go_down(), depth - 1, false);
				if (best_value < val) {
					best_value = val;
					best_node = node.copy();
				}	
				node.go_up();
			}	
			
					
			return best_node;
		}
		else {
			auto best_value = node.hval();
			auto best_node = node.copy();
					
			while (node.cycle_right()) {
				val = minimax(node.go_down(), depth - 1, true))
				if (best_value > val) {
					best_value = val;
					best_node = node.copy();
				}
				node.go_up();
			}
			
				
			return best_node;
		}
		
	}
	
	
	
};




		

#endif
