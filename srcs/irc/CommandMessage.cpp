#include "CommandMessage.hpp"
#include <stdexcept>
#include "RequestParser.hpp"

CommandMessage::CommandMessage(ClientUser &userRef, std::string &rawInput) : _originUser(userRef),
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

void CommandMessage::processInput()
{
	parser->parse();
}

CommandMessage::~CommandMessage()
{
	delete parser;
};

std::string CommandMessage::getCommandMessage()
{
	return (parser->getCommandMessage());
}

RequestParser CommandMessage::get_parser()
{
	return (*parser);
}

std::string CommandMessage::getCommandArgument(const std::string &argumentName) const
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

std::list<std::string> CommandMessage::getCommandArgumentList(const std::string argumentName) const
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

bool CommandMessage::checkCommandArgument(const std::string argumentName) const
{
	return (parser->containsArgument(argumentName));
}

bool CommandMessage::checkCommandArgumentList(const std::string argumentName) const
{
	return (parser->checkCommandArgumentList(argumentName));
}

ClientUser &CommandMessage::getMessageSender() const
{
	return (_originUser);
}
