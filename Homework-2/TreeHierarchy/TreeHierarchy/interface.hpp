#pragma once
#include<iostream>
#include <string>
#include <vector>
#include "Tree.hpp"

using std::string;
using std::vector;
using std::invalid_argument;

class Hierarchy
{
public:
	Hierarchy(Hierarchy&& r) noexcept {}

	Hierarchy(const Hierarchy& r) {}

	Hierarchy(const string& data)
	{
		vector<string> tokens = splitBy(data, "\n");
		removeEmptyStringsInVector(tokens);

		for (size_t i = 0; i < tokens.size(); i++)
		{
			vector<string> pair = splitBy(tokens[i], "-");
			removeEmptyStringsInVector(pair);

			if (i == 0 && pair[0] != "Uspeshnia")
			{
				string msg = "There was an error during the input! Please check it out";
				throw invalid_argument(msg);
			}
			else if (i != 0 && !fHierarchy.find(pair[0]))
			{
				string msg = "There is no trace of a boss with name {" + pair[0] + "} in the hierarchy.";
				throw invalid_argument(msg);
			}

			fHierarchy.insert(pair[1], pair[0]);
		}
	}

	~Hierarchy() noexcept = default;
	void operator=(const Hierarchy&) = delete;

	string print()const { return ""; }

	int longest_chain() const { return 0; }

	bool find(const string& name) const { return fHierarchy.find(name); } /// Check

	int num_employees() const { return fHierarchy.size(); }
	int num_overloaded(int level = 20) const { return 0; }

	string manager(const string& name) const { return ""; }
	int num_subordinates(const string& name) const { return 0; }
	unsigned long getSalary(const string& who) const { return 0; }

	bool fire(const string& who)
	{
		if (who == "Uspehnia")
			return false;

		if (!find(who))
			return false;

		fHierarchy.remove(who);
		return true;
	}

	bool hire(const string& who, const string& boss)
	{
		if (!find(boss))
			return false;

		fHierarchy.insert(who, boss);
		return true;
	}

	void incorporate() {}
	void modernize() {}

	Hierarchy join(const Hierarchy& right) const { return Hierarchy(""); }

	//If you need it - add more public methods here
private:
	//Add whatever you need here

	vector<string> splitBy(string source, const string& delimeter)
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
		}

		return words;
	}

	void removeEmptyStringsInVector(vector<string>& parts)
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

	Tree fHierarchy;
};