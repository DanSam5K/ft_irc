#include "ModeParser.hpp"
#include <cctype>
#include <exception>

ModeParser::ModeParser(const std::string &rawModeString) : rawModeString(rawModeString),
	parseState(EXPECTING_MODE_CHAR), modeTargetType(ADD_MODE), currentIndex(0) {}

ModeParser::~ModeParser() {}

void ModeParser::parse()
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

void ModeParser::parsePrefixSign()
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

void ModeParser::parseModeFlag()
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

void ModeParser::shiftArguments()
{
	currentIndex++;
}

char ModeParser::getCurrentChar()
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


std::string ModeParser::getAddedModeFlags() const
{
	return (addedModeFlags);
}

std::string ModeParser::getRemovedModeFlags() const
{
	return (removedModeFlags);
}
