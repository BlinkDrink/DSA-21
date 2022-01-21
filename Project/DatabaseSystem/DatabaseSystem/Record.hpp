#pragma once
#include<vector>
#include "TypeWrapper.hpp"
#include "ObjectType.h"

class Record {

	/**
	 * @brief A record represents a row in a table. It consists of
	 * an array of values.
	 */
private:
	//private static final long serialVersionUID = 1L
	std::vector<TypeWrapper> fValues;
	size_t columns;
public:
	/**
	 * @brief Creates a new record
	 * @param size number of columns of the table holding the record
	 */
	Record(size_t size) : columns(size)
	{
		this->fValues.reserve(size);
	}

	//~Record()
	//{
	//	for (size_t i = 0; i < fValues.size(); i++)
	//		delete fValues[i];
	//}

	/**
	 * Update the value for a given column in the record
	 * @param index - the index of the column to be updated
	 * @param value - the new value
	 */
	void addValue(TypeWrapper& value)
	{
		if (fValues.size() + 1 > columns)
			throw std::out_of_range("Record addValue(value) - maximum properties for this record reached");

		fValues.push_back(value);
	}

	/**
	 * Get the value of a given column of this record
	 * @param index - index of the required column
	 * @return the value of that column
	 */
	TypeWrapper get(size_t index)
	{
		if (index >= fValues.size())
			throw std::out_of_range("Record get(index) - index is out of range");

		return fValues[index];
	}

	/**
	 *  @brief Write a record to file
	 *  @param out - output stream, used for writing
	 */
	void write(std::ostream& out) const
	{
		out.write((char*)&columns, sizeof(columns));
		for (size_t i = 0; i < fValues.size(); i++)
			fValues[i].getContent()->write(out);
	}

	/** @brief Read a record from file
	*   @param in - input stream, used for reading
	*/
	void read(std::istream& in)
	{
		size_t num_columns;
		in.read((char*)&num_columns, sizeof(num_columns));
		for (size_t i = 0; i < num_columns; i++) {
			ObjectType t;
			in.read((char*)&t, sizeof(t));

			if (t == ObjectType::INT) {
				TypeWrapper t(0);
				t.read(in);
				fValues.push_back(t);
			}
			else if (t == ObjectType::STRING) {
				TypeWrapper t("");
				t.read(in);
				fValues.push_back(t);
			}
		}
	}

	/**
	 *	@brief Getter
	 *	@return the number of columns of this record
	 */
	size_t size() const { return this->columns; }

	/**
	 * Display the record values
	 */
	std::string toString() const
	{
		std::string res;
		for (int i = 0; i < fValues.size(); i++)
			res += fValues[i].getContent()->toString() + "|";
		return res;
	}
};