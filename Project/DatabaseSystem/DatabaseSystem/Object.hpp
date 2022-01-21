#pragma once
#include <string>
#include<fstream>

class Object
{
public:
	virtual Object* clone() const = 0;
	virtual size_t memsize() const = 0;
	virtual std::string toString() const = 0;
	virtual void write(std::ostream& out) const = 0;
	virtual void read(std::istream& in) = 0;
};