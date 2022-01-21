#pragma once 
#include "Object.hpp"

/**
 * @brief Descriptor of string object
*/
class StringObject : public Object
{
public:
	StringObject() {}

	StringObject(std::string value) : fValue(value) {}

	virtual Object* clone() const final override
	{
		return new StringObject(fValue);
	}

	virtual size_t memsize() const final override
	{
		return fValue.size();
	}

	virtual std::string toString() const final override
	{
		return fValue;
	}

	virtual void write(std::ostream& out) const final override
	{
		size_t dataType;
		size_t size;

		ObjectType s = ObjectType::STRING;
		out.write((char*)&s, sizeof(s));
		size = fValue.size();
		out.write((char*)&size, sizeof(size));
		out.write((char*)fValue.c_str(), size);
	}

	virtual void read(std::istream& in) final override
	{
		size_t size = 0;
		char* str;

		in.read((char*)&size, sizeof(size));
		str = new char[size + 1];
		in.read(str, size);
		str[size] = '\0';
		fValue = str;
		delete[] str;
	}

private:
	std::string fValue;
};