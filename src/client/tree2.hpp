#pragma once

#include <iostream>

namespace tree2 {

	template<typename T>
	struct Node {

		using value_type = T;

		T payload;

		Node *parent{nullptr};
		Node *left_child{nullptr};
		Node *right_neigh{nullptr};

		Node(T const& payload): payload(payload) {}
		// Node(T && payload): payload(payload) {}

		~Node();

		T & operator*() {
			return payload;
		}

		T const& operator*() const {
			return payload;
		}

		template<typename N>
		struct base_iterator {
			N * iter;

			base_iterator(N * iter): iter{iter} {}

			void operator++() {
				iter = iter->right_neigh;
			}

			bool operator!=(base_iterator<N> const& other) {
				return iter != other.iter;
			}

			N & operator*() {
				return *iter;
			}
		};

		using iterator = base_iterator<Node<T>>;

		iterator begin() {
			return iterator(this->left_child);
		}

		iterator end() {
			return iterator(nullptr);
		}

		using const_iterator = base_iterator<Node<T> const>;

		const_iterator begin() const {
			return const_iterator(this->left_child);
		}

		const_iterator end() const {
			return const_iterator(nullptr);
		}

		void clear() {
			if (left_child)	{
				delete left_child;
				left_child = nullptr;
			}

			if (right_neigh) {
				delete right_neigh;
				right_neigh = nullptr;
			}
		}

		bool empty() const {
			return left_child == nullptr;
		}

		Node<T> & insert_child(T const& payload);

		iterator find_child(T const& payload);
		const_iterator find_child(T const& payload) const;

		void extract();
		void dump(int maxdeep, int deep=0) const;


	};

	template<typename T> inline
	Node<T>::~Node() {
		clear();
	}




	template<typename T>
	Node<T> * insert_child(Node<T> *parent, Node<T> *child) {
		child->right_neigh = parent->left_child;
		child->parent = parent;
		parent->left_child = child;
		return child;
	}

	template<typename T>
	Node<T> * insert_child(Node<T> *parent, T const& payload) {
		return insert_child(parent, new Node<T>(payload));
	}

	template<typename T> inline
	Node<T> & Node<T>::insert_child(T const& payload) {
		return *tree2::insert_child(this, payload);
	}

	template<typename T>
	void destroy(Node<T> *&node) {
		delete node;
		node = nullptr;
	}



	template<typename T>
	Node<T> * find_child(Node<T> * parent, T const& payload) {
		Node<T> *iter = parent->left_child;
		while (iter) {
			if (*iter->payload == payload) {
				return iter;
			}
			iter = iter->right_neigh;
		}
		return nullptr;
	}


	template<typename T> inline
	typename Node<T>::iterator Node<T>::find_child(T const& payload) {
		auto it = begin();
		while (it != end()) {
			if ((*it).payload == payload) {
				return it;
			}
			++it;
		}
		return end();
	}

	template<typename T> inline
	typename Node<T>::const_iterator Node<T>::find_child(T const& payload) const {
		auto it = begin();
		while (it != end()) {
			if ((*it).payload == payload) {
				return it;
			}
			++it;
		}
		return end();
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
			assert(iter);  // node not found in its parent childs - impossible
		}

		// clear node
		node->parent = nullptr;
		node->right_neigh = nullptr;
		return;
	}

	template<typename T> inline
	void Node<T>::extract() {
		extract(this);
	}



	template<typename T>
	std::ostream& operator<<(std::ostream &out, Node<T> const& n) {
		out<<"Node("<<n.payload<<")";
		return out;
	}

	template<typename T>
	void dump(Node<T> const* n, int maxdeep, int deep=0) {
		for (int i = 0; i < deep; ++i) {
			std::cout << "  ";
		}

		std::cout << *n << std::endl;

		if (deep < maxdeep) {
			Node<T> const* node = n->left_child;
			while (node) {
				dump(node, maxdeep, deep+1);
				node = node->right_neigh;
			}
		}
	}


	template<typename T> inline
	void Node<T>::dump(int maxdeep, int deep) const {
		dump(this, maxdeep, deep);
	}


} // namespace
