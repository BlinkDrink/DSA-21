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

	/// @brief Wrapper of inner method findParentKeyOf
	/// @param key - name of item
	/// @return The parent of the node with name <key>
	string findParentKeyOf(const string& key) const
	{
		string res;
		findParentKeyOf(root, key, res);
		return res;
	}

	/// @brief Wrapper of inner method getNumberOfChildrenOf
	/// @param key - the key of the node for which we will determine the number of children
	/// @return the number of children
	size_t getNumberOfChildrenOf(const string& key) const
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

	size_t calculateSalaryOfEmployee(const string& key)
	{
		return 500 * getNumberOfChildrenOf(key) + calculateSalaryOfEmployee(key);
	}

private:
	/// @brief Tree node descriptor
	struct Node
	{
		string data;
		Node* child, * brother, * parent;


		Node(const string& data, Node* parent = nullptr, Node* child = nullptr, Node* brother = nullptr)
			: data(data), parent(parent), child(child), brother(brother)
		{
		}

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
		if (fSize == 0 && boss == "Uspeshnia")
		{
			root = new Node(boss);
			root->child = new Node(who, root);
			fSize += 2;
			return true;
		}

		if (!root)
			return false;

		/// The boss is found and now we have to attach them the new employee
		if (root->data == boss)
		{
			Node* whoNode = nullptr;
			/// Tree does not contain element who so we add it directly
			if (!find(who))
			{
				whoNode = new Node(who, root/*, nullptr, root->child*/);
				Node* it = root->child;

				if (it)
				{
					while (it->brother)
					{
						it = it->brother;
					}

					it->brother = whoNode;
				}
				else
				{
					root->child = whoNode;
				}

				fSize++;
			}
			else /// Tree contains who, so we need to rearrange nodes
			{
				getElementByName(this->root, who, whoNode);

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

				Node* parentOfWho = nullptr;
				getElementByName(this->root, whoNode->parent->data, parentOfWho); // Uspeshnia
				whoNode->parent = root; // Set parent G to be Z

				if (parentOfWho->child->data == whoNode->data)
					parentOfWho->child = whoNode->brother; // Izmestvame bratqtq s 1 nalqvo
				else
				{
					whoNode = whoNode->brother;
				}

				/*if (whoNode->brother)
					whoNode = whoNode->brother;
				else
					whoNode = nullptr;*/
			}

			return true;
		}

		return insert(root->child, who, boss) || insert(root->brother, who, boss);
	}

	/*Node* findPerson(Node* root, const string& key)
	{
		if (!root)
			return nullptr;

		if (root->key == key) return root;

		/// Search in root's children
		for (Node*& child : root->children)
			if (child->key == key)
				return child;

		/// Search in root's children's children
		for (Node*& child : root->children)
		{
			Node* n = findPerson(child, key);
			if (n != nullptr)
				return n;
		}

		return nullptr;
	}*/

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
	bool findParentKeyOf(Node* root, const string& key, string& res) const
	{
		if (!root)
			return false;

		if (root->data == key) {
			res = root->parent ? root->parent->data : "";
			return true;
		}

		return findParentKeyOf(root->brother, key, res) || findParentKeyOf(root->child, key, res);
	}

	/// @brief Finds element with name <key>
	/// @param root - the beginning of the tree
	/// @param key - the name of the element we will be looking for
	/// @param res - pointer to node in which we will store the found element
	/// @return true if the operation succeeded, false otherwise
	bool getElementByName(Node* root, const string& key, Node*& res)
	{
		if (!root)
			return false;

		if (root->data == key) {
			res = root;
			return true;
		}

		return getElementByName(root->brother, key, res) || getElementByName(root->child, key, res);
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

	size_t calculateSalaryOfEmployee(Node* root)
	{
		if (!root)
			return 0;

		return 50 * (calculateSalaryOfEmployee(root->brother) + calculateSalaryOfEmployee(root->child));
	}
};