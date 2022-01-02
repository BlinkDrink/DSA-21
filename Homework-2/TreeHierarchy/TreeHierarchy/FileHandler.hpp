#pragma once
#include <fstream>
#include <string>
#include<climits>
#include "StringHelper.hpp"
#include "termcolor.hpp" // Open-source libarary used for output colorization of characters
#include "interface.hpp"
#include "CommandParser.hpp"

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

/// @brief Singleton FileHandler, used for end-logic of the program
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
		cout << yellow << "\t\t\tMENU" << endl;
		cout << "help\t\t\t\t\t\t\t\t\t\t - shows this list of commands" << endl;
		cout << "load <name_of_hierarchy> <name_of_file>\t\t\t\t\t\t - loads an object with name <name_of_hierarchy> from file <name_of_file>" << endl;
		cout << "save <name_of_hierarchy> <name_of_file>\t\t\t\t\t\t - saves the current heirarchy in <name_of_file> file with <name_of_hierarchy> name. If no name of file is given - prints the result instead" << endl;
		cout << "find <name_of_hierarchy> <name_of_employee>\t\t\t\t\t - checks if <name_of_employee> exists in <name_of_hierarchy>" << endl;
		cout << "num_subordinates <name_of_hierarchy> <name_of_employee>\t\t\t\t - shows how many subordinates <name_of_employee> has in <name_of_hierarchy>" << endl;
		cout << "manager <name_of_hierarchy> <name_of_employee>\t\t\t\t\t - shows the manager of <name_of_employee> in <name_of_hierarchy>" << endl;
		cout << "num_employees <name_of_hierarchy>\t\t\t\t\t\t - shows how many employees work in <name_of_hierarchy>" << endl;
		cout << "overloaded <name_of_hierarchy>\t\t\t\t\t\t\t - shows how many employees have more than 20 employees working under them in <name_of_hierarchy>" << endl;
		cout << "join <name_of_hierarchy_1> <name_of_hierarchy_2> <name_of_hierarchy_result>\t - unites 2 hierarchies into one under the name of <name_of_hierarchy_result" << endl;
		cout << "fire <name_of_hierarchy> <name_of_employee>\t\t\t\t\t - fires the employee with name <name_of_employee> from <name_of_hierarchy>" << endl;
		cout << "hire <name_of_hierarchy> <name_of_employee> <name_of_boss>\t\t\t - hires <name_of_employee> under <name_of_boss> in <name_of_hierarchy>" << endl;
		cout << "salary <name_of_hierarchy> <name_of_employee>\t\t\t\t\t - shows the salary of <name_of_employee> in <name_of_hierarchy>" << endl;
		cout << "incorporate <name_of_hierarchy>\t\t\t\t\t\t\t - incorporates the branch; this operation is applied on <name_of_hierarchy>" << endl;
		cout << "modernize <name_of_hierarchy>\t\t\t\t\t\t\t - modernizes the branch; this operation is applied on <name_of_hierarchy>" << endl;
		cout << "exit\t\t\t\t\t\t\t\t\t\t - exits the program" << reset << endl;
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

			Hierarchy* res = new Hierarchy(result);
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

			Hierarchy* res = new Hierarchy(result);
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
			cout << getHierarchy(commands.at(1))->print();
		}
		else if (commands.size() == 3)
		{
			fFile.open(commands.at(2), std::ios_base::out);
			fFilePath = commands.at(2);

			if (!fFile.is_open())
				throw invalid_argument("Couldn't open file for writing.");

			Hierarchy* tmp = getHierarchy(commands.at(1));
			fFile << tmp->print();
			tmp->setSaved(true);
			fFile.close();
		}
	}

	/// @brief Finds the hierarchy with the given name
	/// @param name - name of hierarchy
	/// @return pointer to the desired hierarchy
	Hierarchy* getHierarchy(const string& name)
	{
		for (size_t i = 0; i < fHierarchies.size(); i++)
			if (fHierarchies[i]->getName() == name)
				return fHierarchies[i];

		throw invalid_argument("Hierarchy with such name was not found.");
	}

	/// @brief Clears the dynamically allcated hieararchies
	void clearHierarchies()
	{
		for (size_t i = 0; i < fHierarchies.size(); i++)
			delete fHierarchies[i];
	}

	/// @brief Finds all hierarchies that haven't been saved yet
	/// @return vector of all unsaved hierarchies
	vector<Hierarchy*> getUnsavedHierarchies()
	{
		vector<Hierarchy*> res;
		for (size_t i = 0; i < fHierarchies.size(); i++)
		{
			if (!fHierarchies[i]->getSaved())
				res.push_back(fHierarchies[i]);
		}
		return res;
	}

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
					Hierarchy* joined = new Hierarchy(h1->join(*h2));

					if (joined->print() == "")
					{
						delete joined;
						cout << red << "Couldn't join " << h1->getName() << " and " << h2->getName() << reset << endl;
						break;
					}

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
					Hierarchy* tmp = getHierarchy(cp.atToken(1));
					if (tmp->fire(cp.atToken(2)))
					{
						cout << yellow << cp.atToken(2) << " was fired." << reset << endl;
						tmp->setSaved(false);
					}
					else
					{
						cout << red << cp.atToken(2) << " was not found." << reset << endl;
					}
				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				break;
			case CommandType::HIRE:
				try
				{
					Hierarchy* tmp = getHierarchy(cp.atToken(1));
					if (tmp->hire(cp.atToken(2), cp.atToken(3)))
					{
						cout << yellow << cp.atToken(2) << " was hired." << reset << endl;
						tmp->setSaved(false);
					}
					else
					{
						cout << red << cp.atToken(2) << " was not found." << reset << endl;
					}
				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				break;
			case CommandType::SALARY:
				try
				{
					int salary = getHierarchy(cp.atToken(1))->getSalary(cp.atToken(2));
					if (salary != -1)
						cout << yellow << "The salary is " << salary << " BGN." << reset << endl;
					else
						cout << red << cp.atToken(2) << " was not found." << reset << endl;
				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				break;
			case CommandType::INCORPORATE:
				try
				{
					Hierarchy* tmp = getHierarchy(cp.atToken(1));
					tmp->incorporate();
					tmp->setSaved(false);
					cout << green << cp.atToken(1) << " incorporated." << reset << endl;
				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				break;
			case CommandType::MODERNIZE:
				try
				{
					Hierarchy* tmp = getHierarchy(cp.atToken(1));
					tmp->modernize();
					tmp->setSaved(false);
					cout << green << cp.atToken(1) << " modernized." << reset << endl;
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
					vector<Hierarchy*> unsavedHierarchies = getUnsavedHierarchies();
					for (size_t i = 0; i < unsavedHierarchies.size(); i++)
					{
						string fileName;
						cout << yellow << unsavedHierarchies[i]->getName() << " is modified, but not saved." << reset << endl;
						cout << yellow << "Enter file name to save it: " << reset << endl;
						cin >> fileName;

						save({ "save", unsavedHierarchies[i]->getName(), fileName });
					}
				}
				catch (const exception& e)
				{
					cout << red << e.what() << reset << endl;
					break;
				}

				clearHierarchies();
				cout << green << "Goodbye!" << reset << endl;
				return;
			}
		}
	}
};