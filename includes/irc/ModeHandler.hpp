/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  						         ModeHandler.hpp    	 				     #
#****************************************************************************/

#ifndef MODE_HPPANDLER_HPP
# define MODE_HPPANDLER_HPP

#include "ModeParser.hpp"
#include "CommandMessage.hpp"
#include "ft_irc.hpp"
#include "reply_message.hpp"

class ConnectionManager;
class ModeHandler;

// Represents the modeTarget REMOVE_MODE of a mode change (either a channel or a user)
enum TargetCategory {
	TARGET_CHANNEL, 
	TARGET_USER
};

typedef  void (ModeHandler::*handler)(); // Type alias for mode handler functions, left empty, so it can be specialized later

class ModeHandler
{
	private:
		ConnectionManager &context; // Reference to the server's connection manager
		ClientUser &sender;          // Reference to the user sending the command
		CommandMessage &message;     // Reference to the command message being processed

		std::map<char, std::map<TargetCategory, std::map<std::string, handler> > >
		handlers; // Maps mode characters to their respective handlers.

		std::string modeTarget; // The target of the mode change (channel or user)
		std::list<std::string> modeArguments; // The arguments for the mode change

		std::string addedModeFlags; // The mode flags to be added
		std::string removedModeFlags; // The mode flags to be removed

		TargetCategory targetType; // The type of target (channel or user)

		ClientUser *targetUser; // Pointer to the user being targeted
		Channel *targetChannel;  // Pointer to the channel being targeted

		// Internal setup methods
		bool determineTargetType(); // Determines the type of target (channel or user)
		bool parseModeString(); // Parses the mode string from the command message
		void extractArguments(); // Extracts arguments from the command message
		bool containsInvalidModeString(std::string modes); // Checks for invalid mode strings
		bool containsInvalidModeChar(char modeChar); // Checks for invalid mode characters
		void executeModeChanges(); // Executes the mode changes

		// Specific mode flag handlers enable & disable
		void invisibleUserEnableHandler(); // Enables the invisible user mode
		void invisibleUserDisableHandler(); // Disables the invisible user mode

		void inviteChannelEnableHandler(); // Enables the invite-only channel mode
		void inviteChannelDisableHandler(); // Disables the invite-only channel mode

		void topicChannelEnableHandler(); // Enables the topic change channel mode
		void topicChannelDisableHandler(); // Disables the topic change channel mode

		void keyChannelEnableHandler(); // Enables the key channel mode
		void keyChannelDisableHandler(); // Disables the key channel mode

		void operatorChannelEnableHandler(); // Enables the operator channel mode
		void operatorChannelDisableHandler(); // Disables the operator channel mode

		void limitChannelUserEnableHandler(); // Enables the limit channel user mode
		void limitChannelUserDisableHandler(); // Disables the limit channel user mode

		// Arguments management
		std::string currentArguments(); // Returns the current arguments for the mode change
		std::string sanitizeModeString(std::string rawModeString); // Sanitizes the mode string (removes invalid characters)

	public:
		ModeHandler(ConnectionManager &context, ClientUser &sender, CommandMessage &message);
		virtual ~ModeHandler();
};

#endif
