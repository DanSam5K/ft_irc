/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  							     RequestParser.cpp 	 					     #
#****************************************************************************/

#include "RequestParser.hpp"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
#include "InputTokenizer.hpp"

#define NUMBER_CMD 24

// Command list
std::string commands[NUMBER_CMD] = {
	"ADMIN", "INFO", "VERSION", "USERS", "NICK",
	"PRIVMSG", "USER", "QUIT", "JOIN", "LIST", "NAMES",
	"SUMMON", "KICK", "PART", "MODE", "CAP", "PASS",
	"INVITE", "TOPIC", "PING", "PONG"
};

// Parameter list
std::string params[NUMBER_CMD][10] = {
	{"target"}, {"target"}, {}, {}, 
	{"nickname"}, {"msgtarget", "text to be sent"},
	{"user", "mode", "unused", "realname"}, 
	{"Quit Message"}, {"channel", "key"}, 
	{"channel"}, {"channel", "target"}, 
	{"user", "target", "channel"}, 
	{"channel", "user", "comment"}, 
	{"channel", "Part Message"}, 
	{"modeTarget", "rawModeString", "modeChar modeArguments"}, 
	{"a"}, 	{"password"}, {"nickname", "channel"}, 
	{"channel", "topic"}, {"token"}, 
	{"token"}
};

// Argument types for each command's parameters
ArgumentType params_states[NUMBER_CMD][10] = {
	{OPTIONAL_ARG}, {OPTIONAL_ARG}, {}, {}, 
	{REQUIRED}, {REQUIRED, OPTIONAL_ARG}, 
	{REQUIRED, REQUIRED, REQUIRED, REQUIRED}, 
	{OPTIONAL_ARG}, {MULTIPLE, OPTIONAL_LIST}, 
	{OPTIONAL_LIST}, {OPTIONAL_LIST, OPTIONAL_ARG},
	{REQUIRED, OPTIONAL_ARG, MULTI_CHOICE}, 
	{MULTIPLE, MULTIPLE, OPTIONAL_ARG}, {MULTIPLE, OPTIONAL_ARG}, 
	{REQUIRED, OPTIONAL_ARG, MULTI_CHOICE}, {OPTIONAL_ARG}, {OPTIONAL_ARG}, 
	{REQUIRED, REQUIRED}, {REQUIRED, OPTIONAL_ARG}, {REQUIRED, OPTIONAL_ARG}, 
	{REQUIRED}, {REQUIRED}
};

// Constructor
RequestParser::RequestParser(std::string rawInput) : tokenizer(InputTokenizer(
	            rawInput)), currentIndex(0)
{
	tokenizer.tokenize();
	parsedTokens = tokenizer.getAllTokens();
	if (parsedTokens.size() == 0)
	{
		throw RequestParser::InvalidCommandException();
	}
	command = parsedTokens[0];
	normalizeCommand(command);
	shiftArguments();
}

void RequestParser::normalizeCommand(std::string &command) // Normalize the command (uppercase)
{
	std::string::iterator it = command.begin();
	for (; it != command.end(); it++)
	{
		*it = toupper(*it);
	}
}

void RequestParser::parse() // Parse the request
{


	if (!is_in_array(command, commands, NUMBER_CMD))
	{
		throw RequestParser::InvalidCommandException();
	}

	if (is_in_array(command, commands, NUMBER_CMD))
	{
		try
		{
			parseAdvanced();
		}
		catch (std::out_of_range const &e)
		{
			throw MissingArgumentsException();
		}
	}
}

void RequestParser::parseNone() // Parse the request with no specific command
{
	if (parsedTokens.size() > 1)
	{
		throw RequestParser::TooManyArgumentsException();
	}
	return;
}

void RequestParser::parseAdvanced() // Parse the request with advanced command
{
	unsigned int command_index = get_array_index(command, commands, NUMBER_CMD);
	unsigned int i = 0;
	std::string current_param = params[command_index][i] ;
	ArgumentType modeTargetType = params_states[command_index][i] ;

	while (!current_param.empty())
	{
		if (!setCurrentArgument(current_param, modeTargetType))
		{
			throw MissingArgumentsException();
		}
		shiftArguments();
		i++;
		current_param = params[command_index][i] ;
		modeTargetType = params_states[command_index][i] ;
	}
	if (parsedTokens.size() > i + 1 && command != "MODE")
	{
		throw TooManyArgumentsException();
	}
}

std::string RequestParser::getCurrentToken() // Get the current token
{
	if (currentIndex >= parsedTokens.size())
	{
		throw std::out_of_range("No more parsedTokens");
	}
	return (parsedTokens[currentIndex]);
}

bool RequestParser::setCurrentArgument(std::string argName) // Set the current argument
{
	try
	{
		std::string currentToken = getCurrentToken();
		parsedArguments[argName] = currentToken;
		return (true);
	}
	catch (std::out_of_range const &e)
	{
		return (false);
	}
}

std::list<std::string> RequestParser::splitToList(std::string currentToken) // Split the current token into a list
{
	std::list<std::string> args_list;

	char *token = (char *)currentToken.c_str();

	char *subtoken = std::strtok(token, ",");
	args_list.push_back((std::string)subtoken);
	while (subtoken != NULL)
	{
		subtoken = strtok(NULL, ",");
		if (subtoken != NULL)
		{
			args_list.push_back(subtoken);
		}
	}
	return args_list;
}

bool RequestParser::setCurrentArgument(std::string argName, ArgumentType argType) // Set the current argument
{
	try
	{
		std::string currentToken = getCurrentToken();
		if (argType == MULTIPLE || argType == OPTIONAL_LIST)
		{
			argumentLists[argName] = splitToList(currentToken);
		}
		else if (argType == MULTI_CHOICE)
		{
			argumentLists[argName] = collectRemainingTokens(currentToken);
		}
		else
		{
			parsedArguments[argName] = currentToken;
		}
	}
	catch (std::out_of_range const &e)
	{
		if (argType == REQUIRED || argType == MULTIPLE)
		{
			return (false);
		}
		return (true);
	}
	return (true);
}

std::list<std::string> RequestParser::collectRemainingTokens(std::string currentToken) // Collect remaining tokens
{
	std::list<std::string> modeArguments;
	std::string token;
	bool unfinished = true;

	modeArguments.push_back(currentToken);
	while (unfinished)
	{
		try
		{
			shiftArguments();
			token = getCurrentToken();
			modeArguments.push_back(token);
		}
		catch(std::out_of_range &e)
		{
			(void)e;
			unfinished = false;
		}
	}
	return modeArguments;
}

bool RequestParser::setCurrentArgumentList(std::string argName) // Set the current argument list
{
	char *subtoken;

	try
	{
		std::string currentToken = getCurrentToken();
		char *token = (char *)currentToken.c_str();

		subtoken = std::strtok(token, ",");
		parsedArguments[argName] = subtoken;
		while (subtoken != NULL)
		{
			subtoken = strtok(NULL, ",");
			parsedArguments[argName] = subtoken;
		}
		return (true);
	}
	catch (std::out_of_range const &e)
	{
		return (false);
	}
}

void RequestParser::shiftArguments() // Shift the current index to the next argument
{
	currentIndex++;
}

std::string RequestParser::getCommandMessage() // Get the command message
{
	return (command);
}

std::string RequestParser::getCommandArgument(std::string argName) // Get the command argument
{
	return (parsedArguments[argName]);
}

std::list<std::string> RequestParser::getCommandArgumentList(std::string argName) // Get the command argument list
{
	return (argumentLists[argName]);
}

std::vector<std::string> RequestParser::getAllTokens() // Get all tokens
{
	return (parsedTokens);
}

bool RequestParser::containsArgument(std::string argName) // Check if the argument exists
{
	
	if (parsedArguments.count(argName) == 0)
	{
		return (false);
	}
	return (true);
}

bool RequestParser::checkCommandArgumentList(std::string argName) // Check if the command argument list exists
{
	if (argumentLists.count(argName) == 0)
	{
		return (false);
	}
	return (true);
}


RequestParser::~RequestParser() {}

const char* RequestParser::MissingArgumentsException::what() const throw() // Missing arguments exception
{
	return ("Not enough arguments provided");
}

const char* RequestParser::TooManyArgumentsException::what() const throw() // Too many arguments exception
{
	return ("Too many arguments provided");
}

const char* RequestParser::InvalidCommandException::what() const throw() // Invalid command exception
{
	return ("Invalid command");
}
