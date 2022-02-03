#include "Engine.h"
#include <algorithm>

using std::max;

void Engine::menu()
{
	cout << yellow << "\t\t\t\t\t\t\tMENU" << endl;
	cout << "CreateTable {tableName} (ColumnName1:DataType1, ColumnName2:DataType2..) Index ON {columnName}" << endl;
	cout << "DropTable {tableName}" << endl;
	cout << "ListTables" << endl;
	cout << "TableInfo {tableName}" << endl;
	cout << "Select {columnNames} FROM {tableName} WHERE {condition1} {OR|AND} {condition2} OrderBy {columnName} DISTINCT" << endl;
	cout << "Remove FROM {tableName} WHERE {condition1} {OR|AND} {condition2} .." << endl;
	cout << "Insert INTO {tableName} {(value1, value2...)}" << reset << endl;
}

unordered_map<string, string> Engine::getColNameType(string scheme)
{
	unordered_map<string, string> colNameType;
	scheme.erase(scheme.begin());
	scheme.pop_back();

	vector<string> parts = sh::splitBy(scheme, ",");
	for (size_t i = 0; i < parts.size(); i++)
	{
		vector<string> tuple = sh::splitBy(parts[i], ":");
		colNameType.insert({ tuple[0], tuple[1] });
	}
	return colNameType;
}

vector<unordered_map<string, TypeWrapper>> Engine::getColNameValues(string values, unordered_map<string, string>& scheme, unordered_map<size_t, string>& indexColumn)
{
	vector<unordered_map<string, TypeWrapper>> result;
	values.erase(values.begin());
	values.pop_back();

	vector<string> parts = sh::splitBy(values, ", ");
	for (size_t i = 0; i < parts.size(); i++)
	{
		sh::trim(parts[i]);
		parts[i].erase(parts[i].begin());
		parts[i].pop_back();

		vector<string> splitRecord = sh::splitBy(parts[i], ",");
		if (splitRecord.size() != scheme.size())
			throw invalid_argument("Passed number of columns and table's number of columns are mismatching. Please check the record you are about to insert.");

		unordered_map<string, TypeWrapper> colVal;
		for (size_t j = 0; j < splitRecord.size(); j++)
		{
			string colName = indexColumn[j];
			string colType = scheme[colName];

			if (sh::isCorrectColumnType(colType, splitRecord[j]))
			{
				if (colType == "Integer")
					colVal.insert({ colName, TypeWrapper(stoi(splitRecord[j])) });
				else if (colType == "Double")
					colVal.insert({ colName, TypeWrapper(stod(splitRecord[j])) });
				else if (colType == "String")
					colVal.insert({ colName, TypeWrapper(splitRecord[j]) });
			}
			else
			{
				throw invalid_argument("Invalid type for column {" + colName + "} with value {" + splitRecord[j] + "}");
			}
		}
		result.push_back(colVal);
	}
	return result;
}

Engine& Engine::getInstance()
{
	static Engine inst;
	return inst;
}

void Engine::printSelectedRecords(vector<Record>& records, vector<string>& selectedColumns, unordered_map<string, size_t> colIndex) const
{
	unordered_map<string, size_t> longestWordsPerCol = getLongestWordPerCol(records, selectedColumns, colIndex);
	printHeader(selectedColumns, longestWordsPerCol);

	for (size_t i = 0; i < records.size(); i++)
	{
		cout << " | ";
		for (size_t j = 0; j < selectedColumns.size(); j++)
		{
			TypeWrapper content = records[i].get(colIndex[selectedColumns[j]]);
			printCellInformation(content, longestWordsPerCol[selectedColumns[j]], selectedColumns[j].size());
			cout << " | ";
		}

		cout << endl;
	}

	cout << "Total " << records.size() << " records selected." << endl;
}

void Engine::printCellInformation(TypeWrapper& cell, size_t longestWordOfCol, size_t colSize) const
{
	if (cell.getContent() != nullptr)
	{
		string spaces(colSize > longestWordOfCol ? colSize - cell.getContent()->size() : longestWordOfCol - cell.getContent()->size(), ' ');
		cout << cell.getContent()->toString();
		cout << spaces;
	}
}

void Engine::printHeader(vector<string>& selectedColumns, unordered_map<string, size_t>& longestWordsPerCol) const
{
	for (size_t j = 0; j < selectedColumns.size(); j++)
	{
		string spaces;
		if (longestWordsPerCol[selectedColumns[j]] != 0)
		{
			size_t numSpaces = longestWordsPerCol[selectedColumns[j]] > selectedColumns[j].size() ? longestWordsPerCol[selectedColumns[j]] - selectedColumns[j].size() : 0;
			spaces = string(numSpaces, ' ');
		}
		//else
		//	spaces = string(selectedColumns[j].size(), ' ');

		cout << " | " << selectedColumns[j] << spaces;
	}

	cout << " |" << endl;

	cout << " --";
	for (size_t i = 0; i < selectedColumns.size(); i++) {
		for (size_t j = 0; j < longestWordsPerCol[selectedColumns[i]]; j++)
			cout << "-";

		cout << "---";
	}

	cout << "--" << endl;
}

size_t Engine::getLongestContentAtCol(size_t col, vector<Record>& records) const
{
	size_t longest = 0;

	for (size_t i = 0; i < records.size(); i++)
	{
		TypeWrapper content = records[i].get(col);
		if (content.getContent() != nullptr)
		{
			size_t len = content.getContent()->size();
			if (longest < len)
				longest = len;
		}
	}

	return longest;
}

unordered_map<string, size_t> Engine::getLongestWordPerCol(vector<Record>& records, vector<string>& selectedColumns, unordered_map<string, size_t> colIndex) const
{
	unordered_map<string, size_t> lengthPerCol;

	for (size_t i = 0; i < selectedColumns.size(); i++)
		lengthPerCol.insert({ selectedColumns[i] ,getLongestContentAtCol(colIndex[selectedColumns[i]],records) });

	return lengthPerCol;
}

void Engine::run()
{
	menu();
	CommandParser cp;
	string dbName = "FMISql";
	string dbPath = "DB/";

	ifstream in(dbPath + dbName + ".bin", std::ios::binary);
	if (!in.is_open())
	{
		cout << yellow << "Couldn't open DB for reading... Creating new file" << reset << endl;
		DataBase db(dbName, dbPath);
	}
	in.close();

	ifstream inDb(dbPath + dbName + ".bin", std::ios::binary);
	DataBase db(inDb);
	inDb.close();

	while (true)
	{
		string cmd;
		cout << dbName << '>';
		getline(cin, cmd);

		cp.clearCmd();
		try
		{
			cp.setData(cmd);
		}
		catch (const invalid_argument& e)
		{
			cout << red << e.what() << reset << endl;
			continue;
		}

		switch (cp.getCommandType())
		{
		case CommandType::CREATE_TABLE:
			try
			{
				string tblName = cp.atToken(1);
				unordered_map<string, string> scheme = getColNameType(cp.atToken(2));
				string primaryKey = cp.size() >= 6 ? cp.atToken(5) : "";
				db.createTable(dbPath, tblName, scheme, primaryKey);
			}
			catch (const invalid_argument& e)
			{
				cout << red << e.what() << reset << endl;
				break;
			}
			catch (const out_of_range& e)
			{
				cout << red << e.what() << reset << endl;
				break;
			}

			cout << green << "Table " << cp.atToken(1) << " created!" << reset << endl;
			break;
		case CommandType::DROP_TABLE:
			try
			{
				string tblName = cp.atToken(1);
				db.dropTable(tblName);
			}
			catch (const invalid_argument& e)
			{
				cout << red << e.what() << reset << endl;
				break;
			}
			catch (const out_of_range& e)
			{
				cout << red << e.what() << reset << endl;
				break;
			}

			cout << green << "Table " << cp.atToken(1) << " created!" << reset << endl;
			break;
		case CommandType::LIST_TABLES:
			cout << yellow << "There " << (db.getNumTables() == 1 ? "is " : "are ") << db.getNumTables() << "table in the database:" << reset << endl;
			db.listTables();
			break;
		case CommandType::TABLE_INFO:
			try
			{
				Table& t = db.getTable(cp.atToken(1));
				string scheme = "(";
				for (const pair<string, string>& entry : t.getTableScheme())
					scheme += entry.first + ":" + entry.second + ", ";
				scheme += ")";

				cout << yellow << "Table " << cp.atToken(1) << " : " << scheme << reset << endl;
			}
			catch (const out_of_range& e)
			{
				cout << red << e.what() << reset << endl;
				break;
			}
			catch (const invalid_argument& e)
			{
				cout << red << e.what() << reset << endl;
			}
			break;
		case CommandType::INSERT:
			try
			{
				size_t numRecords = 0;
				string tblName = cp.atToken(2);
				if (cp.size() > 4)
					throw invalid_argument("Too many arguments for this command.");

				size_t ind = 0;
				unordered_map<size_t, string> indexColumn;
				unordered_map<string, string> tableScheme = db.getTable(tblName).getTableScheme();
				for (pair<string, string> entry : tableScheme)
					indexColumn.insert({ ind++, entry.first });

				vector<unordered_map<string, TypeWrapper>> values = getColNameValues(cp.atToken(3), tableScheme, indexColumn);
				db.insert(tblName, values);
				numRecords = values.size();
				cout << green << "Total " << numRecords << " inserted." << reset << endl;
			}
			catch (const invalid_argument& e)
			{
				cout << red << e.what() << reset << endl;
				break;
			}
			catch (const out_of_range& e)
			{
				cout << red << e.what() << reset << endl;
				break;
			}

			break;
		case CommandType::SELECT:
			try
			{
				vector<string> selectedColumns = sh::splitBy(cp.atToken(1), ",");
				string tblName = cp.atToken(3);
				Table& target = db.getTable(tblName);
				bool isDistinct = cp.isDistinct();
				string orderBy = cp.getOrderBy();

				if (cp.size() <= 4)
				{
					Query q("", target.getTableScheme(), target.getPrimaryKey());
					if (selectedColumns.size() == 1 && selectedColumns[0] == "*")
					{
						selectedColumns = sh::splitBy(db.getTable(tblName).getTableHeader(), ", ");
						sh::removeEmptyStringsInVector(selectedColumns);
						vector<Record> answer = target.select(q, orderBy, isDistinct, selectedColumns);
						printSelectedRecords(answer, selectedColumns, target.getColIndex());
					}
					else
					{
						vector<Record> answer = target.select(q, orderBy, isDistinct, selectedColumns);
						printSelectedRecords(answer, selectedColumns, target.getColIndex());
					}
				}
				else
				{
					Query query(cp.atToken(4), target.getTableScheme(), target.getPrimaryKey());
					if (selectedColumns.size() == 1 && selectedColumns[0] == "*")
					{
						selectedColumns = sh::splitBy(db.getTable(tblName).getTableHeader(), ", ");
						sh::removeEmptyStringsInVector(selectedColumns);
						vector<Record> answer = target.select(query, orderBy, isDistinct, selectedColumns);
						printSelectedRecords(answer, selectedColumns, target.getColIndex());
					}
					else
					{
						vector<Record> answer = target.select(query, orderBy, isDistinct, selectedColumns);
						printSelectedRecords(answer, selectedColumns, target.getColIndex());
					}
				}

			}
			catch (const invalid_argument& e)
			{
				cout << red << e.what() << reset << endl;
				break;
			}
			catch (const out_of_range& e)
			{
				cout << red << e.what() << reset << endl;
				break;
			}

			break;
		case CommandType::EXIT:
			cout << green << "Goodbye" << reset << endl;
			return;
		}
	}
}
