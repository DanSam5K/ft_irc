/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  						       RequestParser.hpp  				    	     #
#****************************************************************************/

#ifndef REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP

#include <exception>
#include "ft_irc.hpp"
#include "InputTokenizer.hpp"

// Used for argument parsing
enum ArgumentType {
	REQUIRED, // Argument is required
	OPTIONAL_ARG, // Argument is optional
	MULTIPLE, // Argument can have multiple values
	OPTIONAL_LIST, // Argument is an optional list
	MULTI_CHOICE, // Argument is a multi-choice option
};

class RequestParser
{
    private:
        std::string command; // Stores the IRC command name

        InputTokenizer tokenizer; // Tokenizer for breaking input into tokens

        std::vector<std::string> parsedTokens; // Holds all tokens parsed from input
        unsigned int	currentIndex;           // Current position in token list

        std::map<std::string, std::string> parsedArguments; // Maps argument names to their values
        std::map<std::string, std::list<std::string> > argumentLists; // Maps argument names to lists of values

        // Normalizes the command string (e.g., uppercase conversion)
        void normalizeCommand(std::string &command);

    public:
        // Constructs a parser from raw IRC input
        RequestParser(std::string rawInput);
        virtual ~RequestParser();
        
        // Returns the token currently being processed
        std::string getCurrentToken();

        // Sets a named argument from the current token
        bool setCurrentArgument(std::string argName);

        // Sets a named argument list from the current token
        bool setCurrentArgumentList(std::string argName);

        // Main parsing routine
        void parse();

        // Advances to the next token
        void shiftArguments();

        // Handles parsing when no arguments are present
        void parseNone();

        // Handles advanced parsing logic for complex commands
        void parseAdvanced();
        
        // Returns the command name
        std::string getCommandMessage();

        // Sets an argument with a specific type
        bool setCurrentArgument(std::string argName, ArgumentType argType);

        // Collects all remaining tokens into a list
        std::list<std::string> collectRemainingTokens(std::string currentToken);

        // Splits a token into a list based on delimiters
        std::list<std::string> splitToList(std::string currentToken);

        // Retrieves the value of a named argument
        std::string getCommandArgument(std::string argName);

        // Retrieves a list of values for a named argument
        std::list<std::string> getCommandArgumentList(std::string argName);
        
        // Checks if an argument exists
        bool containsArgument(std::string argName);

        // Returns all tokens parsed from input
        std::vector<std::string> getAllTokens();

        // Checks if an argument list exists
        bool checkCommandArgumentList(std::string argName);

        // Exception thrown when too many arguments are provided
        class TooManyArgumentsException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
    
        // Exception thrown when required arguments are missing
        class MissingArgumentsException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
    
        // Exception thrown for invalid command syntax
        class InvalidCommandException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
};

#endif