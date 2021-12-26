#pragma once
#include <string>
#include <vector>

using std::string;
using std::vector;

/// @brief Static class used only to edit strings
class StringHelper
{
private:
	StringHelper();
public:
	StringHelper(const StringHelper& sh) = delete;
	StringHelper& operator=(const StringHelper& sh) = delete;
	StringHelper(StringHelper&& sh) = delete;
	StringHelper& operator=(StringHelper&& sh) = delete;

	/// @brief Splits a given string(source) into parts by given delimeter
	/// 
	/// @param source    - string to be splited
	/// @param delimeter - what to split it by
	/// 
	/// @returns vector<string> filled with the splited parts
	static vector<string> splitBy(string source, const string& delimeter)
	{
		vector<string> words;
		size_t pos = 0;

		if (source.find(delimeter) == string::npos && !source.empty())
		{
			words.push_back(source);
			return words;
		}

		while ((pos = source.find(delimeter)) != string::npos)
		{
			words.push_back(source.substr(0, pos));
			source.erase(0, pos + delimeter.length());

			if (pos = source.find(delimeter) == string::npos)
			{
				words.push_back(source);
				return words;
			}
		}

		return words;
	}

	/// @brief Removes leading and ending whitespaces
	/// 
	/// @param source - string to be trimmed
	/// @returns trimmed source
	static string& trim(string& source)
	{
		if (source.empty())
		{
			return source;
		}

		size_t start = 0;

		while (source[start] == ' ')
		{
			source.erase(source.begin());
		}

		size_t end = source.size() > 0 ? source.size() - 1 : 0;

		while (source[end] == ' ')
		{
			source.erase(source.begin() + end);
			end--;
		}

		return source;
	}

	/// @brief Removes all empty strings from a vector of strings
	///
	/// @param parts - vector of strings to have it's empty strings removed
	static void removeEmptyStringsInVector(vector<string>& parts)
	{
		for (size_t i = 0; i < parts.size(); i++)
		{
			if (parts[i].empty())
			{
				parts.erase(parts.begin() + i);
				i--;
			}
		}
	}

	/// @brief Converts all small alphabetic characters to capital characters.
	/// 
	/// @param str - string to be Uppered
	/// @returns string with all lowercase letters converted to uppercase letters
	static const string toUpper(const string& str)
	{
		string item;
		item = str;
		for (size_t i = 0; i < item.size(); i++)
		{
			if (item[i] >= 'a' && item[i] <= 'z')
			{
				item[i] -= 'a' - 'A';
			}
		}

		return item;
	}
};
