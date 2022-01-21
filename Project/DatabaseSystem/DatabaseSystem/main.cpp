#include<iostream>
#include "IntegerObject.hpp"
#include "StringObject.hpp"
#include "Page.hpp" 

using std::cout;
using std::cin;

int main()
{
	TypeWrapper i(68);
	TypeWrapper s("HelloolleH meet to nice you");
	Record rec(2);
	rec.addValue(i);
	rec.addValue(s);

	Page p(1024, "page1.bin");

	p.load();

	return 0;
}