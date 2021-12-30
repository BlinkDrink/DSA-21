#pragma once
#include<vector>
using std::vector;

template<class T, class Comparator>
class PriorityQueue
{
public:
	using iterator = typename vector<T>::iterator;
	using diff_type = typename iterator::difference_type;

public:
	PriorityQueue() = default;

public:

	void push(T const& elem)
	{
		fContainer.push_back(elem);
		iterator itLast = last();
		iterator itParent = parent(itLast);

		while (itLast != begin() && !fComparator(*itLast, *itParent))
		{
			std::swap(*itLast, *itParent);

			itLast = itParent;
			itParent = parent(itLast);
		}
	}

	void pop()
	{
		std::swap(*begin(), *last());
		fContainer.pop_back();

		iterator itBegin = begin();
		while (itBegin != end())
		{
			iterator itLeft = left_child(itBegin);
			iterator itRight = right_child(itBegin);

			iterator itSwap = itBegin;
			if (itLeft != end() && fComparator(*itSwap, *itLeft))
				itSwap = itLeft;

			if (itRight != end() && fComparator(*itSwap, *itRight))
				itSwap = itRight;

			if (itSwap != itBegin)
			{
				std::swap(*itBegin, *itSwap);
				itBegin = itSwap;
			}
			else
			{
				itBegin = end();
			}
		}
	}

	T const& top()   const { return fContainer.front(); }

	bool empty() const { return fContainer.empty(); }
	size_t size()  const { return fContainer.size(); }

private:
	iterator parent(iterator it)
	{
		if (it == begin())
			return end();

		return iter((index(it) - 1) / 2);
	}

	iterator left_child(iterator it)
	{
		return iter(2 * index(it) + 1);
	}

	iterator right_child(iterator it)
	{
		return iter(2 * index(it) + 2);
	}

	diff_type index(iterator it)
	{
		return std::distance(begin(), it);
	}

	iterator iter(size_t index)
	{
		if (index >= size())
			return end();

		iterator it = begin();
		std::advance(it, index);
		return it;
	}

	iterator begin() { return fContainer.begin(); }
	iterator end() { return fContainer.end(); }

	iterator last() { return --fContainer.end(); }

private:
	vector<T> fContainer;
	Comparator fComparator;
};

