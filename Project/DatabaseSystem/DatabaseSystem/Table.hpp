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

	///**
// * check if the given referencing values are valid for query operations
// * @param htblColNameValue some columns with there values
// * @throws FileNotFoundException If an error occurred in the stored table file
// * @throws DBEngineException If columns, foreign keys or the primary key are not valid
// * @throws IOException If an I/O error occurred
// * @throws ClassNotFoundException If an error occurred in the stored table pages format
// */
//void checkForeignKeyes(std::unordered_map<string, Object*> htblColNameValue)
//{
//	for (std::pair<string, Object*> entry : htblColNameValue) {
//		string referencedColumn = colRefs.at(entry.first);

//		if (referencedColumn != null)
//		{
//			String[] tokens = referencedColumn.split("\\.");
//			//				TableName.colName
//			String tableName = tokens[0];	//TableName
//			String colName = tokens[1];		//colName

//			ObjectInputStream ois = new ObjectInputStream(
//				new FileInputStream(new File(path + "../" + tableName + "/" + tableName + ".class")));
//			Table fetchedTable = (Table)ois.readObject();
//			ois.close();
//			if (!fetchedTable.checkRecordExists(colName, entry.getValue()))
//				throw new DBEngineException("Invalid value for the foreign key: " + entry.getKey() + " = " + entry.getValue());
//		}
//	}
//}

	/**
	 * create index on specified column name by creating BPTree on that column and inserting in it.
	 * @param strColName The name of the column which index is created on
	 */
	void createIndex(string strColName)
	{
		string type = colTypes.at(strColName);
		int colPos = colIndex.at(strColName);
		BPTree<TypeWrapper> tree(indexOrder);

		if (type == "Integer")
			colNameIndex.insert({ strColName, tree });
		if (type.equals("String"))
			tree = new BPTree<String>(indexOrder);
		if (type.equals("Date"))
			tree = new BPTree<Date>(indexOrder);
		if (type.equals("Double"))
			tree = new BPTree<Double>(indexOrder);

		colNameIndex.insert({ strColName, tree });

		ObjectInputStream ois;
		for (int index = 0; index <= curPageIndex; index++) {
			File f = new File(path + tableName + "_" + index + ".class");

			ois = new ObjectInputStream(new FileInputStream(f));
			Page p = (Page)ois.readObject();
			for (int i = 0; i < p.size(); ++i)
			{
				Record r = p.get(i);
				Ref recordReference = new Ref(index, i);
				tree.insert((Comparable)r.get(colPos), recordReference);
			}
			ois.close();
		}
		this.saveTable();
	}

	/**
	 * insert record in table with specified column values.
	 * @param htblColNameValue
	 * @return a boolean to indicate a successful or failed insertion operation
	 * @throws DBEngineException If columns, foreign keys or the primary key are not valid
	 * @throws IOException If an I/O error occurred
	 * @throws ClassNotFoundException If an error occurred in the stored table pages format
	 */
	bool insert(std::unordered_map<string, TypeWrapper> htblColNameValue)
	{

		//		1. check column names and types
		checkColumns(htblColNameValue);

		//		2. check for primary key
		TypeWrapper primaryValue = htblColNameValue.at(primaryKey);
		if (primaryValue.getContent() == nullptr)
			throw std::invalid_argument("Primary key is not allowed to be null");

		if (checkRecordExists(primaryKey, primaryValue))
			throw new DBEngineException("Primary key is already used before");


		//		3. check the foreign keys
		checkForeignKeyes(htblColNameValue);

		//		4. add the record
		Record r = new Record(numOfColumns);
		Stack<String> indexedCol = new Stack<String>();
		for (Entry<String, Object> entry : htblColNameValue.entrySet())
		{
			String colName = entry.getKey();
			if (colNameIndex.containsKey(colName)) // index on that col.
				indexedCol.push(colName);
			Object value = entry.getValue();
			r.addValue(colIndex.get(colName), value);
		}
		r.addValue(colIndex.get("TouchDate"), (Date)Calendar.getInstance().getTime());
		Page page = addRecord(r);
		while (!indexedCol.isEmpty())
			colNameIndex.get(indexedCol.peek()).insert((Comparable)htblColNameValue.get(indexedCol.pop()), new Ref(curPageIndex, page.size() - 1));
		//		System.out.println(colNameIndex.get(primaryKey));
		this.saveTable();
		return true;

	}


	/**
	 * Update the record that has the specified primary key with the given set of values
	 * @param strKey the primary key of the record to be updated
	 * @param htblColNameValue the set of columns associated with the new values to be updated
	 * @return a boolean indicating whether the update is successful or not
	 * @throws DBEngineException if the primary key to be updated, however, it's already used
	 * @throws ClassNotFoundException If an error occurred in the stored table pages format
	 * @throws IOException If an I/O error occurred
	 */
	bool update(Object strKey, std::unordered_map<string, Object*> htblColNameValue)
	{

		//		1. check column names and types
		checkColumns(htblColNameValue);

		//		2. check for primary key
		Object newPrimaryKey = htblColNameValue.get(primaryKey);
		if (newPrimaryKey != null)
		{
			//check that the new primary key does not exist
			if (checkRecordExists(primaryKey, newPrimaryKey))
				throw new DBEngineException("Primary key is already used before");
		}

		//		3. check the foreign keys
		checkForeignKeyes(htblColNameValue);
		BPTree tree = this.colNameIndex.get(primaryKey);
		Ref recordReference = tree.search((Comparable)strKey);
		if (recordReference == null)
			return false;
		int pageNo = recordReference.getPage();
		int indexInPage = recordReference.getIndexInPage();

		ObjectInputStream ois;
		File f = new File(path + tableName + "_" + pageNo + ".class");
		ois = new ObjectInputStream(new FileInputStream(f));
		Page p = (Page)ois.readObject();
		Record r = p.get(indexInPage);

		for (Entry <String, Integer> entry : this.colIndex.entrySet())
		{
			String colName = entry.getKey();
			int index = entry.getValue();
			if (this.colNameIndex.containsKey(colName) && htblColNameValue.containsKey(colName))
			{
				tree = this.colNameIndex.get(colName);
				tree.delete((Comparable)r.get(index));
			}
		}
		for (Entry<String, Object> entry : htblColNameValue.entrySet())
		{
			String colName = entry.getKey();
			Object colValue = entry.getValue();
			if (this.colNameIndex.containsKey(colName)) {
				tree = colNameIndex.get(colName);
				tree.insert((Comparable)colValue, recordReference);
			}
		}

		this.saveTable();
		ois.close();

		return true;
	}

	/**
	 * Add a new record to the table
	 * @param record the record to be added
	 */
	Page addRecord(Record record)
	{
		string pagePath = path + tableName + "_" + std::to_string(curPageIndex) + ".bin";
		std::ifstream in(pagePath);
		if (!in.is_open())
			throw std::invalid_argument("Couldnt open page at path " + path + " for reading.");

		Page curPage(0, pagePath);
		curPage.load();

		if (curPage.isFull())
			curPage = createPage();
		curPage.addRecord(record);

		in.close();
		return curPage;
	}

	/**
	 * Check if there exists a record, with the specified column value, in the table
	 * @param colName the column to be looked at while searching
	 * @param colValue the value to be matched
	 * @return whether the record exists or not
	 */
	bool checkRecordExists(string colName, TypeWrapper colValue)
	{
		std::unordered_map<string, TypeWrapper> htbl = std::unordered_map<string, TypeWrapper>();
		htbl[colName] = colValue;
		Iterator<Record> itr = select(htbl, "AND");
		if (itr.hasNext())
			return true;

		return false;
	}

	/**
	 * Check whether a given record matches any of the given column name-value pairs
	 * @param htblColNameValue - the column name-value pairs against which the record is checked
	 * @param record - the record to be checked
	 * @return a boolean to indicate whether this record matches any of the pairs or not
	 */
	bool checkOr(std::unordered_map<string, TypeWrapper> htblColNameValue, Record record)
	{
		for (std::pair<string, TypeWrapper> entry : htblColNameValue)
		{
			string colName = entry.first;
			TypeWrapper value = entry.second;

			int index = colIndex.at(colName);
			if (record.get(index).getContent() == value.getContent())
				return true;
		}
		return false;
	}

	/**
	 * Check whether a given record matches all the given column name-value pairs
	 * @param htblColNameValue the column name-value pairs against which the record is checked
	 * @param record the record to be checked
	 * @return a boolean to indicate whether this record matches all the pairs or not
	 */
	bool checkAND(std::unordered_map<string, TypeWrapper> htblColNameValue, Record record)
	{
		for (std::pair<string, TypeWrapper> entry : htblColNameValue)
		{
			string colName = entry.first;
			TypeWrapper value = entry.second;
			int index = colIndex.at(colName);
			if (record.get(index).getContent() != value.getContent())
				return false;
		}

		return true;
	}

	/**
	 * Select all records from the table that matches the specified column name-value pairs
	 * with a given conditional operator (AND or OR)
	 * @param htblColNameValue the column name-value pairs to which records will be compared
	 * @param strOperator the conditional operator to be executed ("AND, "OR" only)
	 * @return an iterator pointing to the first record in the result set
	 * @throws IOException If an I/O error occurred
	 * @throws ClassNotFoundException If an error occurred in the stored table pages format
	 */
	Iterator<Record> select(std::unordered_map<string, TypeWrapper> htblColNameValue, string strOperator)
	{
		bool isOr = strOperator == "OR";

		//try to select using an index if it exists
		Iterator<Record> itr = selectWithIndex(htblColNameValue, isOr);
		if (itr != null)
			return itr;


		ObjectInputStream ois;
		LinkedList<Record> answer = new LinkedList<Record>();

		for (int index = 0; index <= curPageIndex; index++) {
			File f = new File(path + tableName + "_" + index + ".class");

			ois = new ObjectInputStream(new FileInputStream(f));
			Page p = (Page)ois.readObject();
			for (int i = 0; i < p.size(); ++i)
			{
				Record r = p.get(i);

				if ((r != null && (isOr && checkOr(htblColNameValue, r) || !isOr && checkAND(htblColNameValue, r))))
					answer.add(r);
			}
			ois.close();
		}
		return answer.listIterator();
	}



	/**
	 * Select all records from the table that matches the specified column name-value pairs using index if exists .
	 * @param htblColNameValue the column name-value pairs to which records will be compared
	 * @param operator the conditional operator to be executed ("AND, "OR" only)
	 * @return an iterator pointing to the first record in the result set
	 */
	Record& selectWithIndex(std::unordered_map<string, TypeWrapper> htblColNameValue, bool op)
	{
		string indexColumn;
		TypeWrapper indexValue;
		for (std::pair<string, TypeWrapper> entry : htblColNameValue)
		{
			if (colNameIndex.find(entry.first) != colNameIndex.end())
			{
				indexColumn = entry.first;
				indexValue = entry.second;
				break;
			}
		}

		if (indexColumn.empty())
			throw std::invalid_argument("Such column wasn't found");

		LinkedList<Record> answer;
		BPTree tree = colNameIndex.get(indexColumn);
		Ref recordReference = tree.search((Comparable)indexValue);
		Record r = fetchRecordByReference(recordReference);
		if ((r != null && (operator && checkOr(htblColNameValue, r) || !operator && checkAND(htblColNameValue, r))))
			answer.add(r);
		return answer.listIterator();
	}
	/**
	 * @param recordReference reference to where the record is located in which page and in which index in that page
	 * @return record in the specified reference.
	 * @throws IOException If an I/O error occurred
	 * @throws ClassNotFoundException If an error occurred in the stored table pages format
	 * @throws FileNotFoundException If an error occurred in the stored table file
	 */
	Record fetchRecordByReference(Ref recordReference)
	{

		if (recordReference == null)
			return null;
		File f = new File(path + tableName + "_" + recordReference.getPage() + ".class");
		ObjectInputStream ois = new ObjectInputStream(new FileInputStream(f));
		Page p = (Page)ois.readObject();
		Record r = p.get(recordReference.getIndexInPage());
		ois.close();
		return r;
	}

	/** delete all records from the table that matches the specified column name-value pairs
	 * with a given conditional operator (AND or OR)
	 * @param htblColNameValue the column name-value pairs to which records will be compared
	 * @param strOperator the conditional operator to be exectuted ("AND, "OR" only)
	 * @throws IOException If an I/O error occurred
	 * @throws ClassNotFoundException If an error occurred in the stored table pages format
	 */

	int deleteRecord(std::unordered_map<string, Object> htblColNameValue, string strOperator)
	{
		boolean isOr = strOperator.equals("OR");
		int deletedRecords = 0;
		ObjectInputStream ois;
		for (int index = 0; index <= curPageIndex; index++) {
			File file = new File(path + tableName + "_" + index + ".class");
			ois = new ObjectInputStream(new FileInputStream(file));
			Page page = (Page)ois.readObject();
			for (int i = 0; i < page.size(); i++) {
				Record r = page.get(i);
				if (isOr && checkOr(htblColNameValue, r) || !isOr && checkAND(htblColNameValue, r)) {
					page.removeRecord(i);
					deleteRecord(r);
					deletedRecords++;
				}
			}

		}
		this.saveTable();
		return deletedRecords;
	}
	/**
	 * deleting specified record columns' values from corresponding BPTrees if exist.
	 * @param record
	 */
	void deleteRecord(Record record)
	{
		for (Entry<String, Integer> entry : this.colIndex.entrySet())
		{
			String colName = entry.getKey();
			int colIndex = entry.getValue();
			if (this.colNameIndex.containsKey(colName))
			{
				BPTree tree = this.colNameIndex.get(colName);
				tree.delete((Comparable)record.get(colIndex));
			}
		}
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