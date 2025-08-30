/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  						         ModeParser.hpp    	 					     #
#****************************************************************************/

#ifndef MODE_PARSER_HPP
# define MODE_PARSER_HPP

#include "ft_irc.hpp"

// Represents the current state while parsing a mode string (waiting for sign or mode character)
enum ModeParsingState
{
    AWAITING_SIGN,         // Waiting for '+' or '-' sign (to know if we're adding or removing a mode)
    EXPECTING_MODE_CHAR    // Expecting a mode character after sign (to know which mode to change)
};

// Indicates whether a mode is being added or removed
enum ModeChangeType {
    ADD_MODE,              // Adding a mode flag (to a user or channel)
    REMOVE_MODE            // Removing a mode flag (from a user or channel)
};

// Parses IRC mode strings and separates added/removed mode flags
class ModeParser
{
    private:
        std::string rawModeString;      // The raw mode string to be parsed

        ModeParsingState parseState;    // Tracks current parsing state
        ModeChangeType modeTargetType;  // Indicates add/remove operation
        std::string addedModeFlags;     // Stores mode flags to add
        std::string removedModeFlags;   // Stores mode flags to remove

        unsigned int currentIndex;      // Current position in the mode string

        // Returns the current character in the mode string
        char getCurrentChar();

        // Processes a mode flag character and updates the appropriate flag string
        void parseModeFlag();

        // Processes a '+' or '-' sign and updates the parsing state
        void parsePrefixSign();

    public:
        // Constructs a ModeParser for the given mode string
        ModeParser(std::string rawModeString);
        virtual ~ModeParser();
        
        // Parses the mode string and fills added/removed flags
        void parse();

        // Returns all mode flags to be added
        std::string getAddedModeFlags();

        // Returns all mode flags to be removed
        std::string getRemovedModeFlags();

        // Advances to the next character in the mode string
        void shiftArguments();

        // Exception thrown for invalid mode strings
        class InvalidModestringException : public std::exception {};
};

#endif