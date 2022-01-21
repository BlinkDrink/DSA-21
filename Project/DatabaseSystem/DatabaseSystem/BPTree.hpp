#pragma once
#include<vector>
#include "RecordPtr.hpp"
using std::vector;


template<typename T>
struct Node
{
public:
	Node(size_t order) : fOrder(order) {}

	bool isRoot() { return fIsRoot; }

	void setRoot(bool isRoot) { fIsRoot = isRoot; }

	T getKey(size_t index) { return fKeys[index]; }

	void setKey(size_t index, T value) { fKeys[index] = value; }

	T getLastKey() { return fKeys[fKeys.size() - 1]; }

	T getFirstKey() { return fKeys.front(); }

	bool isFull() { return fKeys.size() == order; }

	virtual size_t minKeys() = 0;

	virtual RecordPtr search(T key) = 0;

	virtual bool deleteNode(T key, InnerNode<T> parent, size_t indexOfParent) = 0;

	virtual InnerNode<T> insert(T key, Ref recordReference, InnerNode<T> parent, size_t indexOfParent) = 0;

protected:
	vector<T> fKeys;
	size_t fOrder;
	bool fIsRoot;
};

template<typename T>
struct InnerNode : public Node<T>
{
public:
	InnerNode(size_t order) : Node(order) {}

	Node<T> getChild(size_t index) { return children[index]; }

	void setChild(size_t index, Node<T> child) { children[index] = child; }

	Node<T> getFirstChild() { return children.front(); }

	Node<T> getLastChild() { return children.back(); }

	virtual size_t minKeys() const override
	{
		if (isRoot())
			return 1;

		return (fOrder + 2) / 2 - 1;
	}
private:
	vector<Node<T>> children;

};