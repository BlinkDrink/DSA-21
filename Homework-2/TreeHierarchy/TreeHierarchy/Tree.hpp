#pragma once
#include<list>
#include<string>

using std::string;
using std::list;

class Tree
{
public:
	/// @brief Object lifetime
	Tree() : root(nullptr), fSize(0) {}

	~Tree()
	{
		clear(root);
		root = nullptr;
	}

	Tree(const Tree& other)
	{
		root = copy(other.root);
		fSize = other.fSize;
	}

public:
	/// @brief Public methods
	bool find(const string& key) const { return find(root, key); }

	bool insert(const string& who, const string& boss) { return insert(root, who, boss); fSize++; }

	void remove(const string& key) { remove(root, key); }

	size_t size() const { return fSize; };

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
	void clear(Node*& root)
	{
		if (root)
		{
			for (Node*& it : root->children)
			{
				clear(it);
			}

			delete root;
		}

		fSize = 0;
	}

	bool insert(Node*& root, const string& who, const string& boss)
	{
		if (!root)
		{
			root = new Node(boss);
			fSize++;
		}

		Node* boss_node = findPerson(root, boss);
		boss_node->children.push_back(new Node(who, boss_node));
	}

	Node* findPerson(Node* root, const string& key)
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
	}

	bool find(Node* root, const string& key) const
	{
		if (!root)
			return false;

		if (root->key == key) return true;

		for (Node*& child : root->children)
			if (child->key == key)
				return true;

		for (Node*& child : root->children)
		{
			bool res = find(child, key);
			if (res)
				return true;
		}

		return false;
	}

	void remove(Node*& root, const string& key)
	{
		Node* person = findPerson(root, key);

		if (person)
		{
			if (!person->children.empty())
			{
				for (Node* it : person->children)
				{
					person->parent->children.push_back(it);
					it->parent = person->parent;
				}
				person->parent->children.remove(person);
				fSize--;
			}
			else
			{
				person->parent->children.remove(person);
			}
		}
	}

	Node* copy(Node* root)
	{

	}
};
