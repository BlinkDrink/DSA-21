#pragma once
#include<iostream>
#include "../StoreTests/interface.h"

template<typename DataType>
class List {
public:
	struct Node
	{
		Node(const DataType& data, Node* prevNode = nullptr, Node* nextNode = nullptr)
			: fData(data), fpPrev(prevNode), fpNext(nextNode)
		{}

		DataType  fData;
		Node* fpPrev;
		Node* fpNext;
	};

	List() : fpHead(nullptr), fpTail(nullptr), fSize(0) {}

	~List()
	{
		this->clear();
	}

	void push_front(const DataType& elem)
	{
		if (fpHead == nullptr)
		{
			fpHead = new Node(elem);
			fpTail = fpHead;
		}
		else
		{
			fpHead->fpPrev = new Node(elem, nullptr, fpHead);
			fpHead = fpHead->fpPrev;
		}

		fSize++;
	}

	void push_back(const DataType& elem)
	{
		if (fpHead == nullptr)
		{
			fpHead = new Node(elem);
			fpTail = fpHead;
		}
		else
		{
			fpTail->fpNext = new Node(elem, fpTail, nullptr);
			fpTail = fpTail->fpNext;
		}

		fSize++;
	}

	void pop_front()
	{
		if (this->empty())
			throw std::logic_error("List::pop_front() cannot remove element from an empty list.");

		if (fpHead == fpTail)
		{
			delete  fpHead;
			fpHead = nullptr;
			fpTail = nullptr;
		}
		else
		{
			fpHead = fpHead->fpNext;
			delete  fpHead->fpPrev;
			fpHead->fpPrev = nullptr;
		}

		fSize--;
	}

	void pop_back()
	{
		if (this->empty())
			throw std::logic_error("List::pop_front() cannot remove element from an empty list.");

		if (fpHead == fpTail)
		{
			delete  fpHead;
			fpHead = nullptr;
			fpTail = nullptr;
		}
		else
		{
			fpTail = fpTail->fpPrev;
			delete fpTail->fpNext;
			fpTail->fpNext = nullptr;
		}

		fSize--;
	}

	Node* erase(Node* pointer)
	{
		if (this->empty())
			throw std::logic_error("List::erase() cannot erase from an empty list.");

		if (pointer == fpHead)
		{
			this->pop_front();
			return fpHead;
		}

		if (pointer == fpTail)
		{
			this->pop_back();
			return nullptr;
		}

		Node* pPrev = pointer->fpPrev;
		Node* pNext = pointer->fpNext;

		pPrev->fpNext = pNext;
		pNext->fpPrev = pPrev;

		delete  pointer;

		fSize--;

		return pNext;
	}

	void clear()
	{
		while (!this->empty())
		{
			this->pop_front();
		}
	}

	List(const List<DataType>& other) = delete;
	List& operator=(const List<DataType>& other) = delete;

	const DataType& front() const { return fpHead->fData; }
	DataType& front() { return fpHead->fData; }

	DataType& back() { return fpTail->fData; }
	const DataType& back() const { return fpTail->fData; }

	Node* begin() { return fpHead; }
	const Node* begin() const { return fpHead; }

	Node* end() { return fpTail; }
	const Node* end() const { return fpTail; }

	size_t size() const { return fSize; }
	bool empty() const { return fSize == 0; }

private:
	Node* fpHead;
	Node* fpTail;
	size_t fSize;
};