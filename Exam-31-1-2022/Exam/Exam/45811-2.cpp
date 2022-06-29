// 
// СУ "Св. Климент Охридски"
// Факултет по математика и информатика
// 
// Курсове Структури от данни и програмиране
//         Структури от данни
// Зимен семестър 2021/22 г.
// 
// Практически изпит
// 
// Име: Петко Иванов Петков
// ФН: 45811
// Специалност: Информатика
// Курс: 2
// Административна група: 2
// Ден, в който се явявате на изпита: 31/1/2022
// Начален час на изпита: (тук попълнете часа за вашата група) 9:00
// Кой компилатор използвате: (тук посочете едно от Visual C++, GCC или Clang) Visual C++
//

// (Можете да изтриете този коментар след като го прочетете)
// Редът по-долу е специфичен за Visual C++.
// Ако използвате друг компилатор, можете да го изтриете.
// Тъй като strlen, strcmp и т.н. са дефинирани като deprecated,
// имате две възможности:
//
// * да използвате безопасните версии на тези функции
//   (strlen_s, strcmp_s и т.н.). В този случай можете да
//   изтриете дадената по-долу #define директива.
//
// * да дефинирате _CRT_SECURE_NO_WARNINGS преди да
//   включите съответните header файлове от стандартната
//   библиотека.
//
#define _CRT_SECURE_NO_WARNINGS 

#include <cassert>
#include <iostream>
#include<vector>
#include<fstream>
#include<queue>
#include<string>

using std::queue;
using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::getline;
using std::cout;
using std::cin;
using std::stoi;

struct Node
{
	int key;
	vector<Node*> children;

	Node(int key) : key(key) {}

	bool addChild(Node* child)
	{
		for (size_t i = 0; i < children.size(); i++)
			if (children[i]->key == child->key)
				return false;

		children.push_back(child);
		return true;
	}
};

Node* find(Node* root, int data, int level, int& repeatedTimes)
{
	if (!root)
		return nullptr;

	if (root->key == data && level == 0)
	{
		repeatedTimes--;
		if (repeatedTimes == 0)
			return root;
	}

	for (Node* child : root->children)
	{
		Node* res = find(child, data, level - 1, repeatedTimes);
		if (res)
			return res;
	}

	return nullptr;
}

vector<vector<string>> parseInput(ifstream& in)
{
	vector<vector<string>> res;

	string line;
	while (!in.eof())
	{
		getline(in, line);
		vector<string> row;
		string curr;

		for (size_t i = 0; i < line.size(); i++)
		{
			if (line[i] != ' ')
			{
				curr += line[i];
			}

			if (line[i] == ' ' || i == line.size() - 1)
			{
				row.push_back(curr);
				curr.clear();
			}
		}
		res.push_back(row);
	}

	return res;
}

void clear(Node*& root)
{
	if (!root)
		return;

	for (Node* child : root->children)
	{
		clear(child);
		delete child;
		child = nullptr;
	}

	delete root;
	root = nullptr;
}

/**
 * @brief Removes the inner straight slashes ("|") from a row. (inner means without the starting | and ending |)
 * @param parentRow - the vector of strings whoose "|" will be erased
*/
void removeStraightSlashesInParent(vector<string>& parentRow)
{
	for (size_t i = 1; i < parentRow.size() - 1; i++)
	{
		if (parentRow[i] == "|")
			parentRow.erase(parentRow.begin() + i);
	}
}

/**
 * @brief Get the number of times an element repeats itself in the parent row
 * @param parentRow - vector of string representing the row of elements
 * @param col - index of parent in that parent row
 * @return the number of occurences of parentRow[col] in parentRow
*/
int parentPresentNumberOfTimes(vector<string>& parentRow, size_t col)
{
	int res = 0;
	for (size_t i = 0; i <= col; i++)
	{
		if (parentRow[i] == parentRow[col])
			res++;
	}
	return res;
}

Node* readTree(ifstream& in)
{
	vector<vector<string>> treeStrRepr = parseInput(in);
	Node* tree = new Node(stoi(treeStrRepr[0][1]));
	for (size_t i = 1; i < treeStrRepr.size(); i++)
	{
		size_t parIdx = 0;
		removeStraightSlashesInParent(treeStrRepr[i - 1]);
		for (size_t j = 0; j < treeStrRepr[i].size(); j++)
		{
			if (treeStrRepr[i][j] == "|") {
				parIdx++;
				continue;
			}

			int repeatedParentTimes = parentPresentNumberOfTimes(treeStrRepr[i - 1], parIdx);
			Node* parent = find(tree, stoi(treeStrRepr[i - 1][parIdx]), i - 1, repeatedParentTimes);
			parent->addChild(new Node(stoi(treeStrRepr[i][j])));
		}
	}

	return tree;
}

void writeTreeToFile(Node* root, ofstream& out)
{
	queue<Node*> front;

	front.push(root);
	out << "| " << root->key << " |";
	while (!front.empty())
	{
		Node* curr = front.front();
		front.pop();
		out << "| ";

		if (curr == nullptr)
		{
			out << " |";
		}
		else
		{
			for (Node* child : curr->children)
			{
				front.push(child);
				out << child->key;
			}

			front.push(nullptr); // signals that we have ended this brotherhood
		}
	}
}

bool areChildrenPermutation(const vector<Node*>& left, const vector<Node*>& right)
{
	if (left.size() < right.size())
		return false;

	for (size_t i = 0; i < right.size(); i++)
	{
		bool isContained = false;
		for (size_t j = 0; j < left.size(); j++)
		{
			if (left[j]->key == right[i]->key)
			{
				isContained = true;
			}
		}

		if (!isContained)
			return false;
	}

	return true;
}

bool checkIfContainedInLeft(Node* left, Node* right)
{
	if (!left)
		return false;

	if (left->key == right->key && areChildrenPermutation(left->children, right->children))
	{
		for (Node* childR : right->children)
		{
			if (!checkIfContainedInLeft(left, childR))
				return false;
		}

		return true;
	}

	for (Node* child : left->children)
	{
		if (checkIfContainedInLeft(child, right))
			return true;
	}

	return false;
}

int main(int argc, char** argv)
{
	string filePath = argv[1];
	string filePath2 = argv[2];
	string filePath3 = argv[3];
	ifstream in(filePath);
	if (!in.is_open())
		cout << "Couldn't open file for reading." << std::endl;

	ifstream in2(filePath2);
	if (!in.is_open())
		cout << "Couldn't open file for reading." << std::endl;

	Node* firstTree = readTree(in);
	Node* secondTree = readTree(in2);

	cout << checkIfContainedInLeft(firstTree, secondTree);

	ofstream out("treeout.txt");
	writeTreeToFile(secondTree, out);
	clear(firstTree);
	clear(secondTree);
	return 0;
}