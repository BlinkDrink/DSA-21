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

	bool operator>(const Object& other) const
	{
		// If the derived types are the same then compare them
		return typeid(*this) == typeid(other) && isGreaterThan(other);
	}

private:
	virtual bool isGreaterThan(const Object& other) const = 0;
};