/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  							     ModeParser.cpp 	 					     #
#****************************************************************************/

#include "ModeParser.hpp"
#include <cctype>
#include <exception>

ModeParser::ModeParser(std::string rawModeString) : rawModeString(rawModeString),
	parseState(AWAITING_SIGN), modeTargetType(ADD_MODE), currentIndex(0) {}

ModeParser::~ModeParser() {}

void ModeParser::parse() // Parse the mode string
{
	while (currentIndex < rawModeString.size())
	{
		switch (parseState)
		{
			case AWAITING_SIGN:
				parsePrefixSign();
				break;
			case EXPECTING_MODE_CHAR:
				parseModeFlag();
				break;
		}
	}
}

void ModeParser::parsePrefixSign() // Parse the prefix sign
{
	if (getCurrentChar() == '+')
	{
		modeTargetType = ADD_MODE;
		parseState = EXPECTING_MODE_CHAR;
		shiftArguments();
	}
	else if (getCurrentChar() == '-')
	{
		modeTargetType = REMOVE_MODE;
		parseState = EXPECTING_MODE_CHAR;
		shiftArguments();
	}
	else
	{
		throw InvalidModestringException();
	}

}

void ModeParser::parseModeFlag() // Parse the mode flag
{
	if (std::isalpha(getCurrentChar()))
	{
		if (modeTargetType == ADD_MODE)
		{
			addedModeFlags.push_back(getCurrentChar());
			shiftArguments();
		}
		else if (modeTargetType == REMOVE_MODE)
		{
			removedModeFlags.push_back(getCurrentChar());
			shiftArguments();
		}
	}
	else if (getCurrentChar() == '-' || getCurrentChar() == '+')
	{
		parseState = AWAITING_SIGN;
	}
	else
	{
		throw InvalidModestringException();
	}
}



std::string ModeParser::getAddedModeFlags() // Get added mode flags
{
	return (addedModeFlags);
}

std::string ModeParser::getRemovedModeFlags() // Get removed mode flags
{
	return (removedModeFlags);
}

void ModeParser::shiftArguments() // Shift the current index to the next character
{
	currentIndex++;
}

char ModeParser::getCurrentChar() // Get the current character
{
	if (currentIndex < rawModeString.size())
	{
		return (rawModeString[currentIndex]);
	}
	else
	{
		throw std::out_of_range("Index exceeds mode string length.");
	}
}
