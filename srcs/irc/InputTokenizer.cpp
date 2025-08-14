#include "InputTokenizer.hpp"

// Constructor: initializes tokenizer with input and sets starting state
InputTokenizer::InputTokenizer(std::string rawInput) : content(rawInput),
	currentState(READING_PARAM), index(0) {}

// Main tokenization loop: processes input string based on current state
void InputTokenizer::tokenize()
{
	while (index <= content.size())
	{
		if (currentState == READING_PARAM)
		{
			tokenizeParam();
		}
		else if (currentState == READING_SPACE)
		{
			tokenizeSpace();
		}
		else if (currentState == READING_EXTENDED_PARAM)
		{
			tokenizeExtendedParam();
		}
	}
}

// Handles parsing of regular parameters in the input
void InputTokenizer::tokenizeParam()
{
	switch (peekChar())
	{
		case ' ':
			change_state(READING_SPACE);
			if (!tempBuffer.empty())
			{
				storeToken();
			}
			shiftArguments();
			break;
		case '\0':
			if (!tempBuffer.empty())
			{
				storeToken();
			}
			shiftArguments();
			break;
		default:
			collectChar();
	}
}

// Handles whitespace between parameters
void InputTokenizer::tokenizeSpace()
{
	switch (peekChar())
	{
		case ' ':
			shiftArguments();
			break;
		case '\0':
			shiftArguments();
			break;
		case ':':
			change_state(READING_EXTENDED_PARAM);
			shiftArguments();
			break;
		default:
			change_state(READING_PARAM);
	}
}

// Handles extended (trailing) parameters, which may include spaces
void InputTokenizer::tokenizeExtendedParam()
{
	switch (peekChar())
	{
		case '\0':
			storeToken();
			shiftArguments();
			break;
		default:
			collectChar();
	}
}


// Changes the parsing state to the specified value
void InputTokenizer::change_state(ParseState new_state)
{
	currentState = new_state;
}


// Saves the current buffer as a token and clears it
void InputTokenizer::storeToken()
{
	parsedTokens.push_back(tempBuffer);
	tempBuffer.clear();
}

// Returns all tokens parsed from the input string
std::vector<std::string> InputTokenizer::getAllTokens()
{
	return (parsedTokens);
}

// Destructor: cleans up tokenizer resources
InputTokenizer::~InputTokenizer() {}

// Returns the current character or '\0' if at the end of input
char InputTokenizer::peekChar() const
{
	if (index < content.size())
	{
		return (content[index]);
	}
	return ('\0');
}

// Adds the current character to the buffer and advances the index
void InputTokenizer::collectChar()
{
	tempBuffer.push_back(peekChar());
	shiftArguments();
}

// Moves to the next character in the input string
void InputTokenizer::shiftArguments()
{
	index++;
}
