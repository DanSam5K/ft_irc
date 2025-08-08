#include "InputTokenizer.hpp"

InputTokenizer::InputTokenizer(std::string rawInput) : content(rawInput),
	currentState(READING_PARAM), index(0) {}

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

char InputTokenizer::peekChar() const
{
	if (index < content.size())
	{
		return (content[index]);
	}
	return ('\0');
}

void InputTokenizer::collectChar()
{
	tempBuffer.push_back(peekChar());
	shiftArguments();
}

void InputTokenizer::shiftArguments()
{
	index++;
}

void InputTokenizer::change_state(ParseState new_state)
{
	currentState = new_state;
}

void InputTokenizer::storeToken()
{
	parsedTokens.push_back(tempBuffer);
	tempBuffer.clear();
}

std::vector<std::string> InputTokenizer::getAllTokens()
{
	return (parsedTokens);
}

InputTokenizer::~InputTokenizer() {}
