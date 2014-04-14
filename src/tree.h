#ifndef TREE2_H
#define TREE2_H

namespace tree {
	
	template<typename P>
	struct Node {
		P data;
				
		Node *parent;
		Node *left_child;
		Node *right_neigh;
		
		Node(P &data_): data(data_) {
			left_child = nullptr;
			right_neigh = nullptr;
			parent = nullptr;
		}
		
		~Node() {
			if (left_child)	 delete left_child;
			if (right_neigh) delete right_neigh;
		}
		
		

		Node* insert_child(Node *node) {
			node->parent = this;
			node->right_neigh = this->left_child;
			this->left_child = node;
			return node;
		}

		Node* insert_child(P &data_) {
			Node *node = new Node(data_);
			node->parent = this;
			node->right_neigh = this->left_child;
			this->left_child = node;
			return node;
		}

		Node* extract(Node *child) {

			Node<Action> *child_node = left_child;
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
		
	};
	
	
	
	
	struct Node2 {
		Node2 *parent;
		Node2 *left_child;
		Node2 *right_neigh;
		
		Node2() {
			left_child = nullptr;
			right_neigh = nullptr;
			parent = nullptr;
		}
		
		~Node2() {
			if (left_child)	 delete left_child;
			if (right_neigh) delete right_neigh;
		}
				
	};

	Node2* insert(Node2* par, Node2 *chi) {
		chi->parent = par;
		chi->right_neigh = par->left_child;
		par->left_child = chi;
		return chi;
	}
	
	Node2 *extract(Node2 *chi) {
		if (chi->parent == nullptr) {
			return chi;
		}
		
		Node2 *iter = chi->parent->left_child;
		while (iter) {
			if (node == iter->right_neigh) {
				iter->right_neigh = chi->right_neigh;
				chi->parent = nullptr;
				chi->right_neigh = nullptr;
				return chi;
			}					
			iter = iter->right_neigh;
		}
		assert(0); // impossible to get here
		return nullptr;
	}
	
	Node2* move(Node2 *par, Node2 *chi) {
		return insert(par, extract(chi));
	}
	

}
	
#endif
