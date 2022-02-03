#pragma once
#include<iostream>
#include<stack>
#include<fstream>
#include<list>
#include<map>
#include<unordered_map>
#include <filesystem>
#include "Page.hpp"
#include "BPTree.hpp"
#include "FileHelper.hpp"
#include "Query.hpp"
#include "SortingHelper.h"

using std::multimap;
using std::map;
using std::unordered_map;
using std::ifstream;
using std::ofstream;
using std::string;
using std::to_string;
using std::pair;
using std::invalid_argument;
using std::exception;
using std::logic_error;
using std::list;

namespace fs = std::filesystem;
using fh = FileHelper;

class Table
{
public:
	Table() : curPageIndex(0), numOfColumns(0), bytes(0), maxRecordsPerPage(1024) {}

	/**
	 * Create a new table with the specified parameter list
	 * @param path
	 * @param strTableName the table name
	 * @param htblColNameType the types of table columns
	 * @param strKeyColName the primary key of the table
	 * @param maxTuplesPerPage the maximum number of records a page can hold
	 */
	Table(const string& path, const string& tableName, unordered_map<string, string>& colNameType,
		const string& indexedColName, int maxRecordsPerPage)
	{
		this->path = path + tableName + "/";
		this->tableName = tableName;
		this->primaryKey = indexedColName;
		this->colTypes = colNameType;
		this->maxRecordsPerPage = maxRecordsPerPage;
		this->curPageIndex = -1;
		this->numOfColumns = 0;
		this->bytes = 0;

		initializeColumnsIndexes();
		createDirectory();
		createPage();

		if (!primaryKey.empty())
			createIndex(primaryKey);

		saveTable();
	}

	/**
	 * @brief Reading constructor
	 * @param in
	*/
	Table(ifstream& in)
	{
		in.read((char*)&bytes, sizeof(bytes));
		in.read((char*)&maxRecordsPerPage, sizeof(maxRecordsPerPage));
		in.read((char*)&curPageIndex, sizeof(curPageIndex));

		fh::readString(in, path);
		fh::readString(in, tableName);
		fh::readString(in, primaryKey);

		size_t colTypesSize = 0;
		in.read((char*)&colTypesSize, sizeof(colTypesSize));
		for (size_t i = 0; i < colTypesSize; i++)
		{
			string first, second;
			fh::readString(in, first);
			fh::readString(in, second);
			colTypes.insert({ first, second });
		}

		indexedColumnRecords = BPTree(in);

		initializeColumnsIndexes();
	}

	/**
	 *	@brief Save table to binary file on the disk
	 */
	void saveTable()
	{
		ofstream out(path + tableName + ".bin", std::ios::binary);
		if (!out.is_open())
			throw exception("Couldn't open file to save the table");

		out.write((char*)&bytes, sizeof(bytes));
		out.write((char*)&maxRecordsPerPage, sizeof(maxRecordsPerPage));
		out.write((char*)&curPageIndex, sizeof(curPageIndex));

		fh::writeString(out, path);
		fh::writeString(out, tableName);
		fh::writeString(out, primaryKey);

		size_t colTypesSize = colTypes.size();
		out.write((char*)&colTypesSize, sizeof(colTypesSize));
		for (pair<string, string> entry : colTypes)
		{
			fh::writeString(out, entry.first);
			fh::writeString(out, entry.second);
		}

		indexedColumnRecords.write(out);

		out.close();
	}

	/**
	 *	@brief Map every table column to an index
	 */
	void initializeColumnsIndexes() {
		int index = 0;
		for (pair<string, string> entry : colTypes)
		{
			colIndex.insert({ entry.first, index++ });
			tableHeader += entry.first + ", ";
		}
		numOfColumns = colTypes.size();
	}

	/**
	 *	@brief Create a directory for the table in which the table's properties
	 *	and table's pages are stored.
	 */
	void createDirectory()
	{
		fs::create_directories(path);
	}

	/**
	 * @brief Creates the indexed column, if there is one
	 * @param strColName
	*/
	void createIndex(const string& strColName)
	{
		if (colTypes.find(strColName) == colTypes.end())
			throw invalid_argument("Cannot put Index on non existing column");

		string type = colTypes.at(strColName);
		int colPos = colIndex.at(strColName);

		for (int index = 0; index <= curPageIndex; index++) {
			ifstream in(path + tableName + "_" + to_string(index) + ".bin", std::ios::binary);

			Page p(in);
			for (int i = 0; i < p.size(); ++i)
			{
				Record r = p.get(i);
				RecordPtr recordReference(index, i);
				indexedColumnRecords.insert({ r.get(colPos), recordReference });
			}

			in.close();
		}

		saveTable();
	}

	/**
	 *	@brief Create a page to hold records for this table.
	 *	@return the created page
	 */
	Page createPage()
	{
		curPageIndex++;
		Page p(maxRecordsPerPage, path + tableName + "_" + std::to_string(curPageIndex) + ".bin");
		saveTable();
		return p;
	}

	/**
	 *	@brief Check wether an object matches its specified type (column type)
	 *	@param value - the object to be checked
	 *	@param type - the object type to be matched
	 *	@return bool to indicate whether they match or not
	 */
	bool checkType(const TypeWrapper& value, const string& type) {

		if (type == "Integer")
			if (typeid(*value.getContent()) != typeid(IntegerObject))
				return false;
		if (type == "String")
			if (typeid(*value.getContent()) != typeid(StringObject))
				return false;
		if (type == "Double")
			if (typeid(*value.getContent()) != typeid(DoubleObject))
				return false;

		return true;
	}

	/**
	 * @brief Insert record in table with specified column values.
	 * @param htblColNameValue
	 */
	void insert(unordered_map<string, TypeWrapper>& colNameValue)
	{
		checkColumns(colNameValue);

		if (!primaryKey.empty()) {
			TypeWrapper primaryValue = colNameValue.at(primaryKey);
			if (primaryValue.getContent() == nullptr)
				throw invalid_argument("Primary key is not allowed to be empty");

			if (checkRecordExists(primaryKey, primaryValue))
				throw invalid_argument("Primary key " + primaryKey + " is already used before");
		}

		Record r(numOfColumns);
		for (const pair<string, TypeWrapper>& entry : colNameValue)
			r.addValue(entry.second);

		Page page = addRecord(r);
		indexedColumnRecords.insert({ colNameValue.at(primaryKey), RecordPtr(curPageIndex, page.size() - 1) });
		saveTable();
	}

	/**
	 *	@brief Add a new record to the table
	 *	@param record - the record to be added
	 */
	Page addRecord(Record& record)
	{
		string pagePath = path + tableName + "_" + std::to_string(curPageIndex) + ".bin";
		ifstream in(pagePath, std::ios::binary);
		if (!in.is_open())
			throw std::invalid_argument("Couldnt open page at path " + pagePath + " for reading.");

		Page p(in);
		if (p.isFull())
			p = createPage();

		p.addRecord(record);
		bytes += record.getKiloBytesData();

		in.close();
		return p;
	}

	/**
	 *	@brief Check if there is a record with the specified column value in the table
	 *	@param colName the column to be looked for when searching
	 *	@param colValue the value to be matched
	 *	@return whether the record exists or not
	 */
	bool checkRecordExists(const string& colName, const TypeWrapper& colValue)
	{
		Query exp(colName + " = " + colValue.toString(), colTypes, primaryKey);
		vector<Record> answer = select(exp);
		if (!answer.empty())
			return true;

		return false;
	}

	/**
	 * @brief In a vector of records, get the discinct ones and return them
	 * @param target - vector which will be distinct-ized
	 * @param selectedColumns - the columns that the user desries to be outputted
	 * @return Vector of distinct-ized records
	*/
	vector<Record> distinct(vector<Record>& target, vector<string>& selectedColumns)
	{
		for (const string& col : selectedColumns)
			if (colIndex.find(col) == colIndex.end())
				throw invalid_argument("Cannot call distinct on non existing column.");

		vector<Record> res;
		for (size_t i = 0; i < target.size(); i++)
		{
			bool areEqual = true;
			for (size_t j = 0; j < i; j++)
			{
				if (i == j) continue;

				for (size_t k = 0; k < selectedColumns.size(); k++)
				{
					if (target[i].get(colIndex[selectedColumns[k]]) != target[j].get(colIndex[selectedColumns[k]]))
					{
						areEqual = false;
						break;
					}
				}
			}

			if (!areEqual)
				res.push_back(target[i]);
		}

		return res;
	}

	/**
	 * @brief Given a vector of records, order them by the given column
	 * @param target - vector to be sorted
	 * @param orderByWhat - column by which the sorting will be made
	*/
	void orderBy(vector<Record>& target, const string& orderByWhat)
	{
		if (colIndex.find(orderByWhat) == colIndex.end())
			throw invalid_argument("Cannot select a column that is not part of the scheme. (" + orderByWhat + ")");

		heapSort(target, colIndex[orderByWhat]);
	}

	/**
	 * @brief Selects records satisfying the WHERE criteria
	 * @param colValueOperator - "ID > 5" col refers to "ID", Value refers to "5", operator refers to ">"
	 * @return vector with filtered records
	*/
	vector<Record> select(Query& query)
	{
		stack<vector<Record>> result;
		queue<string> output = query.getShuntingOutput();

		while (!output.empty())
		{
			if (sh::isStringInteger(output.front()))
			{
				InternalQuery curr = query.getNumberedQueries().at(output.front());
				if (curr.isPrimaryKeyQuery())
				{
					vector<RecordPtr> fromTree = indexedColumnRecords.getRecordsFromQuery(curr);
					if (!fromTree.empty())
						result.push(fetchRecordsByReference(fromTree));
					else
						result.push(vector<Record>());
				}
				else
				{
					vector<Record> answer;
					for (size_t index = 0; index <= curPageIndex; index++) {
						ifstream in(path + tableName + "_" + to_string(index) + ".bin", std::ios::binary);

						Page p(in);
						for (size_t i = 0; i < p.size(); ++i)
						{
							Record r = p.get(i);
							if (curr.checkRecordAgainstCondition(colIndex, r))
								answer.push_back(r);
						}
						in.close();
					}
					result.push(answer);
				}

				output.pop();
			}
			else
			{
				vector<Record> val2 = result.top();
				result.pop();

				vector<Record> val1 = result.top();
				result.pop();

				string op = output.front();
				output.pop();

				// Intersection (AND)
				if (op == "AND")
				{
					vector<Record> vec_intersection;
					if (val2.size() > val1.size())
						std::swap(val1, val2);

					for (size_t i = 0; i < val2.size(); i++)
						if (std::find(val1.begin(), val1.end(), val2[i]) != val1.end())
							vec_intersection.push_back(val2[i]);

					result.push(vec_intersection);
				}
				// Union (OR)
				else
				{
					map<Record, int> mp;
					vector<Record> vec_union;

					// Inserting array elements in mp
					for (int i = 0; i < val2.size(); i++)
						mp.insert({ val2[i], i });

					for (int i = 0; i < val1.size(); i++)
						mp.insert({ val1[i], i });

					for (const pair<Record, int>& entry : mp)
						vec_union.push_back(entry.first);

					result.push(vec_union);
				}
			}
		}

		return result.top();
	}

	/**
	 * @brief Acts just like select with given query, but can also pass arguments wheter to sort it by
	 * given column or/and to get only the distinct elements
	 * @param query - WHERE clause
	 * @param orderByWhat - by which column shall the sorting be done
	 * @param isDistinct - if True then the answer shall not contain any duplicates of the selected columns
	 * @param selectedCols - columns that the user is selecting
	 * @return array of selected records
	*/
	vector<Record> select(Query& query, const string& orderByWhat, bool isDistinct, vector<string>& selectedCols)
	{
		vector<Record> answer = select(query);

		if (isDistinct)
			answer = distinct(answer, selectedCols);
		if (!orderByWhat.empty())
			orderBy(answer, orderByWhat);

		return answer;
	}

	/**
	 * @param recordReference - a tuple holding info about the index of the page that contains the record, and the record's id in the page
	 * @return record in the specified reference.
	 */
	Record fetchRecordByReference(RecordPtr& recordReference)
	{
		string readPath = path + tableName + "_" + to_string(recordReference.getPage()) + ".bin";
		ifstream in(readPath, std::ios::binary);
		Page p(in);
		Record r = p.get(recordReference.getIndexInPage());
		in.close();
		return r;
	}

	/**
	 * @brief Given vector of pointers to records, fetch them all
	 * @param recordsReferences - vector of record pointers
	 * @return the records pointed to by record pointers
	*/
	vector<Record> fetchRecordsByReference(vector<RecordPtr>& recordsReferences)
	{
		vector<Record> res;

		// Sort refs by page in ascending order, then by index in page in ascending order
		for (size_t i = 0; i < recordsReferences.size() - 1; i++)
		{
			size_t min = i;
			for (size_t j = i + 1; j < recordsReferences.size(); j++)
				if (recordsReferences[j] < recordsReferences[min])
					min = j;

			std::swap(recordsReferences[min], recordsReferences[i]);
		}

		// Whisk through the pages, first opening the one with index 0, then 1, then 2...
		// and extracting the needed records. Prioritizes extracting the records from the current page, then the next, without
		// having to reopen on every iteration for every record
		for (size_t i = 0; i < recordsReferences.size(); i++)
		{
			string readPath = path + tableName + "_" + to_string(recordsReferences[i].getPage()) + ".bin";
			ifstream in(readPath, std::ios::binary);
			Page p(in);

			if (i < recordsReferences.size() - 1 && recordsReferences[i].getPage() != recordsReferences[i + 1].getPage())
			{
				res.push_back(p.get(recordsReferences[i].getIndexInPage()));
			}
			else
			{
				while (i < recordsReferences.size() - 1 && recordsReferences[i].getPage() == recordsReferences[i + 1].getPage())
				{
					res.push_back(p.get(recordsReferences[i].getIndexInPage()));
					i++;
				}
			}

			if (i == recordsReferences.size() - 1)
				res.push_back(p.get(recordsReferences[i].getIndexInPage()));

			in.close();
		}

		return res;
	}

	/** @brief Delete the records that satisfy the WHERE clause
	 *	@param strOperator the conditional operator to be exectuted ("AND, "OR" only)
	 */
	int deleteRecord(Query& query)
	{
		int deletedRecords = 0;
		for (size_t index = 0; index <= curPageIndex; index++)
		{
			ifstream in(path + tableName + "_" + to_string(index) + ".bin", std::ios::binary);
			Page page(in);
			for (size_t i = 0; i < page.size(); i++)
			{
				Record r = page.get(i);
				if (query.checkRecordAgainstQuery(r, colIndex))
				{
					bytes -= r.getKiloBytesData();
					page.removeRecord(i);
					deleteRecord(r);
					deletedRecords++;
				}
			}
		}

		saveTable();
		return deletedRecords;
	}

	/**
	 *	@brief deleting record column value from map if the value exists.
	 *	@param record - record that will be deleted
	 */
	void deleteRecord(Record& record)
	{
		if (!primaryKey.empty())
		{
			int col = colIndex[primaryKey];
			indexedColumnRecords.remove(record.get(col));
		}
	}

	/**
	 *	@brief Check that all specified columns are in the table schema and matches the defined types
	 *	@param htblColNameValue some columns to be checked against the table schema
	 */
	void checkColumns(std::unordered_map<string, TypeWrapper> htblColNameValue)
	{
		for (const pair<string, TypeWrapper>& entry : htblColNameValue)
		{
			string colName = entry.first;
			if (colTypes.find(colName) == colTypes.end())
				throw std::invalid_argument("Column " + colName + " does not exist");

			if (!checkType(entry.second, colTypes.at(colName)))
				throw std::invalid_argument("Type on column " + colName + " isn't matching with the table's header");
		}
	}

	const string& getTableHeader() const { return tableHeader; }

	const string& getTablePath() const { return path; }

	long getKiloBytesData() const { return bytes / 1024; }

	const unordered_map<string, string>& getTableScheme() const { return colTypes; }

	const unordered_map<string, size_t>& getColIndex() const { return colIndex; }

	const string& getPrimaryKey() const { return primaryKey; }

	size_t getColumnsCount() const { return numOfColumns; }

private:
	/**
	 *	@brief Table instance controls pages that contain the stored records on the hard disk.
	 *	All read and write operations are done in this class.
	 */
	long bytes;
	int maxRecordsPerPage, curPageIndex, numOfColumns;
	string path, tableName, tableHeader, primaryKey;
	unordered_map<string, string> colTypes;
	unordered_map<string, size_t> colIndex;
	BPTree indexedColumnRecords;
};