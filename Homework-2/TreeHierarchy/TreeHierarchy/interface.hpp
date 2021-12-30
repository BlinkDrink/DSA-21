#pragma once
#include<iostream>
#include <string>
#include <vector>
#include "Tree.hpp"
#include "StringHelper.hpp"

using std::string;
using std::vector;
using std::invalid_argument;
using sh = StringHelper;

class Hierarchy
{
public:
	Hierarchy(Hierarchy&& r) noexcept
	{
		fHierarchyName = std::move(r.fHierarchyName);
		fTree = std::move(r.fTree);
	}

	Hierarchy(const Hierarchy& r)
	{
		fHierarchyName = r.fHierarchyName;
		fTree = r.fTree;
	}

	Hierarchy(const string& data)
	{
		vector<string> tokens = sh::splitBy(data, "\n");
		sh::removeEmptyStringsInVector(tokens);

		for (size_t i = 0; i < tokens.size(); i++)
		{
			vector<string> pair = sh::splitBy(tokens[i], "-");
			if (pair.size() != 2)
				throw invalid_argument("Boss-employee relations need to have 2 arguments!");

			sh::trim(pair[0]);
			sh::trim(pair[1]);

			sh::removeEmptyStringsInVector(pair);

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
				throw invalid_argument("There was an error hiring " + pair[1] + " under boss " + pair[0]);
			}
		}
	}

	~Hierarchy() noexcept { fHierarchyName.clear(); }
	void operator=(const Hierarchy&) = delete;

	string print()const { return fTree.toString(); }

	int longest_chain() const { return fTree.height(); }

	bool find(const string& name) const { return fTree.find(name); }

	int num_employees() const { return fTree.size(); }

	int num_overloaded(int level = 20) const { return fTree.getOverloadedNodes(level); }

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

	void incorporate() { fTree.incorporateTree(); }

	void modernize() { fTree.modernizeTree(); }

	Hierarchy join(const Hierarchy& right) const
	{
		Tree merged = fTree.size() > right.fTree.size() ? fTree.join(right.fTree) : right.fTree.join(fTree);
		Hierarchy res(merged.toString());
		return res;
	}

	void setName(const string& name) { fHierarchyName = name; }

	string getName() const { return fHierarchyName; }

	void setSaved(bool isSaved) { fIsSaved = isSaved; }

	bool getSaved() const { return fIsSaved; }

private:
	string fHierarchyName;
	Tree fTree;
	bool fIsSaved = false;
};