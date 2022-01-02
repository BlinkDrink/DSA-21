#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "CommandType.h"
#include "StringHelper.hpp"

using std::cin;
using std::getline;
using std::invalid_argument;
using std::out_of_range;
using sh = StringHelper;
using std::string;
using std::vector;

class CommandParser
{
private:
	string fRaw;
	vector<string> fTokens;

public:

	///@brief Sets the inner raw string of data and splits it into tokens
	///
	///@param str - string to be set and splited
	void setData(const string& data)
	{
		clearCmd();
		fRaw = data;

		tokenizeInnerString();

		if (fRaw.size() == 0 || fTokens.size() == 0 || fTokens.size() > 4)
			throw invalid_argument("Invalid command, check the number of arguments you've given");
	}

	/// @brief Splits raw into parts(tokens) and pushes them inside tokens private member.
	void tokenizeInnerString()
	{
		fTokens = sh::splitBy(fRaw, " ");
	}

	///	@brief Getter. Throws out_of_range if pos is invalid
	///
	///	@param pos - index to be accessed
	///	@return the element at index pos
	const string& atToken(size_t pos) const
	{
		if (pos >= fTokens.size())
			throw out_of_range("There are not enough arguments.");

		return fTokens[pos];
	}

	/// @brief Getter
	/// 
	/// @returns size of tokens
	size_t size() const
	{
		return fTokens.size();
	}

	/// @brief Getter
	/// @return raw string
	string& getRaw()
	{
		return fRaw;
	}

	/// @return the type of the input command
	CommandType getCommandType() const
	{
		string cmd = sh::toUpper(fTokens[0]);

		if (cmd == "HELP")
		{
			return CommandType::HELP;
		}
		else if (cmd == "LOAD")
		{
			return CommandType::LOAD;
		}
		else if (cmd == "SAVE")
		{
			return CommandType::SAVE;
		}
		else if (cmd == "FIND")
		{
			return CommandType::FIND;
		}
		else if (cmd == "NUM_SUBORDINATES")
		{
			return CommandType::NUM_SUBORDINATES;
		}
		else if (cmd == "MANAGER")
		{
			return CommandType::MANAGER;
		}
		else if (cmd == "NUM_EMPLOYEES")
		{
			return CommandType::NUM_EMPLOYEES;
		}
		else if (cmd == "OVERLOADED")
		{
			return CommandType::OVERLOADED;
		}
		else if (cmd == "JOIN")
		{
			return CommandType::JOIN;
		}
		else if (cmd == "FIRE")
		{
			return CommandType::FIRE;
		}
		else if (cmd == "HIRE")
		{
			return CommandType::HIRE;
		}
		else if (cmd == "SALARY")
		{
			return CommandType::SALARY;
		}
		else if (cmd == "INCORPORATE")
		{
			return CommandType::INCORPORATE;
		}
		else if (cmd == "MODERNIZE")
		{
			return CommandType::MODERNIZE;
		}

		return CommandType::EXIT;
	}

	/// @brief Getter
	/// @return vector of commands
	const vector<string>& getCommands() const { return fTokens; }

	///	@brief Clears raw, leaves it empty. Clears tokens, leaves it empty
	void clearCmd()
	{
		fRaw.clear();
		fTokens.clear();
	}
};