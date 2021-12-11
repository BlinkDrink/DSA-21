#pragma once
#include<iostream>

using std::underflow_error;

template <class DataType>
class LinkedQueue
{
public:
	LinkedQueue() : begin(nullptr), end(nullptr) {}

	LinkedQueue(const LinkedQueue<DataType>& rhs) = delete;
	LinkedQueue<DataType>& operator=(const LinkedQueue<DataType>& rhs) = delete;

	~LinkedQueue()
	{
		this->clear();
	}

	void push(const DataType& data)
	{
		LinkedQueueNode* n = new LinkedQueueNode(data);

		if (empty())
		{
			begin = n;
		}
		else
		{
			end->next = n;
		}

		end = n;
	}

	DataType pop()
	{
		if (empty())
			throw std::underflow_error("LinkedQueue::empty() cannot pop on an empty queue");

		DataType result = top();
		LinkedQueueNode* tmp = begin;

		begin = begin->next;
		delete tmp;

		return result;
	}

	const DataType& top() const
	{
		if (!empty())
			return begin->data;

		throw underflow_error("LinkedQueue::first() cannot be called on empty queue");
	}

	bool empty() const { return begin == nullptr; }

private:
	void clear() {
		while (!empty())
		{
			LinkedQueueNode* tmp = begin;
			begin = begin->next;
			delete tmp;
		}
	}

private:
	struct LinkedQueueNode
	{
		LinkedQueueNode(const DataType& data, LinkedQueueNode* nextNode = nullptr)
			: data(data), next(nextNode) {}

		DataType data;
		LinkedQueueNode* next;
	};

	LinkedQueueNode* begin;
	LinkedQueueNode* end;
};