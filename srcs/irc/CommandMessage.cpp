/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  						     CommandMessage.cpp 	 					     #
#****************************************************************************/

#include "CommandMessage.hpp"
#include <stdexcept>
#include "RequestParser.hpp"

CommandMessage::CommandMessage(ClientUser &userRef, std::string rawInput) : _originUser(userRef),
	parser(NULL)
{
	if (rawInput.size() > MESSAGE_MAX_SIZE)
	{
		throw std::invalid_argument("CommandMessage: Input exceeds allowed length");
	}
	if (rawInput.size() < MESSAGE_MIN_SIZE)
	{
		throw std::invalid_argument("CommandMessage: Input below min length");
	}
	if (!(rawInput[rawInput.size() - 2] == '\r'
	        && rawInput[rawInput.size() - 1] == '\n'))
	{
		throw std::invalid_argument("CommandMessage: Invalid termination sequence in message");
	}
	rawInput.resize(rawInput.size() - 2);

	parser = new RequestParser(rawInput);
}

void CommandMessage::processInput() // Process the incoming command message, calling the parser
{
	parser->parse();
}

CommandMessage::~CommandMessage() 
{
	delete parser;
};

std::string CommandMessage::getCommandMessage() // Get the raw command message
{
	return (parser->getCommandMessage());
}

RequestParser CommandMessage::get_parser() // Get the request parser
{
	return (*parser);
}

std::string CommandMessage::getCommandArgument(std::string argumentName) // Get a specific command argument
{
	if (parser->containsArgument(argumentName))
	{
		return (parser->getCommandArgument(argumentName));
	}
	else
	{
		throw std::out_of_range("CommandMessage: No such argument found: [" + argumentName + "]");
	}
}

std::list<std::string> CommandMessage::getCommandArgumentList(std::string argumentName) // Get a list of command arguments
{
	if (parser->checkCommandArgumentList(argumentName))
	{
		return (parser->getCommandArgumentList(argumentName));
	}
	else
	{
		throw std::out_of_range("CommandMessage: No such argument found: [" + argumentName + "]");
	}
}

bool CommandMessage::checkCommandArgument(std::string argumentName) // Check if a specific command argument exists
{
	return (parser->containsArgument(argumentName));
}

bool CommandMessage::checkCommandArgumentList(std::string argumentName) // Check if a specific command argument list exists
{
	return (parser->checkCommandArgumentList(argumentName));
}

ClientUser &CommandMessage::getMessageSender() const // Get the user who sent the message
{
	return (_originUser);
}
