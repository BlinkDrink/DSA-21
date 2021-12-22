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
	Hierarchy(Hierarchy&& r) noexcept : Hierarchy("")
	{
		fTree = std::move(r.fTree);
	}

	Hierarchy(const Hierarchy& r)
	{
		fTree = r.fTree;
	}

	Hierarchy(const string& data)
	{
		vector<string> tokens = splitBy(data, "\n");
		removeEmptyStringsInVector(tokens);

		for (size_t i = 0; i < tokens.size(); i++)
		{
			vector<string> pair = splitBy(tokens[i], "-");
			if (pair.size() != 2)
				throw invalid_argument("Boss-employee relations need to have 2 arguments!");

			trim(pair[0]);
			trim(pair[1]);

			removeEmptyStringsInVector(pair);

			if (i == 0 && pair[0] != "Uspeshnia")
			{
				string msg = "Cannot assign anyone before Uspeshnia!";
				throw invalid_argument(msg);
			}
			else if (i != 0 && !fTree.find(pair[0]))
			{
				string msg = "There is no trace of a boss with name {" + pair[0] + "} in the hierarchy.";
				throw invalid_argument(msg);
			}

			if (!hire(pair[1], pair[0]))
			{
				throw invalid_argument("There was an error hiring this person");
			}
		}
	}

	~Hierarchy() noexcept = default;
	void operator=(const Hierarchy&) = delete;

	string print()const { return fTree.toString(); }

	int longest_chain() const { return fTree.height(); }

	bool find(const string& name) const { return fTree.find(name); }

	int num_employees() const { return fTree.size(); }

	int num_overloaded(int level = 20) const { return fTree.getOverloadedNodes(level); } // ✖

	string manager(const string& name) const { return fTree.findParentKeyOf(name); }

	int num_subordinates(const string& name) const
	{
		if (!find(name))
			return -1;

		return fTree.getNumberOfChildrenOf(name);
	}

	unsigned long getSalary(const string& who) const
	{
		if (!find(who))
			return -1;

		return fTree.getSalaryOf(who);
	}

	bool fire(const string& who)
	{
		if (who == "Uspeshnia")
			return false;

		if (!find(who))
			return false;

		return fTree.remove(who);
	}

	bool hire(const string& who, const string& boss)
	{
		if (boss == "Uspeshnia")
			fTree.insert(who, boss);

		if (!find(boss))
			return false;

		if (manager(boss) == who)
			return false;

		if (manager(who) == boss)
			return true;

		return fTree.insert(who, boss);
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

			if (pos = source.find(delimeter) == string::npos)
			{
				words.push_back(source);
				return words;
			}
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

	string& trim(string& source)
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

	Tree fTree;
};