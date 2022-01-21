#pragma once
#include<iostream>
#include<fstream>
#include<unordered_map>
#include "Page.hpp"
#include "BPTree.hpp"

using std::fstream;
using std::string;

class Table {

private:
	/**
	 * Table instances control pages that contain the stored records on the hard disk.
	 * All actual read and write operations are done in this class.
	 */
	 //private static final long serialVersionUID = 1L;

	int maxTuplesPerPage, indexOrder, curPageIndex, numOfColumns;
	string path, tableName, tableHeader, primaryKey;
	std::unordered_map<string, string> colTypes, colRefs;
	std::unordered_map<string, size_t> colIndex;
	std::unordered_map<string, BPTree<TypeWrapper>> colNameIndex;

	/**
	 * Create a new table with the specified parameter list
	 * @param path
	 * @param strTableName the table name
	 * @param htblColNameType the types of table columns
	 * @param htblColNameRefs a mapping of foreign key references
	 * @param strKeyColName the primary key of the table
	 * @param maxTuplesPerPage the maximum number of records a page can hold
	 */
	Table(string path, string strTableName, std::unordered_map<string, string> htblColNameType,
		std::unordered_map<string, string> htblColNameRefs, string strKeyColName, int maxTuplesPerPage, int indexOrder)

	{

		this->path = path + strTableName + "/";
		this->tableName = strTableName;
		this->primaryKey = strKeyColName;
		this->colTypes = htblColNameType;
		this->colRefs = htblColNameRefs;
		this->maxTuplesPerPage = maxTuplesPerPage;
		this->curPageIndex = -1;
		this->numOfColumns = 0;
		this->indexOrder = indexOrder;

		initializeColumnsIndexes();
		createDirectory();
		createPage();
		this->createIndex(primaryKey);
		saveTable();
	}

	/**
	 * Save the table object in a binary file on the secondary storage
	 */
	void saveTable()
	{
		std::fstream f(path + tableName + ".bin", std::ios::out);
		if (!f.is_open())
			throw std::exception("Couldn't open file to save the table");

		f.write((char*)this, sizeof(*this));
		f.close();
	}

	/**
	 * Map every table column to an index to store records in arrays.
	 */
	void initializeColumnsIndexes() {
		tableHeader = "";
		int index = 0;
		for (std::pair<string, string> entry : colTypes)
		{
			colIndex.insert({ entry.first, index++ });
			numOfColumns++;
			tableHeader += entry.first + ", ";
		}
	}

	/**
	 * Create a directory for the table in which the table object
	 * and table pages are stored.
	 */
	void createDirectory()
	{
		system(("mkdir " + path).c_str());
	}

	/**
	 * Create a page to hold records for this table.
	 * @return the created page
	 * @throws IOException If an I/O error occurred
	 */
	Page createPage()
	{
		curPageIndex++;
		Page p(maxTuplesPerPage, path + tableName + "_" + std::to_string(curPageIndex) + ".bin");
		saveTable();
		return p;
	}

	/**
	 * Check wether an object matches its specified type
	 * @param value the object to be checked
	 * @param type the object type to be matched
	 * @return a boolean to indicate whether they match or not
	 */
	bool checkType(TypeWrapper value, string type) {

		if (type == "Integer")
			if (typeid(*value.getContent()) != typeid(IntegerObject))
				return false;
		if (type == "String")
			if (typeid(*value.getContent()) != typeid(StringObject))
				return false;

		return true;
	}

	/**
	 * Check that all specified columns are in the table schema and matches the defined types
	 * @param htblColNameValue some columns to be checked against the table schema
	 */
	void checkColumns(std::unordered_map<string, TypeWrapper> htblColNameValue)
	{
		for (std::pair<string, TypeWrapper> entry : htblColNameValue)
		{
			string colName = entry.first;
			if (colTypes.find(colName) == colTypes.end())
				throw std::invalid_argument("Column " + colName + " does not exist");

			if (!checkType(entry.second, colTypes.at(colName)))
				throw std::invalid_argument("Type on column " + colName + " isn't matching with the table's header");
		}
	}



	/**
	 * Get the column names in the order the are indexed
	 * @return the header of the table
	 */
	string getTableHeader()
	{
		return tableHeader;
	}
};