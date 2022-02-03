#include "DataBase.h"

DataBase::DataBase(ifstream& in)
{
	fh::readString(in, fDBName);
	fh::readString(in, fDBPath);

	size_t size = 0;
	in.read((char*)&size, sizeof(size));
	for (size_t i = 0; i < size; i++)
	{
		string first, second;
		fh::readString(in, first);
		fh::readString(in, second);

		ifstream tableReader(second + first + ".bin", std::ios::binary);
		if (!tableReader.is_open())
			throw invalid_argument("Couldn't open " + second + " path for reading.");

		fTables.insert({ first, Table(tableReader) });
	}
}

DataBase::DataBase(const string& name, const string& path)
{
	fDBName = name;
	fDBPath = path;
	createDirectory();
	save();
}

void DataBase::createTable(const string& path, const string& tableName, unordered_map<string, string>& colNameType, const string primaryKey, int maxRecordsPerPage)
{
	if (fTables.find(tableName) != fTables.end())
		throw invalid_argument("There is already a table with this name in the system");

	Table t(fDBPath, tableName, colNameType, primaryKey, maxRecordsPerPage);
	fTables[tableName] = t;
	save();
}

void DataBase::dropTable(const string& tableName)
{
	string pathToDelete = getTable(tableName).getTablePath();
	bool status = remove(pathToDelete.c_str());
	if (status != 0)
		throw logic_error("Something happened while trying to drop the table with name " + tableName);

	fTables.erase(tableName);
}

void DataBase::insert(const string& tableName, vector<unordered_map<string, TypeWrapper>> colNameValueList)
{
	for (size_t i = 0; i < colNameValueList.size(); i++)
		fTables[tableName].insert(colNameValueList[i]);

	save();
}

void DataBase::listTables() const
{
	for (const pair<string, Table>& entry : fTables)
	{
		std::cout << entry.first << "\n";
	}
}

Table& DataBase::getTable(const string& name)
{
	if (fTables.find(name) == fTables.end())
		throw invalid_argument("There is no such table!");

	return fTables[name];
}

void DataBase::save() const
{
	ofstream out(fDBPath + fDBName + ".bin", std::ios::binary);
	if (!out.is_open())
		throw exception("Couldn't open file to save Database");

	fh::writeString(out, fDBName);
	fh::writeString(out, fDBPath);

	size_t tablePathsSize = fTables.size();
	out.write((char*)&tablePathsSize, sizeof(tablePathsSize));
	for (const pair<string, Table>& entry : fTables)
	{
		fh::writeString(out, entry.first);
		fh::writeString(out, entry.second.getTablePath());
	}

	out.close();
}

void DataBase::createDirectory() const
{
	fs::create_directories(fDBPath);
}
