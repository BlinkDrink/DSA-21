#pragma once
#include "IntegerObject.hpp"
#include "StringObject.hpp"

class TypeWrapper
{
public:
	TypeWrapper() :fContent(nullptr) {}

	TypeWrapper(const std::string& content) :fContent(new StringObject(content)) {}

	TypeWrapper(int content) :fContent(new IntegerObject(content)) {}

	/**
	*	@brief Copy constructor
	*
	*	@param other - TypeWrapper object from which copying will be made
	*/
	TypeWrapper(const TypeWrapper& other)
	{
		copyFrom(other);
	}

	/**
	*	@brief Copy assignment operator
	*
	*	@param other - TypeWrapper object from which copying will be made
	*	@returns *this
	*/
	TypeWrapper& operator=(const TypeWrapper& other)
	{
		if (this != &other)
		{
			delete fContent;
			copyFrom(other);
		}
		return *this;
	}

	void write(std::ostream& out)
	{
		fContent->write(out);
	}

	void read(std::istream& in)
	{
		fContent->read(in);
	}

	/**
	*	@brief Move constructor
	*
	*	@param other - Cell object from which moving will be made
	*/
	TypeWrapper(TypeWrapper&& other) noexcept
	{
		moveFrom(other);
	}

	/**
	*	@brief Move assignment operator
	*
	*	@param other - Cell object from which moving will be made
	*/
	TypeWrapper& operator=(TypeWrapper&& other) noexcept
	{
		if (this != &other)
		{
			delete fContent;
			moveFrom(other);
		}

		return *this;
	}

	/**
	 * @brief Getter
	 *
	 * @return the content behind the pointer(IntegerType/DoubleType/FormulaType/StringType)
	*/
	Object* getContent() const
	{
		return fContent;
	}

	~TypeWrapper()
	{
		delete fContent;
	}


private:
	Object* fContent;

	void copyFrom(const TypeWrapper& other)
	{
		if (other.fContent != nullptr)
		{
			fContent = other.fContent->clone();
		}
		else
		{
			fContent = nullptr;
		}
	}

	void moveFrom(TypeWrapper& other) {
		fContent = other.fContent;
		other.fContent = nullptr;
	}
};