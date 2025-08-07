#ifndef MODE_HANDLER_H
# define MODE_HANDLER_H

#include "ModeParsing.hpp"
#include "CommandMessage.hpp"
#include "ft_irc.hpp"
#include "reply.hpp"

// #include <map>
// #include <string>
// #include <iostream>


class ConnectionManager;
class ModeHandler;

// Represents the modeTarget type of a mode change (either a channel or a user)

enum TargetCategory {
	TARGET_CHANNEL, 
	TARGET_USER
};

typedef  void (ModeHandler::*handler)();

class ModeHandler
{
	private:
		ConnectionManager &context;
		User &sender;
		CommandMessage &message;

		std::map<char, std::map<TargetCategory, std::map<std::string, handler > > >
		handlers;

		std::string modeTarget;
		std::list<std::string> modeArguments;

		std::string addedModeFlags;
		std::string removedModeFlags;

		TargetCategory targetType;

		ModeParsing *parser;

		User *targetUser;
		Channel *targetChannel;

		// Internal setup methods
		bool determineTargetType();
		bool parseModeString();
		void extractArguments();
		bool containsInvalidModeString(std::string modes);
		bool containsInvalidModeChar(char mode);
		void executeModeChanges();

		// Specific mode flag handlers enable & disable
		void invisibleUserEnableHandler();
		void invisibleUserDisableHandler();

		void inviteChannelEnableHandler();
		void inviteChannelDisableHandler();
		
		void topicChannelEnableHandler();
		void topicChannelDisableHandler();
		
		void keyChannelEnableHandler();
		void keyChannelDisableHandler();

		void operatorChannelEnableHandler();
		void operatorChannelDisableHandler();

		void limitChannelUserEnableHandler();
		void limitChannelUserDisableHandler();

		// Arguments management
		void shiftArguments();
		std::string currentArguments();
		std::string santizeModeString(std::string modestring);

	public:
		ModeHandler(ConnectionManager &context, User &sender, CommandMessage &message);
		virtual ~ModeHandler();

		// void modeCommandHandler(CommandMessage &message, User &sender);
};

#endif
