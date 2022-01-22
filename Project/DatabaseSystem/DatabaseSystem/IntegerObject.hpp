#pragma once
#include<memory>
#include "Object.hpp"
#include "ObjectType.h"

/**
 * @brief Descriptor of int object
*/
class IntegerObject : public Object
{
public:
	IntegerObject() :fValue(0) {}

	IntegerObject(int value) : fValue(value) {}

	virtual Object* clone() const final override
	{
		return new IntegerObject(fValue);
	}

	virtual size_t memsize() const final override
	{
		return sizeof(*this);
	}

	virtual std::string toString() const final override
	{
		return std::to_string(fValue);
	}

	virtual void write(std::ostream& out) const final override
	{
		ObjectType i = ObjectType::INT;
		out.write((char*)&i, sizeof(i));
		out.write((char*)&fValue, sizeof(fValue));
	}

	virtual void read(std::istream& in) final override
	{
		in.read((char*)&fValue, sizeof(fValue));
	}

private:
	int fValue;

	virtual bool isGreaterThan(const Object& other) const final override
	{
		return fValue > static_cast<const IntegerObject&>(other).fValue;
	}
};