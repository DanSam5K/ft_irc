/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  							     ModeHandler.cpp 	 					     #
#****************************************************************************/

#include "MessageHandler.hpp"
#include "ModeHandler.hpp"
#include "CommandMessage.hpp"
#include "ConnectionManager.hpp"
#include "Channel.hpp"
#include "ModeParser.hpp"
#include "RequestParser.hpp"
#include "reply_message.hpp"
#include <cctype>
#include <cstdlib>
#include <exception>
#include <list>
#include <stdexcept>
#include <string>

// constructor does initialization, and also sets up modeTarget, targetType, and targetChannel.
ModeHandler::ModeHandler(ConnectionManager &context, ClientUser &sender,
                            CommandMessage &message) : context(context), sender(sender), message(message)
{
	modeTarget = message.getCommandArgument("modeTarget");

	handlers['i'][TARGET_USER]["+"] = &ModeHandler::invisibleUserEnableHandler;
	handlers['i'][TARGET_USER]["-"] = &ModeHandler::invisibleUserDisableHandler;

	handlers['i'][TARGET_CHANNEL]["+"] = &ModeHandler::inviteChannelEnableHandler;
	handlers['i'][TARGET_CHANNEL]["-"] = &ModeHandler::inviteChannelDisableHandler;
	handlers['t'][TARGET_CHANNEL]["+"] = &ModeHandler::topicChannelEnableHandler;
	handlers['t'][TARGET_CHANNEL]["-"] = &ModeHandler::topicChannelDisableHandler;
	handlers['k'][TARGET_CHANNEL]["+"] = &ModeHandler::keyChannelEnableHandler;
	handlers['k'][TARGET_CHANNEL]["-"] = &ModeHandler::keyChannelDisableHandler;
	handlers['o'][TARGET_CHANNEL]["+"] = &ModeHandler::operatorChannelEnableHandler;
	handlers['o'][TARGET_CHANNEL]["-"] = &ModeHandler::operatorChannelDisableHandler;
	handlers['l'][TARGET_CHANNEL]["+"] = &ModeHandler::limitChannelUserEnableHandler;
	handlers['l'][TARGET_CHANNEL]["-"] = &ModeHandler::limitChannelUserDisableHandler;

	if (determineTargetType())
	{
		return ;
	}
	if (parseModeString())
	{
		return;
	}
	extractArguments();
	if (!targetChannel->confirmInChannelByUser(sender))
	{
		sender.userBroadcast(rpl_msg::errNotOnChannel(sender,
		                   modeTarget));
		return;
	}
	if (!targetChannel->checkChannelOperatorByUser(sender))
	{
		sender.userBroadcast(rpl_msg::errChanOprivsNeeded(sender,
		                   modeTarget));
		return;
	}
	executeModeChanges();
}

ModeHandler::~ModeHandler() {}

std::string ModeHandler::currentArguments() // Get current mode arguments
{
	std::string current_argument;
	if (modeArguments.size() == 0)
	{
		throw std::out_of_range("No more modeArguments");
	}
	current_argument = modeArguments.front();
	modeArguments.pop_front();
	return current_argument;
}

bool ModeHandler::determineTargetType() // Determine the target type (user or channel)
{

	if (confirmChannel(modeTarget))
	{
		targetType = TARGET_CHANNEL;
		if (!context.checkChannelExist(modeTarget))
		{
			sender.userBroadcast(rpl_msg::errNoSuchChannel(sender, modeTarget));
			return 1;
		}
		targetChannel = &context.getChannel(modeTarget);
	}
	else if (context.checkUserNicknameExist(modeTarget))
	{
		return 1;
	}
	else
	{
		sender.userBroadcast(rpl_msg::errNoSuchChannel(sender, modeTarget));
		return 1;
	}
	return 0;
}

bool ModeHandler::containsInvalidModeString(std::string modes) // Check for invalid mode strings
{
	for (unsigned int i = 0; i < modes.size(); i++)
	{
		if (!handlers.count(modes[i]))
		{
			return (true);
		}
	}
	return (false);
}

bool ModeHandler::containsInvalidModeChar(char modeChar) // Check for invalid mode characters
{
	if (!handlers.count(modeChar))
	{
		return (true);
	}
	return (false);
}

std::string ModeHandler::sanitizeModeString(std::string rawModeString) // Sanitize mode string
{
	std::string cleaned;

	for (std::string::iterator it = rawModeString.begin();
	        it != rawModeString.end();
	        it++)
	{
		if (containsInvalidModeChar(*it))
		{
			sender.userBroadcast(rpl_msg::errUnknownMode(sender, modeTarget, *it));
		}
		else
		{
			cleaned += *it;
		}
	}
	return (cleaned);
}

bool ModeHandler::parseModeString() // Parse the mode string
{
	std::string cleaned_added_modes;
	std::string cleaned_removed_modes;

	if (!message.checkCommandArgument("rawModeString"))
	{
		if (targetType == TARGET_USER)
		{
			sender.userBroadcast(rpl_msg::umodeIs(message, *targetUser));
			return 1;
		}
		sender.userBroadcast(rpl_msg::channelModeIs(message, *targetChannel));
		return 1;
	}
	ModeParser parser(message.getCommandArgument("rawModeString"));
	try
	{
		parser.parse();
		addedModeFlags = sanitizeModeString(parser.getAddedModeFlags());
		removedModeFlags = sanitizeModeString(parser.getRemovedModeFlags());
	}
	catch (ModeParser::InvalidModestringException &e)
	{
		sender.userBroadcast(rpl_msg::errInvalidModeString());
		return 1;
	}
	return 0;
}

void ModeHandler::extractArguments() // Extract mode arguments from the message
{
	if (message.checkCommandArgumentList("modeChar modeArguments"))
	{
		modeArguments = message.getCommandArgumentList("modeChar modeArguments");
		return;
	}
}

void ModeHandler::executeModeChanges() // Execute mode changes
{
	std::string::iterator it = addedModeFlags.begin();
	for (; it != addedModeFlags.end(); it++)
	{
		((this)->*(handlers[ *it ][targetType]["+"]))();
	}
	std::string::iterator itr = removedModeFlags.begin();
	for (; itr != removedModeFlags.end(); itr++)
	{
		((this)->*(handlers[ *itr ][targetType]["-"]))();
	}
}

void ModeHandler::invisibleUserEnableHandler() // Enable invisible user mode
{
	// Compatibility with irssi
	return;
}

void ModeHandler::invisibleUserDisableHandler() // Disable invisible user mode
{
	// Compatibility with irssi
	return;
}

void ModeHandler::inviteChannelEnableHandler() // Enable invite-only mode for the channel
{
	if (!targetChannel->checkInviteToChannelOnly())
	{
		targetChannel->setInviteOnly(true);
		targetChannel->addModeFlags("i");
		targetChannel->clearInvites();
		targetChannel->broadcast(rpl_msg::modeChannel(sender, *targetChannel,
		                            "+i"));
	}
	return;
}

void ModeHandler::inviteChannelDisableHandler() // Disable invite-only mode for the channel
{
	if (targetChannel->checkInviteToChannelOnly())
	{
		targetChannel->setInviteOnly(false);
		targetChannel->removeModeFlags("i");
		targetChannel->broadcast(rpl_msg::modeChannel(sender, *targetChannel,
		                            "-i"));
	}
	return;
}

void ModeHandler::topicChannelEnableHandler() // Enable topic restriction for the channel
{
	if (!targetChannel->checkTopicRestricted())
	{
		targetChannel->setTopicLock(true);
		targetChannel->addModeFlags("t");
		targetChannel->broadcast(rpl_msg::modeChannel(sender, *targetChannel,
		                            "+t"));
	}
	return;
}

void ModeHandler::topicChannelDisableHandler() // Disable topic restriction for the channel
{
	if (targetChannel->checkTopicRestricted())
	{
		targetChannel->setTopicLock(false);
		targetChannel->removeModeFlags("t");
		targetChannel->broadcast(rpl_msg::modeChannel(sender, *targetChannel,
		                            "-t"));
	}
	return;
}

void ModeHandler::keyChannelEnableHandler() // Enable key mode for the channel
{
	std::string argument;

	try
	{
		argument = currentArguments();
		if (argument.find(' ') != std::string::npos)
		{
			throw std::out_of_range("Bad key");
		}
	}
	catch (std::out_of_range &e)
	{
		sender.userBroadcast(rpl_msg::invalidModeParam(sender, targetChannel->getChannelName(),
		                   'k',
		                   "Invalid channel key")) ;
		return;
	}
	targetChannel->setPassword(argument);
	targetChannel->addModeFlags("k");
	targetChannel->broadcast(rpl_msg::modeChannel(sender, *targetChannel,
	                            "+k " + argument));
	return;
}

void ModeHandler::keyChannelDisableHandler() // Disable key mode for the channel
{
	if (targetChannel->checkPasswordProtection())
	{
		targetChannel->removePassword();
		targetChannel->removeModeFlags("k");
		targetChannel->broadcast(rpl_msg::modeChannel(sender, *targetChannel,
		                            "-k"));
	}
	return;
}

void ModeHandler::operatorChannelEnableHandler() // Enable channel operator mode
{
	std::string argument;
	try
	{
		argument = currentArguments();
		ClientUser &new_operator = context.getUserByNickname(
		                          argument);
		if (!targetChannel->confirmInChannelByUser(new_operator))
		{
			sender.userBroadcast(rpl_msg::errUserNotInChannel(sender,
			                   new_operator.getNickname(), targetChannel->getChannelName()));
			return;
		}
		if (targetChannel->checkChannelOperatorByUser(new_operator))
		{
			return;
		}
		targetChannel->promoteOperatorByUser(new_operator);
		targetChannel->broadcast(rpl_msg::modeChannel(sender, *targetChannel,
		                            "+o " + argument));
	}
	catch (std::out_of_range &e)
	{
		return;
	}
	catch (ConnectionManager::UserNotFoundException &e)
	{
		return;
	}
}

void ModeHandler::operatorChannelDisableHandler() // Disable channel operator mode
{
	std::string argument;
	try
	{
		argument = currentArguments();
		ClientUser &new_operator = context.getUserByNickname(argument);
		if (!targetChannel->checkChannelOperatorByUser(new_operator))
		{
			return;
		}
		targetChannel->demoteOperatorByUser(new_operator);
		targetChannel->broadcast(rpl_msg::modeChannel(sender, *targetChannel,
		                            "-o " + argument));
	}
	catch (std::out_of_range &e)
	{
		return;
	}
	catch (ConnectionManager::UserNotFoundException &e)
	{
		return;
	}
	return;
}

bool isInt(const std::string& str) // Check if a string is an integer
{
	int n;
	std::istringstream istreamObject(str);
	istreamObject >> std::noskipws >> n;
	return istreamObject.eof() && !istreamObject.fail();
}

void ModeHandler::limitChannelUserEnableHandler() // Enable user limit mode for the channel
{
	std::string argument;
	int limit;
	try
	{
		argument = currentArguments();
		if (isInt(argument))
		{
			limit =  std::atoi(argument.c_str());
			if (limit < 0)
			{
				return;
			}
			targetChannel->setUserLimit(limit);
			targetChannel->addModeFlags("l");
			targetChannel->broadcast(rpl_msg::modeChannel(sender, *targetChannel,
			                            "+l " + argument));
		}
	}
	catch (std::out_of_range &e)
	{
		return;
	}
	return;
}

void ModeHandler::limitChannelUserDisableHandler() // Disable user limit mode for the channel
{
	if (targetChannel->checkUserRestriction())
	{
		targetChannel->removeUserRestriction();
		targetChannel->removeModeFlags("l");
		targetChannel->broadcast(rpl_msg::modeChannel(sender, *targetChannel,
		                            "-l"));
	}
	return;
}
