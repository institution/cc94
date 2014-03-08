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
	
	

}
	
#endif
