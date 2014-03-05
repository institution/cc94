#ifndef AI_H
#define AI_H

#include "aga2.hpp"
#include "col.h"
#include "env.h"


namespace col {
	template<typename Action>
	struct Node {
		Action *action;
		float win, total;
		
		Node *parent;
		Node *left_child;
		Node *right_neigh;
					
		Node(Action *action_, float win_=0, float total_=0) {
			action = action_;
			win = win_; 
			total = total_;
			
			left_child = nullptr;
			right_neigh = nullptr;
			parent = nullptr;
		}
		
		~Node() {
			if (action)
				delete action;
			
			if (left_child)
				delete left_child;
			
			if (right_neigh)
				delete right_neigh;
		}
	};
	
	
	ostream& operator<<(ostream &out, Node const& n) {
		out<<"Node("<<(*n.action)<<","<<n.win<<","<<n.total<<")";
		//out << (format(
		//	"Icon(id=%||, type_id=%||, x=%||, y=%||, player_id=%||)"
		//) % t.id % t.type->id % t.x % t.y % t.player->id);
	}
	
	
	template<typename Game>
	struct MCTS {
		using PlayerId = typename Game::Player::Id;
		using NodeType = Node<typename Game::Action>;
		
		PlayerId player_id;
		NodeType *root;
		Game const& o_env;
		
		MCTS(Game const& env, PlayerId const& pid): o_env(env), player_id(pid) {
			root = new Node<typename Game::Action>(nullptr);			
		}
		
		~MCTS() {
			if (root) 
				delete root;
		}
		
		void think() {
			Action *ra = nullptr;
			auto const& pid = player_id;
			int const NUM_OF_PLAYERS = 2;
			
			NodeType *node = root;
			
			Env env(o_env);  // copy curr state
			
			// select
			while (node->left_child) {
								
				float w = node->win;
				float n = node->total;
				float ln_t = ln(parent->total);
				
				NodeType *child_node = node->left_child;
				
				ra = create_random_action(env, pid)
				
				float best_value = 0;
				do {
					// check action already generated
					if (ra && *ra == *(child_node->action)) {
						delete ra;
						ra == nullptr;
					}							
					
					// find best node
					float value = w / n + sqrt(2 * ln_t / n);
					if (value > best_value) {
						best_child = child_node;
						best_value = value;
					}
					
				} 
				while (child_node = child_node->right_neigh);
				
				// select new action or best_node
				if (ra) {
					// add and select node with new action
					node = create_child(node, ra);
				}
				else {
					// select best node
					node = best_node;
				}
						
			}
			
			// expand
			while (ra = create_random_action(env, pid)) {
				env.exec(ra);
				node = create_child(node, ra);				
			}
			
			
			// play (5 random turns ahead)
			for (int i=0; i<5*NUM_OF_PLAYERS; ++i) {
				while (ra = create_random_action(env, get_pid(env))) {
					env.exec(ra);
				}
			}
			
			float result = get_result(env, player_id);
			
			// backprop
			while (node) {
				node->win += result;
				node->total += 1.0f;
				node = node->parent;
			}
			
		}
		
		Action const* get_action() {
			// return best move
			node = root;
			Node *child_node = root->left_child;
			Node *best_child = nullptr;
			float best_value = -1;
				
			while (child_node) {
				float value = child_node->total;
				if (value > best_value) {
					best_child = child_node;
					best_value = value;
				}
				child_node = child_node->right_neigh;
			}
			
			// best_child becomes new root
			extract(best_child);
			destroy_node(root);			
			root = best_child;			
			return root->action;
		}
				
	};
	
	
	float get_result(Env const& env, Player::Id pid) {
		map<Player::Id, float> pts;
		
		for (auto& x: env.players) {
			Player& pl = x.second;
			pts[pl.id] = 0;			
		}
		
		for (auto& x: env.icons) {
			pts[x.second->player->id] += 1;
		}		
		
		float total = 0;
		for (auto& x: pts) {
			total += x.second;
		}
		
		return pts.at(pid)/total;		
	}
		
	
	void destroy_node(Node *node) {
		delete node;
	}
	
	void create_child(Node *parent, Node *child) {
		tmp = left_child;
		left_child = child;
		child.right_neigh = tmp;		
		child.parent = parent;
	}
	
	void extract(Node *node) {
		
		parent = node->parent;
		assert(parent != nullptr);
		child_node = parent->left_child;
		while (child_node) {
			if (node == child_node->right_neigh) {
				child_node->right_neigh = node->right_neigh;
				node->parent = nullptr;
				node->right_neigh = nullptr;
				return;
			}					
			child_node = child_node->right_neigh;
		}
		
		assert(0); // impossible to get here
	}
			
	
		
}

#endif







































	
/*
	template<typename T>
	using Vector2 = aga2::Mv1<T>;

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
		Vector2<uint8>::Iter p;
		uint32 unit_id;

		AttackMove(Icon::Id const& unit_id) {
			dir = {0,0};
			p = dir.begin();
			this->unit_id = unit_id;
		}

		bool cycle() {
			while (p != dir.end()) {
				// move state to next variant
				if (cycle_dir_coord(*p)) {
					return true;
				}
				p = ++p;
			}		
			p = dir.begin();   // reset cycle		
			return false;    // return false on full cycle (on {0,0}))
		}

		void show() {
			cout << "move " << unit_id << " " << dir[0] << " " << dir[1] << endl;
		}

	};




	template <typename Env>
	struct Node {
		Env env;
		
		vector<AttackMove> actions;

		vector<AttackMove>::iterator curr_action_p;
		vector<AttackMove>::iterator end_p;
		
		Node(const Env &org): env(org) {
			
			for (auto &u: env.icons) {
				if (u.player == env.curr_player) {
					actions.push_back(AttackMove(u.id));
				}
			}
			
			curr_action_p = actions.begin();
			end_p = actions.end();
			
			cout << "init node" << endl;
			for (auto &a: actions) {
				a.show();
			}			
			
		}
		
		Node copy() {
			return *this;
		}

		void init_actions() {

		}

		bool is_terminal() {
			return false;
		}

		float hvalue() {
			return 1;
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

	template <typename Env>
	struct Ai{
		const Env &srv;

		Ai(const Env &envproxy): srv(envproxy) {

		}


		void minmax(float &best_val, int &best_ind, Node<Env> &node, uint depth, bool maximizing_player) {

			if (depth == 0 or node.is_terminal()) {
				best_value = node.hvalue();
				best_index = 0;
			}

			if (maximizing_player) 
			{
				best_value = node.hvalue();
				best_index = 0;
				
				int index = 0;
				while(node.cycle_right()) {
					++index;
					
					val = minimax(val, ind, node.go_down(), depth - 1, false);
					if (best_value < val) {
						best_value = val;
						best_index = index;
					}
					node.go_up();
				} 
				
				
				val = node.hvalue();
				ind = 0;
				
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


		void run() {
			auto n = minimax(origin, depth, TRUE);
			
			cout << "result" << endl;
			for (auto &a: n.actions) {
				a.show();
			}
		}

	};



*/