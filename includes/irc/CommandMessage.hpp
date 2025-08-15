#ifndef COMMAND_MESSAGE_HPP
#define COMMAND_MESSAGE_HPP

#include "ft_irc.hpp"
#include "RequestParser.hpp"
#include "ClientUser.hpp"

// Maximum allowed size for an IRC message
#define MESSAGE_MAX_SIZE 512
// Minimum allowed size for an IRC message
#define MESSAGE_MIN_SIZE 2

// Represents a parsed IRC command received from a client
class CommandMessage
{
    private:
        ClientUser &_originUser;      // Reference to the user who sent the command
        RequestParser *parser;        // Pointer to the parser for extracting command details

        // Returns a copy of the parser object
        RequestParser get_parser();

    public:
        // Constructs a CommandMessage from raw input and the sending user
        CommandMessage(ClientUser &userRef, std::string rawInput);
        virtual ~CommandMessage();

        // Retrieves the command name (e.g., JOIN, PRIVMSG)
        std::string getCommandMessage();

        // Gets the value of a named argument (e.g., channel name, nickname)
        std::string getCommandArgument(std::string argumentName);

        // Gets a list of values for arguments that can have multiple entries
        std::list<std::string> getCommandArgumentList(std::string argumentName);

        // Checks if a named argument exists in the command
        bool checkCommandArgument(std::string argumentName);

        // Checks if a named argument list exists in the command
        bool checkCommandArgumentList(std::string argumentName);

        // Processes the raw input to extract command and arguments
        void processInput();

        // Returns the user who sent this command
        ClientUser &getMessageSender() const;
};

#endif









// #define MESSAGE_MAX_SIZE 512
// #define MESSAGE_MIN_SIZE 2

// class CommandMessage
// {
// 	private:

// 		// std::string _command;
// 		// std::map<std::string, std::string> _arguments;
// 		// std::map<std::string, std::list<std::string> > _argumentList;

// 		ClientUser &_originUser;
// 		RequestParser *parser;

// 		RequestParser get_parser();

// 	public:
// 		CommandMessage(ClientUser &userRef, std::string rawInput);
// 		virtual ~CommandMessage();

// 		std::string getCommandMessage();
// 		std::string getCommandArgument(std::string argumentName);
// 		std::list<std::string> getCommandArgumentList(std::string argumentName);
// 		bool checkCommandArgument(std::string argumentName);
// 		bool checkCommandArgumentList(std::string argumentName);

// 		void processInput();

// 		ClientUser &getMessageSender() const;
// };

// #endif
