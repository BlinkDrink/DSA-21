#pragma once
#include <iostream>
#include<string>
#include<set>
#include<vector>
#include<forward_list>

using std::vector;
using std::string;
using std::forward_list;

class HashMap
{
private:
	/// @brief Default values for the initialization
	const size_t DEFAULT_BUCKET_COUNT = 8;
	const float DEFAULT_MAX_LOAD_FACTOR = 1;

public:
	/// Adds times occurences of word to the container
	///
	/// For example, add("abc") adds the word "abc" once,
	/// while add("abc", 4) adds 4 occurrences.
	void add(const string& word, size_t times = 1)
	{
		this->insert({ word, times });
	}

	/// Checks whether word is contained in the container
	bool contains(const string& word) const
	{
		if (this->find(word))
			return true;

		return false;
	}

	/// Number of occurrances of word 
	size_t countOf(const string& word) const
	{
		const Data* res = find(word);
		return res ? res->fValue : 0;
	}

	/// Number of unique words in the container
	size_t countOfUniqueWords() const
	{
		return fUniqueWordsCount;
	}

	/// Returns a multiset of all words in the container
	std::multiset<std::string> words() const
	{
		std::multiset<string> res;

		for (const forward_list<Data>& l : fArr)
			for (const Data& elem : l)
				for (size_t i = 0; i < elem.fValue; i++)
					res.insert(elem.fKey);

		return res;
	}

public:
	/// @brief Descriptor of hash-table element
	struct Data
	{
		string fKey;
		size_t fValue;
	};

	HashMap()
		: fArr(DEFAULT_BUCKET_COUNT)
		, fUniqueWordsCount(0)
		, fSize(0)
		, fMaxLoadFactor(DEFAULT_MAX_LOAD_FACTOR)
	{}

	/// @brief Getter
	/// @return max load factor of the hash-table
	float max_load_factor() const { return fMaxLoadFactor; }

	/// @brief Getter
	/// @return number of elements in the hash-table
	size_t size() const { return fSize; }

	/// @brief Getter
	/// @return the number of separate chains(number forward_lists in the vector)
	size_t bucket_count() const { return fArr.size(); }

	/// @brief Decrements the value at key with <how_much>
	/// @param key - key of element
	/// @param how_much - how many times to decrement
	void decrementValueAtKey(const string& key, int how_much)
	{
		if (contains(key)) {
			find(key)->fValue -= how_much;
			fSize -= how_much;
		}

		if (countOf(key) == 0)
			fUniqueWordsCount--;
	}

	/// @brief Rehashing function, used to resize the inner vector of forward_lists
	/// @param bucketsCount - the number of buckets to the hash-table will be resized
	void rehash(size_t bucketsCount)
	{
		size_t minBuckets = std::ceil(this->size() / this->max_load_factor());
		if (bucketsCount < minBuckets)
			bucketsCount = minBuckets;

		if (this->bucket_count() == bucketsCount)
			return;

		vector<forward_list<Data>> newArr(bucketsCount);
		std::swap(fArr, newArr);
		fUniqueWordsCount = 0;
		fSize = 0;

		for (const std::forward_list< Data >& list : newArr)
			for (const Data& elem : list)
				this->add(elem.fKey, elem.fValue);
	}

private:
	/// @brief Private methods

	/// @brief Hashing function, using std::hash and % size to find the bucket of each element
	/// @param key - string which will be hashed
	/// @return the hashed index
	size_t hash(const string& key) const { return std::hash<string>{}(key) % this->bucket_count(); }

	/// @brief Finds the element with given key
	/// @param key - key of element
	/// @return the element if found, nullptr otherwise
	const Data* find(const string& key) const
	{
		size_t index = this->hash(key);

		for (const Data& elem : fArr[index])
			if (elem.fKey == key)
				return &elem;

		return nullptr;
	}

	/// @brief Non const version of method find()
	/// @param key - key of element
	/// @return the element if found, nullptr otherwise
	Data* find(const string& key) { return const_cast<Data*>(std::as_const(*this).find(key)); }

	/// @brief Inserts Key-Value pair in the hash-table, resizes hash if max load factor is exceeded
	/// If pair is existing in the hash-table then simply increment its value by pair.fValue amount
	/// @param pair - Key-Value pair
	/// @return the added element by &
	Data* insert(const Data& pair)
	{
		size_t index = this->hash(pair.fKey);

		if ((float)(this->size() + pair.fValue) / this->bucket_count() > this->max_load_factor())
			this->rehash(this->bucket_count() * 2);

		if (this->contains(pair.fKey))
		{
			find(pair.fKey)->fValue += pair.fValue;
		}
		else {
			fArr[index].push_front(pair);
			++fUniqueWordsCount;
		}

		fSize += pair.fValue;
		return &fArr[index].front();
	}

private:
	/// @brief Private data members
	vector<forward_list<Data>> fArr;
	size_t fUniqueWordsCount;
	size_t fSize;
	float fMaxLoadFactor;
};