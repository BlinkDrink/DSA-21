#pragma once
#include "HashMap.hpp"

///
/// Represents a multiset of words
///
/// If you need to see how it is intended to be used,
/// check out the corresponding unit tests
///
/// Implement all methods of this class
/// 
class WordsMultiset {
public:
	/// Adds times occurences of word to the container
	///
	/// For example, add("abc") adds the word "abc" once,
	/// while add("abc", 4) adds 4 occurrances.
	void add(const std::string& word, size_t times = 1)
	{
		if (!contains(word))
			map.add(word, times);
		else
			map[word] += times;
	}

	/// Checks whether word is contained in the container
	bool contains(const std::string& word) const { return map.contains(word); }

	/// Number of occurrances of word 
	size_t countOf(const std::string& word) const { return map.countOf(word); }

	/// Number of unique words in the container
	size_t countOfUniqueWords() const { return map.countOfUniqueWords(); }

	/// Returns a multiset of all words in the container
	std::multiset<std::string> words() const { return map.words(); }

	/// @brief operator[] used to access given key's value
	/// @param key - key of element
	/// @return the value corresponding to the given key in the hash-table
	size_t& operator[](const string& key) { return map[key]; }

	/// @return Count of all words in the multiset
	size_t countOfAllWords() const { return map.countOfAllWords(); }

private:
	HashMap map;
};

///
/// Results of the comparison of two streams of words
/// DO NOT modify this class
/// If you need to see how it is intended to be used,
/// check out the corresponding unit tests
///
class ComparisonReport {
public:
	/// A multiset of all words that exist in both streams
	WordsMultiset commonWords;

	/// Multisets of words unique to the two streams
	/// The first element of the array contains the words that are unique
	/// to the first stream (a) and the second one -- to the second stream (b)
	WordsMultiset uniqueWords[2];
};

/// 
/// Can be used to compare two streams of words
///
class Comparator {
public:
	ComparisonReport compare(std::istream& a, std::istream& b)
	{
		ComparisonReport res;

		while (!a.eof())
		{
			string word;
			char c = a.get();

			if (a.eof())
				break;

			while (!isValid(c) && !a.eof())
				c = a.get();

			while (isValid(c) && !a.eof())
			{
				word += c;
				c = a.get();
			}

			if (word.size())
				res.uniqueWords[0][word]++;
		}

		while (!b.eof())
		{
			string word;
			char c = b.get();

			if (b.eof())
				break;

			while (!isValid(c) && !b.eof())
			{
				c = b.get();
			}

			while (isValid(c) && !b.eof())
			{
				word += c;
				c = b.get();
			}

			if (word.size())
			{
				if (res.uniqueWords[0].countOf(word))
				{
					res.uniqueWords[0][word]--;
					res.commonWords[word]++;
				}
				else
				{
					res.uniqueWords[1][word]++;
				}
			}
		}

		return res;
	}

	/// @brief Used to check if the given character is a whitespace or not
	/// @param c - character to check
	/// @return true if not whitespace, false otherwise
	bool isValid(char c)
	{
		if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
			return false;

		return true;
	}
};