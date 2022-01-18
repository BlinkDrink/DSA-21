#pragma once
#include<string>

using std::string;

class DataType
{
public:
	DataType(const string& name) : typeName(name) {}
	virtual void print() const = 0;
	virtual DataType* clone() const = 0;
protected:
	const string typeName;
};