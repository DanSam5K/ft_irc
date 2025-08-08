#include "RequestParser.hpp"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
#include "Tokenizer.hpp"

#define NUMBER_CMD 24

std::string commands[NUMBER_CMD] = {"ADMIN", "INFO", "VERSION", "USERS", "NICK", "PRIVMSG", "USER", "QUIT", "JOIN", "LIST", "NAMES", "SUMMON", "KICK", "PART", "MODE", "CAP", "PASS", "KICK", "INVITE", "TOPIC", "PING", "PONG"};
std::string params[NUMBER_CMD][10] = {{"modeTarget"}, {"modeTarget"}, {}, {}, {"nickname"}, {"msgtarget", "text to be sent"}, {"user", "ArgumentType", "unused", "realname"}, {"Quit CommandMessage"}, {"channel", "key"}, {"channel"}, {"channel", "modeTarget"}, {"user", "modeTarget", "channel"}, {"channel", "user", "comment"}, {"channel", "Part CommandMessage"}, {"modeTarget", "rawModeString", "ArgumentType modeArguments"}, {"a"}, {"password"}, {"channel", "user", "comment"}, {"nickname", "channel"}, {"channel", "topic"}, {"token"}, {"token"}};

ArgumentType params_states[NUMBER_CMD][10] = {{Optional}, {Optional}, {}, {}, {Mandatory}, {Mandatory, Optional}, {Mandatory, Mandatory, Mandatory, Mandatory}, {Optional}, {List, ListOptional}, {ListOptional}, {ListOptional, Optional}, {Mandatory, Optional, Optional}, {List, List, Optional}, {List, Optional}, {Mandatory, Optional, MultiOptional}, {Optional}, {Optional}, {Mandatory, List, Optional}, {Mandatory, Mandatory}, {Mandatory, Optional}, {Mandatory}, {Mandatory}};


RequestParser::RequestParser(std::string raw_content) : tokenizer(Tokenizer(
	            raw_content)), current(0)
{
	tokenizer.tokenize();
	tokens = tokenizer.get_tokens();
	if (tokens.size() == 0)
	{
		throw RequestParser::UnknownCommandException();
	}
	command = tokens[0]; // TODO: add to upper here
	command_to_upper(command);
	shiftArguments();
}

void RequestParser::command_to_upper(std::string &command)
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
		throw RequestParser::UnknownCommandException();
	}

	if (is_in_array(command, commands, NUMBER_CMD))
	{
		try
		{
			parse_complex();
		}
		catch (std::out_of_range const &e)
		{
			throw NeedMoreParamsException();
		}
	}
}

void RequestParser::parse_no_arg(void)
{
	if (tokens.size() > 1)
	{
		throw RequestParser::TooManyParamsException();
	}
	return;
}

void RequestParser::parse_complex(void)
{
	unsigned int command_index = get_array_index(command, commands, NUMBER_CMD);
	unsigned int i = 0;
	std::string current_param = params[command_index][i] ;
	ArgumentType modeTargetType = params_states[command_index][i] ;

	while (!current_param.empty())
	{
		if (!set_current_arg(current_param, modeTargetType))
		{
			throw NeedMoreParamsException();
		}
		shiftArguments();
		i++;
		current_param = params[command_index][i] ;
		modeTargetType = params_states[command_index][i] ;
	}
	if (tokens.size() > i + 1 && command != "MODE")
	{
		throw TooManyParamsException();
	}
}

std::string RequestParser::get_current_token()
{
	if (current >= tokens.size())
	{
		throw std::out_of_range("No more tokens");
	}
	return (tokens[current]);
}

bool RequestParser::set_current_arg(std::string arg_name)
{
	try
	{
		std::string current_token = get_current_token();
		args[arg_name] = current_token;
		return (true);
	}
	catch (std::out_of_range const &e)
	{
		return (false);
	}
}

std::list<std::string> RequestParser::arg_to_list(std::string current_token)
{
	std::list<std::string> args_list;

	char *token = (char *)current_token.c_str();

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

bool RequestParser::set_current_arg(std::string arg_name, ArgumentType arg_type)
{
	try
	{
		std::string current_token = get_current_token();
		if (arg_type == List || arg_type == ListOptional)
		{
			args_lists[arg_name] = arg_to_list(current_token);
		}
		else if (arg_type == MultiOptional)
		{
			args_lists[arg_name] = get_rest_tokens(current_token);
		}
		else
		{
			args[arg_name] = current_token;
		}
	}
	catch (std::out_of_range const &e)
	{
		if (arg_type == Mandatory || arg_type == List)
		{
			return (false);
		}
		return (true);
	}
	return (true);
}

std::list<std::string> RequestParser::get_rest_tokens(std::string current_token)
{
	std::list<std::string> modeArguments;
	std::string token;
	bool unfinished = true;

	modeArguments.push_back(current_token);
	while (unfinished)
	{
		try
		{
			shiftArguments();
			token = get_current_token();
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

bool RequestParser::set_current_arg_list(std::string arg_name)
{
	char *subtoken;

	try
	{
		std::string current_token = get_current_token();
		char *token = (char *)current_token.c_str();

		subtoken = std::strtok(token, ",");
		args[arg_name] = subtoken;
		while (subtoken != NULL)
		{
			subtoken = strtok(NULL, ",");
			args[arg_name] = subtoken;
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
	current++;
}

std::string RequestParser::getCommandMessage(void)
{
	return (command);
}

std::string RequestParser::getCommandArgument(std::string arg_name)
{
	return (args[arg_name]);
}

std::list<std::string> RequestParser::getCommandArgumentList(std::string arg_name)
{
	return (args_lists[arg_name]);
}

std::vector<std::string> RequestParser::get_tokens(void)
{
	return (tokens);
}

bool RequestParser::has_arg(std::string arg_name)
{
	// XXX might be improved by checking the command types and which modeArguments should exist
	if (args.count(arg_name) == 0)
	{
		return (false);
	}
	return (true);
}

bool RequestParser::checkCommandArgumentList(std::string arg_name)
{
	// XXX might be improved by checking the command types and which modeArguments should exist
	if (args_lists.count(arg_name) == 0)
	{
		return (false);
	}
	return (true);
}


RequestParser::~RequestParser() {}

const char* RequestParser::NeedMoreParamsException::what() const throw()
{
	return ("Not enough parameters provided");
}

const char* RequestParser::TooManyParamsException::what() const throw()
{
	return ("Too many parameters provided");
}

const char* RequestParser::UnknownCommandException::what() const throw()
{
	return ("Unknown command");
}
