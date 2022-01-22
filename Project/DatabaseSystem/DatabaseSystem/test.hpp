// Searching on a B+ tree in C++

#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include<vector>
#include<list>
#include "RecordPtr.hpp"

using std::list;
using std::pair;
using std::vector;

// BP node
class Node {
private:
	using data = pair<int, RecordPtr>;

private:
	bool fIsLeaf;
	int fOrder;
	pair<int, RecordPtr>* fKeys;
	int fKeysSize;
	Node** ptr;
	friend class BPTree;

public:
	Node(int order, bool isLeaf) : fOrder(order), fIsLeaf(isLeaf), fKeysSize(0)
	{
		fKeys = new data[fOrder];
		ptr = new Node * [fOrder + 1];
	}
};

// BP tree
class BPTree {
private:
	using data = pair<int, RecordPtr>;
	Node* root;
	int fOrder;

	/**
	 * @brief Method used for inserting a kvp that is located neither in the leaves
	 * nor in the root.
	 * @param kvp - key value pair that will be inserted
	 * @param cursor - root of subtree
	 * @param child - child of cursor
	*/
	void insertInternal(data kvp, Node* cursor, Node* child)
	{
		/// Check to see if the current node can contain any more kvp's
		if (cursor->fKeysSize < fOrder)
		{
			int i = 0;
			while (kvp.first > cursor->fKeys[i].first && i < cursor->fKeysSize)
				i++;

			for (int j = cursor->fKeysSize; j > i; j--)
				cursor->fKeys[j] = cursor->fKeys[j - 1];

			for (int j = cursor->fKeysSize + 1; j > i + 1; j--)
				cursor->ptr[j] = cursor->ptr[j - 1];

			cursor->fKeys[i] = kvp;
			cursor->fKeysSize++;
			cursor->ptr[i + 1] = child;
		}
		else
		{
			Node* newInternal = new Node(fOrder, false);
			data* virtualKvp = new data[fOrder + 1];
			Node** virtualPtr = new Node * [fOrder + 2];
			for (int i = 0; i < fOrder; i++)
				virtualKvp[i] = cursor->fKeys[i];

			for (int i = 0; i < fOrder + 1; i++)
				virtualPtr[i] = cursor->ptr[i];

			int i = 0, j;
			while (kvp.first > virtualKvp[i].first && i < fOrder)
				i++;

			for (int j = fOrder + 1; j > i; j--)
				virtualKvp[j] = virtualKvp[j - 1];

			virtualKvp[i] = kvp;

			/// Careful here- previously was fOorder +2
			for (int j = fOrder + 1; j > i + 1; j--)
				virtualPtr[j] = virtualPtr[j - 1];

			virtualPtr[i + 1] = child;
			newInternal->fIsLeaf = false;
			cursor->fKeysSize = (fOrder + 1) / 2;
			newInternal->fKeysSize = fOrder - (fOrder + 1) / 2;

			for (i = 0, j = cursor->fKeysSize + 1; i < newInternal->fKeysSize; i++, j++)
				newInternal->fKeys[i] = virtualKvp[j];

			for (i = 0, j = cursor->fKeysSize + 1; i < newInternal->fKeysSize + 1; i++, j++)
				newInternal->ptr[i] = virtualPtr[j];

			if (cursor == root)
			{
				Node* newRoot = new Node(fOrder, false);
				newRoot->fKeys[0] = cursor->fKeys[cursor->fKeysSize];
				newRoot->ptr[0] = cursor;
				newRoot->ptr[1] = newInternal;
				newRoot->fKeysSize = 1;
				root = newRoot;
			}
			else
			{
				insertInternal(cursor->fKeys[cursor->fKeysSize], findParent(root, cursor), newInternal);
			}
		}
	}

	Node* findParent(Node* cursor, Node* child)
	{
		Node* parent = nullptr;
		if (cursor->fIsLeaf || (cursor->ptr[0])->fIsLeaf)
			return nullptr;

		for (int i = 0; i < cursor->fKeysSize + 1; i++)
		{
			if (cursor->ptr[i] == child)
			{
				return cursor;
			}
			else
			{
				parent = findParent(cursor->ptr[i], child);
				if (parent != nullptr)
					return parent;
			}
		}

		return parent;
	}

public:
	BPTree(int order) : root(nullptr), fOrder(order) {}

	Node* search(int key)
	{
		if (root == nullptr)
		{
			return nullptr;
		}
		else
		{
			Node* cursor = root;
			while (cursor->fIsLeaf == false) {
				for (int i = 0; i < cursor->fKeysSize; i++) {
					if (key < cursor->fKeys[i].first) {
						cursor = cursor->ptr[i];
						break;
					}

					if (i == cursor->fKeysSize - 1) {
						cursor = cursor->ptr[i + 1];
						break;
					}
				}
			}
			for (int i = 0; i < cursor->fKeysSize; i++) {
				if (cursor->fKeys[i].first == key)
					return cursor;
			}

			return nullptr;
		}
	}

	void insert(data kvp)
	{
		if (root == nullptr) {
			root = new Node(fOrder, true);
			root->fKeys[0] = kvp;
			root->fIsLeaf = true;
			root->fKeysSize = 1;
		}
		else {
			Node* cursor = root;
			Node* parent = nullptr;
			while (cursor->fIsLeaf == false)
			{
				parent = cursor;
				for (int i = 0; i < cursor->fKeysSize; i++)
				{
					if (kvp.first < cursor->fKeys[i].first)
					{
						cursor = cursor->ptr[i];
						break;
					}

					if (i == cursor->fKeysSize - 1)
					{
						cursor = cursor->ptr[i + 1];
						break;
					}
				}
			}
			if (cursor->fKeysSize < fOrder) {
				int i = 0;

				while (kvp.first > cursor->fKeys[i].first && i < cursor->fKeysSize)
					i++;

				for (int j = cursor->fKeysSize; j > i; j--)
				{
					cursor->fKeys[j] = cursor->fKeys[j - 1];
				}

				cursor->fKeys[i] = kvp;
				cursor->fKeysSize++;
				cursor->ptr[cursor->fKeysSize] = cursor->ptr[cursor->fKeysSize - 1];
				cursor->ptr[cursor->fKeysSize - 1] = nullptr;
			}
			else
			{
				Node* newLeaf = new Node(fOrder, true);
				data* virtualNode = new data[fOrder + 1];

				for (int i = 0; i < fOrder; i++)
					virtualNode[i] = cursor->fKeys[i];

				int i = 0, j;
				while (kvp.first > virtualNode[i].first && i < fOrder)
					i++;

				for (int j = fOrder + 1; j > i; j--)
					virtualNode[j] = virtualNode[j - 1];

				virtualNode[i] = kvp;
				newLeaf->fIsLeaf = true;
				cursor->fKeysSize = (fOrder + 1) / 2;
				newLeaf->fKeysSize = fOrder + 1 - (fOrder + 1) / 2;
				cursor->ptr[cursor->fKeysSize] = newLeaf;
				newLeaf->ptr[newLeaf->fKeysSize] = cursor->ptr[fOrder];
				cursor->ptr[fOrder] = nullptr;

				for (i = 0; i < cursor->fKeysSize; i++)
					cursor->fKeys[i] = virtualNode[i];

				for (i = 0, j = cursor->fKeysSize; i < newLeaf->fKeysSize; i++, j++)
					newLeaf->fKeys[i] = virtualNode[j];

				if (cursor == root)
				{
					Node* newRoot = new Node(fOrder, false);
					newRoot->fKeys[0] = newLeaf->fKeys[0];
					newRoot->ptr[0] = cursor;
					newRoot->ptr[1] = newLeaf;
					newRoot->fKeysSize = 1;
					root = newRoot;
				}
				else {
					insertInternal(newLeaf->fKeys[0], parent, newLeaf);
				}
			}
		}
	}

	//void display(Node*) {}

	Node* getRoot()
	{
		return root;
	}

	void clear(Node* cursor)
	{
		if (cursor != nullptr)
		{
			if (cursor->fIsLeaf != true)
			{
				for (int i = 0; i < cursor->fKeysSize + 1; i++)
				{
					clear(cursor->ptr[i]);
				}
			}

			delete[] cursor->fKeys;
			delete[] cursor->ptr;
			delete cursor;
		}
	}
};

//// Print the tree
//void BPTree::display(Node* cursor) {
//	if (cursor != nullptr) {
//		for (int i = 0; i < cursor->fKeysSize; i++) {
//			cout << cursor->fKeys[i] << " ";
//		}
//		cout << "\n";
//		if (cursor->fIsLeaf != true) {
//			for (int i = 0; i < cursor->fKeysSize + 1; i++) {
//				display(cursor->ptr[i]);
//			}
//		}
//	}
//}



