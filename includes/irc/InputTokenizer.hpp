#ifndef INPUT_TOKENIZER_HPP
#define INPUT_TOKENIZER_HPP

#include "ft_irc.hpp"

// States used during parsing of IRC input strings
enum ParseState {
    READING_PARAM,         // Currently reading a regular parameter
    READING_SPACE,         // Currently processing whitespace between parameters
    READING_EXTENDED_PARAM // Reading an extended parameter (e.g., trailing argument)
};

// Splits raw IRC input into tokens for command processing
class InputTokenizer
{
    private:
        std::string content;                  // The raw input string to be tokenized
        std::vector<std::string> parsedTokens;// Stores the resulting tokens after parsing
        ParseState currentState;              // Tracks the current parsing state
        unsigned int index;                   // Current position in the input string

        std::string tempBuffer;               // Temporary buffer for building tokens

        // Handles parsing of a standard parameter
        void tokenizeParam();

        // Handles parsing of whitespace between parameters
        void tokenizeSpace();

        // Handles parsing of extended (trailing) parameters
        void tokenizeExtendedParam();

        // Returns the next character in the input without advancing the index
        char peekChar() const;

        // Adds the current character to the buffer and advances the index
        void collectChar();

        // Saves the current buffer as a token and clears it
        void storeToken();

        // Shifts arguments for further processing if needed
        void shiftArguments();

    public:
        // Initializes the tokenizer with the input string
        InputTokenizer(std::string content);

        virtual ~InputTokenizer();

        // Performs the tokenization process
        void tokenize();

        // Changes the parsing state
        void change_state(ParseState new_state);

        // Returns all parsed tokens
        std::vector<std::string> getAllTokens();
};

#endif



// enum ParseState {
// 	READING_PARAM, 
// 	READING_SPACE, 
// 	READING_EXTENDED_PARAM
// };

// class InputTokenizer
// {
// 	private:

// 		std::string content;
// 		std::vector<std::string> parsedTokens;
// 		ParseState currentState;
// 		unsigned int index;

// 		std::string tempBuffer;

// 		void tokenizeParam();
// 		void tokenizeSpace();
// 		void tokenizeExtendedParam();
// 		char peekChar() const;
// 		void collectChar();
// 		void storeToken();
// 		void shiftArguments();

// 	public:
// 		InputTokenizer(std::string content);
// 		virtual ~InputTokenizer();
// 		void tokenize();
// 		void change_state(ParseState new_state);
// 		std::vector<std::string> getAllTokens();
// };

// #endif
