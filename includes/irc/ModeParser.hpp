#ifndef MODE_PARSER_HPP
#define MODE_PARSER_HPP

#include "ft_irc.hpp"

// // Represents the current parsing modeParsingState in a rawModeString (expecting '+'/'-' or a mode character)
enum ModeParsingState
{
		AWAITING_SIGN,
		EXPECTING_MODE_CHAR
};
	
// // Represents the ModeChangeType of mode change: adding or removing a mode
enum ModeChangeType {
	ADD_MODE, 
	REMOVE_MODE
};

class ModeParser
{
	private:
		std::string rawModeString;

		ModeParsingState parseState;
		ModeChangeType modeTargetType;
		std::string addedModeFlags;
		std::string removedModeFlags;

		unsigned int currentIndex;

		char getCurrentChar();
		void parseModeFlag();
		void parsePrefixSign();

	public:
		ModeParser(const std::string &rawModeString);
		virtual ~ModeParser();
		void parse();
		std::string getAddedModeFlags() const;
		std::string getRemovedModeFlags() const;
		void shiftArguments();

		class InvalidModestringException : public std::exception {};
};

#endif
