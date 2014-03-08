#ifndef AI_H
#define AI_H

#include <cmath>
#include <memory>
#include <stdexcept>
#include <iostream>
#include "aga2.hpp"
#include "col.h"
#include "env.h"


namespace tree {
	
	using std::move;
	using std::unique_ptr;
	using std::runtime_error;
	using std::ostream;
	using std::cout;
	using std::endl;
	
	template<typename T>
	struct Node {
		
		unique_ptr<T> action;
		float win, total;
				
		Node *parent;
		Node *left_child;
		Node *right_neigh;
							
		Node(unique_ptr<T> &a): action(move(a)), win(0), total(0) {
			left_child = nullptr;
			right_neigh = nullptr;
			parent = nullptr;
		}
		
		~Node() {
			if (left_child)	{
				delete left_child;
				left_child = nullptr;
			}
		
			if (right_neigh) {
				delete right_neigh;
				right_neigh = nullptr;
			}
		}
		
	};
	
	//using NodeType = Node<Data<>>
	
	template<typename T>
	Node<T>* insert_child(Node<T> *parent, Node<T> *child) {		
		child->right_neigh = parent->left_child;
		child->parent = parent;		
		parent->left_child = child;
		return child;
	}
	
	template<typename T>
	Node<T>* insert_child(Node<T> *parent, unique_ptr<T> &a) {		
		return insert_child(parent, new Node<T>(a));
	}
	
	template<typename T>
	void destroy(Node<T> *&node) {
		delete node;
		node = nullptr;
	}
	
	
	template<typename T>
	void extract(Node<T> *node) {
		/* Extract subtree with root at node */
		
		Node<T> *parent = node->parent;
		assert(parent != nullptr);  // no parent - node already extracted
		
		Node<T> *iter = parent->left_child;
		if (node == iter) {
			// rewire parent
			parent->left_child = node->right_neigh;			
		}
		else {
			// find and rewire left neigh
			while (iter) {
				if (node == iter->right_neigh) {
					// rewire left neigh
					iter->right_neigh = node->right_neigh;
					break;
				}					
				iter = iter->right_neigh;
			}		
			assert(iter);  // node not found is its parent childs - impossible
		}
		
		// clear node
		node->parent = nullptr;
		node->right_neigh = nullptr;
		return;
	}
	
	
	
	template<typename T>
	ostream& operator<<(ostream &out, Node<T> const& n) {
		if (n.action) {
			out<<"Node("<<(*n.action)<<","<<n.win<<","<<n.total<<")";
		}
		else {
			out<<"Node(nullptr,"<<n.win<<","<<n.total<<")";
		}
		//out << (format(
		//	"Icon(id=%||, type_id=%||, x=%||, y=%||, player_id=%||)"
		//) % t.id % t.type->id % t.x % t.y % t.player->id);
		return out;
	}
	
	template<typename T>
	void dump(Node<T> *n, int maxdeep, int deep=0) {
		for (int i=0; i<deep; ++i) cout << "  ";
		cout << *n << endl;
		
		if (deep < maxdeep) {
			Node<T> *node = n->left_child;
			while (node) {
				dump(node, maxdeep, deep+1);
				node = node->right_neigh;
			}
		}
	}
	
} // tree


namespace mcts {
	using std::cout;
	using std::endl;
	using std::unique_ptr;
	using std::move;
	
	// 1.0 - normal
	const float EXPLORE_FACTOR = 1.0;   
	
	template <typename Node, typename Game>
	Node* select(Node *node, Game &game, int verbose=0) {

		using std::log;
		
		auto ra = game.create_random_action();
		
		if (!ra) {
			// terminal node
			return node;
		}
		
		
		// try add new action
		//cout << "try add: " << *ra << endl;
		Node *iter = node->left_child;
		while (iter) {
			if (*ra == *iter->action) {
				ra = nullptr;
				break;
			}
			iter = iter->right_neigh;
		}

		if (ra) {	
			node = insert_child(node, ra);
			
			if (verbose > 1)
				cout << "select/new_move: " << *node->action << endl;
			
			game.exec(node->action);
			return node;
		}
	

		// or find most interesting node
		// cout << "find exiting move " << endl;
		float best_value = -1;
		Node *best_node = nullptr;
		float ln_t = log(node->total);
		
		iter = node->left_child;
		while (iter) {
			float w = iter->win;
			float n = iter->total;
			float value = w / n + EXPLORE_FACTOR * sqrt(2 * ln_t / n);
			if (value > best_value) {
				best_value = value;
				best_node = iter;				
			}
			iter = iter->right_neigh;
		}
		
		if (best_node) {
			node = best_node;
			
			if (verbose > 1)
				cout << "select/existing_move: " << *node->action << endl;
			
			game.exec(node->action);
			return node;
		}
		
		assert(0);
		//throw runtime_error("impossible to be here");		
	}

	
	template <typename Node, typename Game>
	void step(Node *root, Game &game, int verbose=0) {
		Node *node = root;
		
		if (verbose) 
			cout << "step/root: " << *node->action << endl;
		
		
		// select until leaf
		while (node->left_child) {
			// select already visited or not yet tried node (if any)
			node = select(node, game, verbose);
			if (verbose)
				cout << "step/select:  " << *node->action << endl;
		}
		
		// expand this node
		auto ra = game.create_random_action();
		if (ra) {
			game.exec(ra);
			node = insert_child(node, ra);
			if (verbose)
				cout << "step/expand: " << *node->action << endl;			
		}
		else {
			// terminal node - so what?
			// cout << "[EXPAND] " << "(terminal node - no expand)" << endl;
		}
		
		// play
		// cout << "mcts::step - play" << endl;
		while (ra = game.create_random_action()) {
			game.exec(ra);
		}
		
		
		// backprop
		//cout << "mcts::step - backprop" << endl;
		while (node) {
			if (node->action)
				node->win += game.get_result(node->action->player_id);
			node->total += 1.0f;
			node = node->parent;
		}
		
		if (verbose)
			cout << endl;
		
	}
	
	template <typename T>
	void apply(tree::Node<T> *&root, T &action) {
		
		tree::Node<T>* iter = root->left_child;
		while (iter) {
			if (*iter->action == action) {
				break;
			}
			iter = iter->right_neigh;
		}
		
		tree::Node<T> *node = nullptr;
		if (iter) {
			node = iter;
		}
		else {
			unique_ptr<T> tmp(action.new_copy());
			node = insert_child(root, tmp);
		}
	
		tree::extract(node);
		tree::destroy(root);
		root = node;
	}
	
	template <typename Node>
	Node* preferred_node(Node *root) {
		float best_total = -1;
		Node *best_node = nullptr;
		
		Node *iter = root->left_child;
		while (iter) {
			auto total = iter->total;
			if (best_total < total) {
				best_total = total;
				best_node = iter;				
			}
			iter = iter->right_neigh;
		}
		
		return best_node;
	}
	
	

}

#endif









/*
namespace col {
	template<typename Game>
	struct MCTS {
		using PlayerId = typename Game::PlayerId;
		using NodeType = Node<typename Game::Action>;
		using Action = typename Game::Action;
		
		
		PlayerId player_id;
		NodeType *root;
		Game const& o_env;
		
		MCTS(Game const& env, PlayerId const& pid): o_env(env), player_id(pid) {
			root = new NodeType(nullptr);
		}
		
		~MCTS() {
			if (root) 
				delete root;
		}
		
		void think(Node *root, Game &game) {
			Action *ra = nullptr;
			auto const& pid = player_id;
			int const NUM_OF_PLAYERS = 2;
			
			Game env(o_env);  // copy curr state
			
			// select
			NodeType *node = root;
			
			while (node->left_child) {
				
				NodeType *child_node = node->left_child;
				
				float ln_t = std::log(node->total);
				ra = env.create_random_action();
				
				float best_value = 0;
				NodeType *best_child = nullptr;
				
				while (child_node) {
					// check action already generated
					if (ra && ((*ra) == *(child_node->action))) {
						delete ra;
						ra = nullptr;
					}							
				
					float w = child_node->win;
					float n = child_node->total;
					
					// find best node
					float value = w / n + sqrt(2 * ln_t / n);
					if (value > best_value) {
						best_child = child_node;
						best_value = value;
					}
					
					child_node = child_node->right_neigh;
				} 
				
				
				// select new action or best_node
				if (ra) {
					// add and select node with new action
					node = insert_child(node, ra);
				}
				else {
					// select best node
					node = best_child;
				}
						
			}
			
			// expand
			while (ra = env.create_random_action()) {
				env.exec(ra);
				node = insert_child(node, ra);				
			}
			
			
			// play (5 random turns ahead)
			for (int i=0; i<5*NUM_OF_PLAYERS; ++i) {
				while (ra = env.create_random_action()) {
					env.exec(ra);
				}				
			}
			
			float result = env.get_result(player_id);
			
			// backprop
			while (node) {
				node->win += result;
				node->total += 1.0f;
				node = node->parent;
			}
			
		}
		
		
		Action const* get_action() {
			// return best move
			NodeType *node = root;
			NodeType *child_node = root->left_child;
			NodeType *best_child = nullptr;
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
			pts[x.second.id] = 0;			
		}
		
		for (auto& x: env.icons) {
			pts[x.second.player->id] += 1;
		}		
		
		float total = 0;
		for (auto& x: pts) {
			total += x.second;
		}
		
		return pts.at(pid)/total;		
	}
		
} // ns col
*/
























	
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