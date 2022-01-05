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
	/// while add("abc", 4) adds 4 occurrances.
	void add(const std::string& word, size_t times = 1)
	{
		this->insert({ word, times });
	}

	/// Checks whether word is contained in the container
	bool contains(const std::string& word) const
	{
		if (this->find(word))
			return true;

		return false;
	}

	/// Number of occurrances of word 
	size_t countOf(const std::string& word) const
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

	float max_load_factor() const { return fMaxLoadFactor; }

	size_t size() const { return fSize; }

	size_t bucket_count() const { return fArr.size(); }

	void decrementValueAtKey(const string& key, int how_much)
	{
		if (contains(key)) {
			find(key)->fValue -= how_much;
			fSize -= how_much;
		}

		if (countOf(key) == 0)
			fUniqueWordsCount--;
	}

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
	size_t hash(const string& key) const { return std::hash<string>{}(key) % this->bucket_count(); }

	const Data* find(const string& word) const
	{
		size_t index = this->hash(word);

		for (const Data& elem : fArr[index])
			if (elem.fKey == word)
				return &elem;

		return nullptr;
	}

	Data* find(const string& word)
	{
		return const_cast<Data*>(std::as_const(*this).find(word));
	}

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