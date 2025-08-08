#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <exception>
#include "ft_irc.hpp"
#include "Tokenizer.hpp"


enum ArgumentType {Mandatory, Optional, List, ListOptional, MultiOptional, Special};

class RequestParser
{
	private:

		std::string command;

		Tokenizer tokenizer;

		std::vector<std::string> tokens;
		unsigned int	current;

		std::map<std::string, std::string> args;
		std::map<std::string, std::list<std::string> > args_lists;
		void command_to_upper(std::string &command);
	public:

		RequestParser(std::string raw_content);
		virtual ~RequestParser();
		std::string get_current_token();
		bool set_current_arg(std::string arg_name);
		bool set_current_arg_list(std::string arg_name);
		void parse();
		void shiftArguments();

		class TooManyParamsException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

		class NeedMoreParamsException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

		class UnknownCommandException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
		void parse_no_arg();
		void parse_simple();
		void parse_mode();
		std::string getCommandMessage();
		void parse_complex();
		bool set_current_arg(std::string arg_name, ArgumentType arg_type);
		std::list<std::string> get_rest_tokens(std::string current_token);
		std::list<std::string> arg_to_list(std::string current_token);
		std::string getCommandArgument(std::string arg_name);
		std::list<std::string> getCommandArgumentList(std::string arg_name);

		bool has_arg(std::string arg_name);
		std::vector<std::string> get_tokens();
		bool checkCommandArgumentList(std::string arg_name);
};

#endif /* PARSING_HPP */
