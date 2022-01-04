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
		map.add(word, times);
	}

	/// Checks whether word is contained in the container
	bool contains(const std::string& word) const
	{
		return map.contains(word);
	}

	/// Number of occurrances of word 
	size_t countOf(const std::string& word) const
	{
		return map.countOf(word);
	}

	/// Number of unique words in the container
	size_t countOfUniqueWords() const
	{
		return map.countOfUniqueWords();
	}

	/// Returns a multiset of all words in the container
	std::multiset<std::string> words() const
	{
		return map.words();
	}

	vector<string> getUniqueWords() const { return map.getUnqiueWords(); }
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
		using streamiter = std::istream_iterator<std::string>;

		for (streamiter it = streamiter(a); it != streamiter(); it++)
			if (!res.uniqueWords[0].contains(*it))
				res.uniqueWords[0].add(*it);

		for (streamiter it = streamiter(b); it != streamiter(); it++)
			if (!res.uniqueWords[1].contains(*it))
				res.uniqueWords[1].add(*it);

		for (const string& it : res.uniqueWords[0].getUniqueWords())
		{
			if (res.uniqueWords[1].contains(it))
			{
				size_t minFromTwo = std::min(res.uniqueWords[0].countOf(it), res.uniqueWords[1].countOf(it));
				res.commonWords.add(it, minFromTwo);
			}
		}

		return res;
	}

	bool isValidChar(char c) const
	{
		if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
			return false;

		return true;
	}
};