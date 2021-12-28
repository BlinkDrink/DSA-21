#pragma once
#include <fstream>
#include <string>
#include<climits>
#include "StringHelper.hpp"
#include "termcolor.hpp" // Open-source libarary used for output colorization of character
#include "interface.hpp"
#include "CommandParser.hpp"
#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

using std::fstream;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using sh = StringHelper;
using std::exception;
using termcolor::red;
using termcolor::reset;
using termcolor::green;
using termcolor::yellow;

class FileHandler
{
private:
	string fFilePath;
	fstream fFile;
	vector<Hierarchy*> fHierarchies;

	FileHandler() {}

	/// @brief Prints menu of commands on the console
	void menu() const
	{
		cout << "\t\t\tMENU" << endl;
		cout << "help - shows this list of commands" << endl;
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

	/// @brief Loads a hiearachy. Input can be done manually or from file, depending
	/// on the number of arguments the user has given.
	/// 
	/// @param commands - vector of commands the user has given
	void load(const vector<string>& commands)
	{
		if (commands.size() < 2 || commands.size() > 3)
			throw invalid_argument("Invalid number of arguments for this command.");

		if (commands.size() == 2)
		{
			string line, result;
			while (!getline(cin, line).eof())
				result += line + '\n';
			cin.clear();

			Hierarchy* res = DBG_NEW Hierarchy(result);
			res->setName(commands.at(1));
			fHierarchies.push_back(res);
		}
		else if (commands.size() == 3)
		{
			fFile.open(commands.at(2), std::ios_base::in);
			fFilePath = commands.at(2);

			if (!fFile.is_open())
				throw invalid_argument("Couldn't open file for reading.");

			string content, result;
			while (getline(fFile, content))
				result += content + '\n';

			Hierarchy* res = DBG_NEW Hierarchy(result);
			res->setName(commands.at(1));
			fHierarchies.push_back(res);

			fFile.close();
		}
	}

	/// @brief Saves a heirarchy. Given 3 arguments - save <name_hierarchy> <name_file> saves the given hiearchy in desired file.
	/// If the user hasn't given <name_file> then the information about the heirarchy is printed on the coonsole instead.
	/// 
	/// @param commands - vector of arguments the user has given
	void save(const vector<string>& commands)
	{
		if (commands.size() < 2 || commands.size() > 3)
			throw invalid_argument("Invalid number of arguments for this command.");

		if (commands.size() == 2)
		{
			cout << getHierarchy(commands.at(1))->print() << endl;
		}
		else if (commands.size() == 3)
		{
			fFile.open(commands.at(2), std::ios_base::out);
			fFilePath = commands.at(2);

			if (!fFile.is_open())
				throw invalid_argument("Couldn't open file for writing.");

			fFile << getHierarchy(commands.at(1))->print();
			fFile.close();
		}
	}

	Hierarchy* getHierarchy(const string& name)
	{
		for (size_t i = 0; i < fHierarchies.size(); i++)
			if (fHierarchies[i]->getName() == name)
				return fHierarchies[i];

		throw invalid_argument("Hierarchy with such name was not found.");
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
					load(cp.getCommands());
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
			case CommandType::SAVE:
				try
				{
					save(cp.getCommands());
				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				cout << green << cp.atToken(1) << " saved." << reset << endl;
				break;
			case CommandType::FIND:
				try
				{
					bool isFound = false;
					isFound = getHierarchy(cp.atToken(1))->find(cp.atToken(2));
					cout << yellow << cp.atToken(2) << (isFound ? " is" : " is not") << " employed in " << cp.atToken(1) << "." << reset << endl;
				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				break;
			case CommandType::NUM_SUBORDINATES:
				try
				{
					int numSubordinates = getHierarchy(cp.atToken(1))->num_subordinates(cp.atToken(2));

					if (numSubordinates == -1)
						cout << red << "There is no employee with name " << cp.atToken(2) << " in " << cp.atToken(1) << reset << endl;
					else
						cout << yellow << cp.atToken(2) << " has " << numSubordinates << " subordinates." << reset << endl;
				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				break;
			case CommandType::MANAGER:
				try
				{
					string managerName = getHierarchy(cp.atToken(1))->manager(cp.atToken(2));

					if (managerName.empty())
						cout << red << cp.atToken(2) << " has no manager." << reset << endl;
					else
						cout << yellow << "The manager of " << cp.atToken(2) << " is " << managerName << "." << reset << endl;
				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				break;
			case CommandType::NUM_EMPLOYEES:
				try
				{
					int num_employees = getHierarchy(cp.atToken(1))->num_employees();

					cout << yellow << "There are " << num_employees << " employees in " << cp.atToken(1) << "." << reset << endl;
				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				break;
			case CommandType::OVERLOADED:
				try
				{
					int overloaded_employees = getHierarchy(cp.atToken(1))->num_overloaded();

					cout << yellow << "There are " << overloaded_employees << " overloaded employees in " << cp.atToken(1) << "." << reset << endl;
				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				break;
			case CommandType::JOIN:
				try
				{
					Hierarchy* h1 = getHierarchy(cp.atToken(1));
					Hierarchy* h2 = getHierarchy(cp.atToken(2));
					Hierarchy* joined = DBG_NEW Hierarchy(h1->join(*h2));
					joined->setName(cp.atToken(3));
					fHierarchies.push_back(joined);

					cout << yellow << cp.atToken(3) << " created." << reset << endl;
				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				break;
			case CommandType::FIRE:
				try
				{
					if (getHierarchy(cp.atToken(1))->fire(cp.atToken(2)))
						cout << yellow << cp.atToken(2) << " was fired." << reset << endl;
					else
						cout << red << cp.atToken(2) << " was not found." << reset << endl;
				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				break;
			case CommandType::EXIT:
				try
				{

				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				for (size_t i = 0; i < fHierarchies.size(); i++)
					delete fHierarchies[i];

				cout << termcolor::magenta << "Goodbye!" << reset << endl;

				return;
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