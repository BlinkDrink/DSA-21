#pragma once
#include <fstream>
#include <string>
#include "StringHelper.hpp"
#include "termcolor.hpp" // Open-source libarary used for output colorization of character
#include "interface.hpp"
#include "CommandParser.hpp"

using std::fstream;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using sh = StringHelper;
using termcolor::red;
using termcolor::reset;
using termcolor::green;
using termcolor::yellow;

struct KeyValuePair
{
	string key;
	Hierarchy value;
};

class FileHandler
{
private:
	string fFilePath;
	fstream fFile;
	vector<KeyValuePair> fHierarchies;

	FileHandler() {}

	/// @brief Prints menu of commands on the console
	void menu() const
	{
		cout << "\t\t\tMENU" << endl;
		cout << "help	- shows this list of commands" << endl;
		cout << "load <name_of_hierarchy> <name_of_file> - loads an object with name <name_of_hierarchy> from file <name_of_file>" << endl;
		cout << "save <name_of_hierarchy> <name_of_file> - saves the current heirarchy in <name_of_file> file with <name_of_hierarchy> name. If no name of file is given - prints the result instead" << endl;
		cout << "find <name_of_hierarchy> <name_of_employee> - checks if <name_of_employee> exists in <name_of_hierarchy>" << endl;
		cout << "num_subordinates <name_of_hierarchy> <name_of_employee> - shows how many subordinates <name_of_employee> has in <name_of_hierarchy>" << endl;
		cout << "manager <name_of_hierarchy> <name_of_employee> - shows the manager of <name_of_employee> in <name_of_hierarchy>" << endl;
		cout << "num_employees <name_of_hierarchy> - shows how many employees work in <name_of_hierarchy>" << endl;
		cout << "overloaded <name_of_hierarchy> - shows how many employees have more than 20 employees working under them in <name_of_hierarchy>" << endl;
		cout << "join <name_of_hierarchy_1> <name_of_hierarchy_2> <name_of_hierarchy_result> - unites 2 hierarchies into one under the name of <name_of_hierarchy_result" << endl;
		cout << "fire <name_of_hierarchy> <name_of_employee> - fires the employee with name <name_of_employee> from <name_of_hierarchy>" << endl;
		cout << "hire <name_of_hierarchy> <name_of_employee> <name_of_boss> - hires <name_of_employee> under <name_of_boss> in <name_of_hierarchy>" << endl;
		cout << "salary <name_of_hierarchy> <name_of_employee> - shows the salary of <name_of_employee> in <name_of_hierarchy>" << endl;
		cout << "incorporate <name_of_hierarchy> - incorporates the branch; this operation is applied on <name_of_hierarchy>" << endl;
		cout << "modernize <name_of_hierarchy> - modernizes the branch; this operation is applied on <name_of_hierarchy>" << endl;
		cout << "exit - exits the program" << endl;
	}

public:
	static FileHandler& getInstance()
	{
		static FileHandler inst;
		return inst;
	}

	FileHandler(const FileHandler& other) = delete;
	FileHandler& operator=(const FileHandler& other) = delete;
	FileHandler(FileHandler&& other) = delete;
	FileHandler& operator=(FileHandler&& other) = delete;

	/// @brief Attempts to open file with given path then populates table from the file
	/// 
	/// @param path - path to file
	/// @returns true if the operation succeeded, false otherwise
	void loadFromFile(const string& path, const string& hierarchy_name)
	{
		string cpy(path), content, result;

		fFile.open(cpy, std::ios_base::in);

		if (!fFile.is_open())
		{
			throw invalid_argument("Couldn't open file for reading.");
		}

		while (getline(fFile, content))
		{
			result += content + '\n';
		}

		fHierarchies.push_back({ hierarchy_name, Hierarchy(result) });
		return true;
	}

	/**
	* 	@brief Attempts to close the opened file. If session hasn't been saved then it prompts the user for confirmation
	*	to either save/don't save/cancel. Clears in memory table.
	*
	*	@returns true if the operation succeeded, false if operation has been canceled
	*/
	//bool closeFile();

	/**
	* 	@brief Attempts to save the current table into the same file opened by openFile()
	*/
	//void saveToFile();

	/**
	*	@brief Saves the current session into file with given path
	*
	*	@param path - path to the new file
	*/
	//void saveFileAs(const string& path);

	/**
	 *	@brief Creates new document(in memory) which is empty. If wished to be saved it should be called with "saveas"
	 *
	 *	@returns true if the operation succeeded in creating new document, false if it didn't
	*/
	//bool createNewDocument();

	/**
	*	@brief Edits a cell at the given row and column with given content.
	*
	*	@param row	   - row of table
	*	@param col	   - column of table
	*	@param content - content with which it will be edited
	*/
	//void editFile(size_t row, size_t col, const string& content);

	// @brief Executes the main logic of the program
	void exe()
	{
		menu();
		CommandParser cp;

		while (true)
		{
			string cmd;
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
			case CommandType::HELP:
				menu();
				break;
			case CommandType::LOAD:
				try
				{
					if (cp.size() == 2)
					{
						string hierarchy_name = cp.atToken(1);
						string line, result;
						while (!getline(cin, line).eof())
							result += line + '\n';

						fHierarchies.push_back({ hierarchy_name, Hierarchy(result) });
					}
					else if (cp.size() == 3)
					{
						loadFromFile(cp.atToken(2), cp.atToken(1));
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

				cout << green << cp.atToken(1) << " loaded successfully!" << reset << endl;
				break;
				/*case CommandType::CLOSE:
					try
					{
						if (!closeFile())
							break;
					}
					catch (const exception& e)
					{
						cout << red << e.what() << reset << endl;
						break;
					}

					cout << green << "Successfully closed file." << reset << endl;
					break;
				case CommandType::NEW:
					try
					{
						if (!createNewDocument())
							break;
					}
					catch (const exception& e)
					{
						cout << red << e.what() << reset << endl;
						break;
					}

					cout << green << "Succesfully created new document" << reset << endl;
					break;
				case CommandType::SAVE:
					try
					{
						saveToFile();
					}
					catch (const exception& e)
					{
						cout << red << e.what() << reset << endl;
						break;
					}

					cout << green << "Successfully saved file into " << m_filePath << reset << endl;
					break;
				case CommandType::SAVEAS:
					try
					{
						saveFileAs(cp.atToken(1));
					}
					catch (const exception& e)
					{
						cout << red << e.what() << reset << endl;
						break;
					}

					cout << green << "Successfully saved table to " << m_filePath << reset << endl;
					break;
				case CommandType::EDIT:
					try
					{
						if (!sh::isStringValidCellAddress(cp.atToken(1)))
						{
							cout << red << "Invalid formula address" << reset << endl;
							break;
						}

						Pair pair = sh::extractCellAddressDetails(cp.atToken(1));
						editFile(pair.key, pair.value, cp.atToken(2));
					}
					catch (const invalid_argument& e)
					{
						cout << red << e.what() << reset << endl;
						break;
					}
					catch (const exception& e)
					{
						cout << red << e.what() << reset << endl;
						break;
					}

					cout << green << "Successfully set cell " << cp.atToken(1) << ", with content " << cp.atToken(2) << reset << endl;
					break;
				case CommandType::PRINT:
					try
					{
						m_table.print();
					}
					catch (const std::exception& e)
					{
						cout << red << e.what() << reset << endl;
						break;
					}

					break;
				case CommandType::EXIT:
					try
					{
						if (!m_isClosed)
						{
							if (!closeFile())
								break;
						}
					}
					catch (const exception& e)
					{
						cout << red << e.what() << reset << endl;
						break;
					}

					cout << termcolor::magenta << "Exiting the program..." << reset << endl;
					return;
				case CommandType::NOCOMMAND:
					cout << red << "Invalid command." << reset << endl;
					break;
				default:
					break;*/
			}
		}
	}
};