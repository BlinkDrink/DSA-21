#pragma once
#include<iostream>
#include "DataType.hpp"

using std::cout;

class DoubleType : public DataType
{
public:
	DoubleType(const string& name, double value) : DataType(name), fValue(value) {}

	virtual void print() const final override { cout << this->typeName << " " << fValue; }

	virtual DataType* clone() const final override { return new DoubleType("DOUBLE", 0); }

private:
	double fValue;
};
