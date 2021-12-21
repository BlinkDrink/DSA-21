#pragma once
#include<list>
#include<string>

using std::string;
using std::list;
using std::max;

class Tree
{
public:
	/// @brief Object lifetime
	Tree() : root(nullptr), fSize(0) {}

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

	/// @brief Wrapper of inner method find
	/// @param key - the key of the element we will be searching for
	/// @return true if the element is contained, false otherwise
	bool find(const string& key) const { return find(root, key); }

	/// @brief Wrapper of inner method insert
	/// @param who - the employee to be added underneath <boss>
	/// @param boss - the boss of <who>
	/// @return true if the operation succeeded, false otherwise
	bool insert(const string& who, const string& boss) { return insert(root, who, boss); fSize++; }

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

	string getParentKeyOf(const string& key) { return getParentKeyOf(root, key)->parent->data; }

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
		if (!root)
			return false;

		if (!find(boss))
			return false;

		if (root->data == boss)
		{
			if (find(who))
				remove(who);

			Node* it = root->child;

			if (it)
			{
				while (it->brother)
					it = it->brother;

				it->brother = new Node(who, root);
			}
			else
			{
				root->child = new Node(who, root);
			}

			fSize++;
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

	Node* findNodeByKey(Node* root, const string& key) const
	{
		if (!root)
			return nullptr;
		if (root->data == key) return root;
		return findNodeByKey(root->child, key) || findNodeByKey(root->brother, key);
	}
};
