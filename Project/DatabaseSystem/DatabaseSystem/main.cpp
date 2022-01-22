#include<iostream>
#include "IntegerObject.hpp"
#include "StringObject.hpp"
#include "Page.hpp" 
#include "test.hpp" 


using std::cout;
using std::cin;

int main()
{
	//TypeWrapper i(68);
	//TypeWrapper s("HelloolleH meet to nice you");
	//Record rec(2);
	//rec.addValue(i);
	//rec.addValue(s);

	//Page p(1024, "page1.bin");

	//p.load();

	BPTree tree(4);
	tree.insert({ 50, {5,5} });
	tree.insert({ 55, {5,5} });
	tree.insert({ 60, {5,5} });
	tree.insert({ 70, {5,5} });
	tree.insert({ 43, {5,5} });
	tree.insert({ 45, {5,5} });
	tree.insert({ 40, {5,5} });
	tree.insert({ 30, {5,5} });
	tree.insert({ 20, {5,5} });

	Node* res = tree.search(15);

	return 0;
}