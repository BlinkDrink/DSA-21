#pragma once
#include<vector>
#include<list>
#include<queue>
#include<string>

using std::string;
using std::list;
using std::vector;
using std::max;
using std::priority_queue;

class Tree
{
public:
	/// @brief Object lifetime

	Tree() : root(nullptr), fSize(0) {}

	Tree(const string& top) : root(new Node(top)), fSize(1) {}

	~Tree() { clear(root); }

	Tree(const Tree& tree) : root(copy(tree.root)), fSize(tree.fSize) {};

	Tree(Tree&& tree) noexcept : Tree()
	{
		std::swap(root, tree.root);
		std::swap(fSize, tree.fSize);
	};

	Tree& operator=(const Tree& tree)
	{
		if (&tree != this)
		{
			clear(root);
			root = copy(tree.root);
			fSize = tree.fSize;
		}
		return *this;
	}

	Tree& operator=(Tree&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::swap(root, rhs.root);
			std::swap(fSize, rhs.fSize);
		}
		return *this;
	}

public:
	/// @brief Public methods

	/// @brief Wrapper of inner method getSalaryOf
	/// @param who - node' salary that needs to be calculated
	/// @return salary of node
	unsigned long getSalaryOf(const string& who) const { return getSalaryOf(root, who); }

	/// @brief Wrapper of inner method findParentKeyOf
	/// @param key - name of item
	/// @return The parent of the node with name <key>
	string findParentKeyOf(const string& key) const
	{
		return findParentKeyOf(root, key);
	}

	/// @brief Wrapper of inner method getNumberOfChildrenOf
	/// @param key - the key of the node for which we will determine the number of children
	/// @return the number of children
	int getNumberOfChildrenOf(const string& key) const
	{
		size_t res = 0;
		getNumberOfChildrenOf(root, key, res);
		return res;
	}

	/// @brief Wrapper of inner method find
	/// @param key - the key of the element we will be searching for
	/// @return true if the element is contained, false otherwise
	bool find(const string& key) const { return find(root, key); }

	/// @brief Wrapper of inner method insert
	/// @param who - the employee to be added underneath <boss>
	/// @param boss - the boss of <who>
	/// @return true if the operation succeeded, false otherwise
	bool insert(const string& who, const string& boss) { return insert(root, who, boss); }

	/// @brief Wrapper of inner method getOverloadedNodes
	/// @param level - overloadness factor 
	/// @return number of nodes having more than <level> descendants
	int getOverloadedNodes(int level) const { return getOverloadedNodes(root, level); }

	/// @brief Wrapper of inner method remove
	/// Follows a specific rule given in the description of the prblem
	/// @param key - the name of the element that we will be removing
	/// @return true if the operation succeeded, false otherwise
	bool remove(const string& key) { return remove(root, key); }

	/// @brief Wrapper of inner method height
	/// @return The height of the tree
	size_t height() const { return height(root); };

	/// @return number of nodes in the tree
	size_t size() const { return fSize; };

	//void printByLevels() const { printByLevels(root); }

	/// @brief Wrapper of inner method toString
	/// @return stringified tree
	string toString() const { return toString(root); };

private:
	/// @brief Tree node descriptor
	struct Node
	{
		string data;
		Node* child, * brother, * parent;

		Node(const string& data, Node* parent = nullptr, Node* child = nullptr, Node* brother = nullptr)
			: data(data), parent(parent), child(child), brother(brother)
		{}

	} *root;

	struct LessByName {
		bool operator()(Tree::Node* a, Tree::Node* b)
		{
			return a->data > b->data;
		}
	};

	size_t fSize;

private:
	/// @brief Clears the tree(deleting every node)
	/// @param root - the beginning of the tree
	void clear(Node*& root)
	{
		if (root)
		{
			clear(root->child);
			clear(root->brother);
			delete root;
		}
	}

	/// @brief Makes a copy of a tree given it's root
	/// @param root - the root of the tree that will be copied
	/// @return the copied tree
	Node* copy(Node* root)
	{
		return root ? new Node(root->data, root->parent, copy(root->child), copy(root->brother)) : root;
	}

	/// @brief Inserts element <who> underneath <boss>
	/// @param root - the begining of the tree
	/// @param who - the employee to be added underneath <boss>
	/// @param boss - the boss of <who>
	/// @return true if the operation succeeded, false otherwise
	bool insert(Node*& root, const string& who, const string& boss)
	{
		if (fSize == 0)
		{
			root = new Node(boss);
			root->child = new Node(who, root);
			fSize += 2;
			return true;
		}

		if (!root)
			return false;

		if (root->data == boss)
		{
			Node* whoNode = findNodeByKey(this->root, who);

			if (!whoNode)
			{
				whoNode = new Node(who, root/*, nullptr, root->child*/);
				Node* it = root->child;

				if (it)
				{
					while (it->brother)
						it = it->brother;

					it->brother = whoNode;
				}
				else
				{
					root->child = whoNode;
				}

				fSize++;
			}
			else
			{
				if (!root->child)
				{
					root->child = whoNode;
				}
				else
				{
					Node* it = root->child->brother;

					if (it)
					{
						while (it->brother)
							it = it->brother;

						it->brother = whoNode;
					}
					else
					{
						root->child->brother = whoNode;
					}
				}

				/// Once <who> has been assigned under <boss> we need to rearrange the old relations of <who>
				Node* parentOfWho = whoNode->parent;
				whoNode->parent = root;

				if (parentOfWho->child->data == whoNode->data)
				{
					parentOfWho->child = whoNode->brother;
				}
				else
				{
					Node* it = parentOfWho->child;
					while (it->brother && it->brother->data != whoNode->data)
						it = it->brother;

					it->brother = whoNode->brother;
				}

				whoNode->brother = nullptr;
			}

			return true;
		}

		return insert(root->child, who, boss) || insert(root->brother, who, boss);
	}

	/// @brief Checks if the tree contains a node with given key
	/// @param root - the begining of the tree
	/// @param key - the key of the element that we are searching for
	/// @return true if the element is in the tree, false otherwise
	bool find(Node* root, const string& key) const
	{
		if (!root)
			return false;
		if (root->data == key)
			return true;
		return find(root->brother, key) || find(root->child, key);
	}

	/// @brief Removes node with <key> from the tree.
	/// Follows a specific rule given in the description of the prblem
	/// @param root - the begining of the tree
	/// @param key - the name of the element that we will be removing
	/// @return true if the operation succeeded, false otherwise
	bool remove(Node*& root, const string& key)
	{
		if (!root)
			return false;

		if (root->data == key)
		{
			Node* toDelete = root;
			if (!root->child)
			{
				root = root->brother;
			}
			else if (root->brother)
			{
				Node* parentOfRemovedElement = root->parent;
				Node* it = root->child;
				it->parent = parentOfRemovedElement;
				while (it->brother)
				{
					it = it->brother;
					it->parent = parentOfRemovedElement;
				}

				//it->parent = parentOfRemovedElement;
				it->brother = root->brother;
				root = root->child;
			}
			else
			{
				root = root->child;
			}

			--fSize;
			delete toDelete;
			return true;
		}
		else
		{
			return remove(root->brother, key) || remove(root->child, key);
		}
	}

	/// @brief Finds the height of the tree(childwise)
	/// @param root - the beginning of the tree
	/// @return The height of the tree
	size_t height(Node* root) const
	{
		if (!root)
			return 0;

		return max(1 + height(root->child), height(root->brother));
	}

	/// @brief toString() method of tree
	/// @param root - the beginning of the tree
	/// @return Stringified tree
	string toString(const Node* root) const
	{
		if (!root)
			return "";

		std::queue<const Node*> front;
		string res;
		front.push(root);
		front.push(nullptr);

		while (true)
		{
			const Node* current = front.front();
			front.pop();
			if (!current)
			{
				//res += '\n';
				if (front.empty())
					return res;
				front.push(nullptr);
			}
			else
			{
				priority_queue<Node*, vector<Node*>, LessByName> pq;
				/*for (const Node* it = current->child; it; it = it->brother)
				{
					res += current->data + "-" + el->data + '\n';
				}*/

				for (Node* it = current->child; it; it = it->brother)
				{
					pq.push(it);
				}

				while (!pq.empty())
				{
					Node* el = pq.top();
					res += current->data + "-" + el->data + '\n';
					pq.pop();
					front.push(el);
				}
			}
		}

		return res;
	}

	/// @brief Finds node with name <key> and accesses its parent
	/// @param root - the begining of the tree
	/// @param key - element whoose parent we will search for
	/// @param res - string in which we will store the key of the parent
	/// @return true if the operation succeeded, false otherwise
	string findParentKeyOf(Node* root, const string& key) const
	{
		//if (!root)
		//	return false;

		//if (root->data == key) {
		//	res = root->parent ? root->parent->data : "";
		//	return true;
		//}

		//return findParentKeyOf(root->brother, key, res) || findParentKeyOf(root->child, key, res);

		if (!root)
			return "";

		if (root->data == key)
			return root->parent ? root->parent->data : "";

		string tmp = findParentKeyOf(root->brother, key);
		if (!tmp.empty())
			return tmp;

		tmp = findParentKeyOf(root->child, key);
		return tmp;
	}

	/// @brief Finds element with name <key>
	/// @param root - the beginning of the tree
	/// @param key - the name of the element we will be looking for
	/// @param res - pointer to node in which we will store the found element
	/// @return true if the operation succeeded, false otherwise
	Node* findNodeByKey(Node* root, const string& key)
	{
		if (!root)
			return nullptr;

		if (root->data == key)
			return root;

		Node* tmp = findNodeByKey(root->brother, key);
		if (tmp)
			return tmp;

		tmp = findNodeByKey(root->child, key);
		return tmp;
	}

	/// @brief Const version of findNodeByKey
	const Node* findNodeByKey(const Node* root, const string& key) const
	{
		if (!root)
			return nullptr;

		if (root->data == key)
			return root;

		const Node* tmp = findNodeByKey(root->brother, key);
		if (tmp)
			return tmp;

		tmp = findNodeByKey(root->child, key);
		return tmp;
	}

	/// @brief Given a name of node, counts all of its children
	/// @param root - beginning of the tree
	/// @param key - name of node
	/// @param res - variable in which result is stored
	/// @return true if the operation succeeded, false otherwise
	bool getNumberOfChildrenOf(Node* root, const string& key, size_t& res) const
	{
		if (!root)
			return false;

		if (root->data == key)
		{
			Node* it = root->child;
			if (it)
			{
				res++;
				while (it->brother)
				{
					res++;
					it = it->brother;
				}
			}

			return true;
		}

		return getNumberOfChildrenOf(root->child, key, res) || getNumberOfChildrenOf(root->brother, key, res);
	}

	/// @brief Calculates the size of tree(subtree) with given root
	/// @param root - beginning of the tree
	/// @param level - depth of current iteration
	/// @return number of nodes in tree
	int getSubtreeSize(Node* root, int level = 0) const
	{
		if (!root)
			return 0;

		/// We need to make sure that we enter the child of the root first
		if (level == 0)
			return 1 + getSubtreeSize(root->child, level + 1);
		/// Then we can spread out in all directions
		else
			return (1 + getSubtreeSize(root->child, level + 1) + getSubtreeSize(root->brother, level + 1));
	}

	/// @brief Finds how many nodes have more than <level> descendants
	/// @param root - beginning of tree
	/// @param level - overloadness factor 
	/// @return number of nodes having more than <level> descendants
	int getOverloadedNodes(Node* root, int level) const
	{
		if (!root)
			return 0;

		if (getSubtreeSize(root) - 1 > level)
			return 1 + getOverloadedNodes(root->child, level) + getOverloadedNodes(root->brother, level);

		return getOverloadedNodes(root->child, level) + getOverloadedNodes(root->brother, level);
	}

	/// @brief Calculates the salary of the given node by the formula
	/// 500 * <num_direct_descendants> + 50 * <num_indirect_descendants>
	/// @param root - beginning of tree
	/// @param who - the person whoose salary needs to be calculated
	/// @return salary of node
	unsigned long getSalaryOf(Node* root, const string& who) const
	{
		if (!root)
			return 0;

		if (root->data == who)
		{
			unsigned long res = 0;
			Node* it = root->child;

			while (it)
			{
				res += 500 + 50 * (getSubtreeSize(it) - 1);
				it = it->brother;
			}

			return res;
		}

		return getSalaryOf(root->child, who) + getSalaryOf(root->brother, who);
	}
};