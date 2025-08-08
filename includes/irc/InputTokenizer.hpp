#ifndef INPUT_TOKENIZER_HPP
#define INPUT_TOKENIZER_HPP

#include "ft_irc.hpp"

enum ParseState {
	READING_PARAM, 
	READING_SPACE, 
	READING_EXTENDED_PARAM
};

class InputTokenizer
{
	private:

		std::string content;
		std::vector<std::string> parsedTokens;
		ParseState currentState;
		unsigned int index;

		std::string tempBuffer;

		void tokenizeParam();
		void tokenizeSpace();
		void tokenizeExtendedParam();
		char peekChar() const;
		void collectChar();
		void storeToken();
		void shiftArguments();

	public:
		InputTokenizer(std::string content);
		virtual ~InputTokenizer();
		void tokenize();
		void change_state(ParseState new_state);
		std::vector<std::string> getAllTokens();
};

#endif
