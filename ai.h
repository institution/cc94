#ifndef AI_H
#define AI_H

#include "aga2.hpp"
#include "col.h"
#include "env.h"


namespace col {
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
}

#endif
