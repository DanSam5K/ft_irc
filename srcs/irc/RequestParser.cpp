#include "RequestParser.hpp"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
#include "InputTokenizer.hpp"

#define NUMBER_CMD 24

std::string commands[NUMBER_CMD] = {
	"ADMIN", "STATUS", "VERSION", "USERS", "NICK",
	"PRIVMSG", "USER", "QUIT", "JOIN", "LIST", "NAMES",
	"SUMMON", "KICK", "PART", "MODE", "CAP", "PASS",
	"KICK", "INVITE", "TOPIC", "PING", "PONG"
};
std::string params[NUMBER_CMD][10] = {
	{"modeTarget"}, {"modeTarget"}, {}, {}, 
	{"nickname"}, {"msgtarget", "text to be sent"},
	{"user", "ArgumentType", "unused", "realname"}, 
	{"Quit CommandMessage"}, {"channel", "key"}, 
	{"channel"}, {"channel", "modeTarget"}, 
	{"user", "modeTarget", "channel"}, 
	{"channel", "user", "comment"}, 
	{"channel", "Part CommandMessage"}, 
	{"modeTarget", "rawModeString", "ArgumentType modeArguments"}, 
	{"a"}, {"password"}, {"channel", "user", "comment"}, {"nickname", "channel"}, {"channel", "topic"}, {"token"}, {"token"}};

ArgumentType params_states[NUMBER_CMD][10] = {{OPTIONAL_ARG}, {OPTIONAL_ARG}, {}, {}, {REQUIRED}, {REQUIRED, OPTIONAL_ARG}, {REQUIRED, REQUIRED, REQUIRED, REQUIRED}, {OPTIONAL_ARG}, {MULTIPLE, OPTIONAL_LIST}, {OPTIONAL_LIST}, {OPTIONAL_LIST, OPTIONAL_ARG}, {REQUIRED, OPTIONAL_ARG, OPTIONAL_ARG}, {MULTIPLE, MULTIPLE, OPTIONAL_ARG}, {MULTIPLE, OPTIONAL_ARG}, {REQUIRED, OPTIONAL_ARG, MULTI_CHOICE}, {OPTIONAL_ARG}, {OPTIONAL_ARG}, {REQUIRED, MULTIPLE, OPTIONAL_ARG}, {REQUIRED, REQUIRED}, {REQUIRED, OPTIONAL_ARG}, {REQUIRED}, {REQUIRED}};


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

void RequestParser::normalizeCommand(std::string &command)
{
	std::string::iterator it = command.begin();
	for (; it != command.end(); it++)
	{
		*it = toupper(*it);
	}
}

void RequestParser::parse(void)
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

void RequestParser::parseNone(void)
{
	if (parsedTokens.size() > 1)
	{
		throw RequestParser::TooManyArgumentsException();
	}
	return;
}

void RequestParser::parseAdvanced(void)
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

std::string RequestParser::getCurrentToken()
{
	if (currentIndex >= parsedTokens.size())
	{
		throw std::out_of_range("No more parsedTokens");
	}
	return (parsedTokens[currentIndex]);
}

bool RequestParser::setCurrentArgument(std::string argName)
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

std::list<std::string> RequestParser::splitToList(std::string currentToken)
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

bool RequestParser::setCurrentArgument(std::string argName, ArgumentType argType)
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

std::list<std::string> RequestParser::collectRemainingTokens(std::string currentToken)
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

bool RequestParser::setCurrentArgumentList(std::string argName)
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

void RequestParser::shiftArguments(void)
{
	currentIndex++;
}

std::string RequestParser::getCommandMessage(void)
{
	return (command);
}

std::string RequestParser::getCommandArgument(std::string argName)
{
	return (parsedArguments[argName]);
}

std::list<std::string> RequestParser::getCommandArgumentList(std::string argName)
{
	return (argumentLists[argName]);
}

std::vector<std::string> RequestParser::getAllTokens(void)
{
	return (parsedTokens);
}

bool RequestParser::containsArgument(std::string argName)
{
	
	if (parsedArguments.count(argName) == 0)
	{
		return (false);
	}
	return (true);
}

bool RequestParser::checkCommandArgumentList(std::string argName)
{
	if (argumentLists.count(argName) == 0)
	{
		return (false);
	}
	return (true);
}


RequestParser::~RequestParser() {}

const char* RequestParser::MissingArgumentsException::what() const throw()
{
	return ("Not enough arguments provided");
}

const char* RequestParser::TooManyArgumentsException::what() const throw()
{
	return ("Too many arguments provided");
}

const char* RequestParser::InvalidCommandException::what() const throw()
{
	return ("Invalid command");
}
