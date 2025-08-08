#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <exception>
#include "ft_irc.hpp"
#include "InputTokenizer.hpp"


enum ArgumentType {
	REQUIRED, 
	OPTIONAL_ARG, 
	MULTIPLE, 
	OPTIONAL_LIST, 
	MULTI_CHOICE, 
	// SPECIAL_CASE
};

class RequestParser
{
	private:

		std::string command;

		InputTokenizer tokenizer;

		std::vector<std::string> parsedTokens;
		unsigned int	currentIndex;

		std::map<std::string, std::string> parsedArguments;
		std::map<std::string, std::list<std::string> > argumentLists;
		void normalizeCommand(std::string &command);

	public:

		RequestParser(std::string rawInput);
		virtual ~RequestParser();
		
		std::string getCurrentToken();
		bool setCurrentArgument(std::string argName);
		bool setCurrentArgumentList(std::string argName);
		void parse();
		void shiftArguments();

		void parseNone();
		// void parseBasic();
		// void parseModes();
		void parseAdvanced();
		
		std::string getCommandMessage();
		bool setCurrentArgument(std::string argName, ArgumentType argType);
		std::list<std::string> collectRemainingTokens(std::string currentToken);
		std::list<std::string> splitToList(std::string currentToken);
		std::string getCommandArgument(std::string argName);
		std::list<std::string> getCommandArgumentList(std::string argName);
		
		bool containsArgument(std::string argName);
		std::vector<std::string> getAllTokens();
		bool checkCommandArgumentList(std::string argName);

		class TooManyArgumentsException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
	
		class MissingArgumentsException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
	
		class InvalidCommandException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
	};
	
	#endif
	