#ifndef COMMAND_MESSAGE_HPP
#define COMMAND_MESSAGE_HPP

#include "ft_irc.hpp"
#include "RequestParser.hpp"
#include "User.hpp"

#define MESSAGE_MAX_SIZE 512
#define MESSAGE_MIN_SIZE 2

class CommandMessage
{
	private:

		// std::string _command;
		// std::map<std::string, std::string> _arguments;
		// std::map<std::string, std::list<std::string> > _argumentList;

		User &_originUser;
		RequestParser *parser;

		RequestParser get_parser();

	public:
		CommandMessage(User &userRef, std::string &rawInput);
		virtual ~CommandMessage();

		std::string getCommandMessage();
		std::string getCommandArgument(const std::string &argumentName) const;
		std::list<std::string> getCommandArgumentList(const std::string argumentName) const;
		bool checkCommandArgument(const std::string argumentName) const;
		bool checkCommandArgumentList( const std::string argumentName) const;

		void processInput();

		User &getMessageSender() const;
};

#endif
