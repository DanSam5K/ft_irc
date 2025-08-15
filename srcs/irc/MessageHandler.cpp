#include "MessageHandler.hpp"
#include "ModeHandler.hpp"
#include "ModeParser.hpp"
#include "ConnectionManager.hpp"
#include "Channel.hpp"
#include "RequestParser.hpp"
#include "PasswordManager.hpp"
#include "utils_logger.hpp"
#include "reply_message.hpp"
#include <cctype>
#include <exception>
#include <list>
#include <stdexcept>
#include <string>

MessageHandler::MessageHandler(ConnectionManager &context) : context(context)
{
	configureMessageHandlers();
}

MessageHandler::~MessageHandler() {}


// Handles a raw IRC command from a client, parses it, validates it, and dispatches to the correct handler.
// Ensures that only eligible and valid commands are processed.
void MessageHandler::processClientCommand(ClientUser &sender, const std::string &rawMessage)
{
    CommandMessage *message = NULL;

    try
    {
        // Parse the raw input into a CommandMessage object
        message = buildCommandMessage(sender, rawMessage);

        // Validate the command structure and required arguments
        checkMessageValidity(sender, *message);

        // Only process commands if the user is eligible (registered, etc.)
        if (checkMessageEligibility(sender, *message))
        {
            handler function = handle[message->getCommandMessage()];
            (this->*function)(*message); // Dispatch to the appropriate handler
        }
        else
        {
            // Inform the user they are not registered for this command
            sender.userBroadcast(rpl_msg::errNotRegistered(sender));
        }
    }
    catch (std::exception &e)
    {
        // Log any errors encountered during command handling
        logActionUtils::warn("CommandMessage Handler: error: ", e.what());
        logActionUtils::warn("--> Raw command: ", rawMessage);
    }
    // Clean up the CommandMessage object
    if (message != NULL)
        delete message;
}

// void MessageHandler::processClientCommand(ClientUser &sender, const std::string &rawMessage)
// {
// 	CommandMessage *message = NULL;

// 	try
// 	{
// 		message = buildCommandMessage(sender, rawMessage);
// 		checkMessageValidity(sender, *message);
// 		if (checkMessageEligibility(sender, *message))
// 		{
// 			handler function = handle[message->getCommandMessage()];
// 			(*this.*function)(*message);
// 		}
// 		else
// 		{
// 			sender.userBroadcast(rpl_msg::errNotRegistered(sender));
// 		}
// 		delete (message);
// 	}
// 	catch (std::exception &e)
// 	{
// 		logActionUtils::warn("CommandMessage Handler: CommandMessage creation error: ", e.what());
// 		logActionUtils::warn("--> CommandMessage was", rawMessage);
// 		if (message != NULL)
// 		{
// 			delete (message);
// 		}
// 		return ;
// 	}
// }

CommandMessage *MessageHandler::buildCommandMessage(ClientUser &sender,
        std::string rawMessage)
{
	CommandMessage *message = NULL;
	try
	{
		message = new CommandMessage(sender, rawMessage);
	}
	catch (RequestParser::InvalidCommandException &e)
	{
		sender.userBroadcast(rpl_msg::errUnknownCommand(sender, ""));
		throw std::runtime_error(e.what());
	}
	catch (std::exception &e)
	{
		throw std::runtime_error(e.what());
	}
	return (message);
}

void MessageHandler::checkMessageValidity(ClientUser &sender, CommandMessage &message)
{
	try
	{
		message.processInput();
	}
	catch (RequestParser::TooManyArgumentsException &e)
	{
		sender.userBroadcast(rpl_msg::errTooManyParams(sender, message.getCommandMessage()));
		throw std::runtime_error(e.what());
	}
	catch (RequestParser::MissingArgumentsException &e)
	{
		sender.userBroadcast(rpl_msg::errNeedMoreParams(sender, message.getCommandMessage()));
		throw std::runtime_error(e.what());
	}
	catch (RequestParser::InvalidCommandException &e)
	{
		sender.userBroadcast(rpl_msg::errUnknownCommand(sender, message.getCommandMessage()));
		throw std::runtime_error(e.what());
	}
	catch (std::exception &e)
	{
		throw std::runtime_error(e.what());
	}
}


// Determines if the user is allowed to execute the given command based on registration and password status.
bool MessageHandler::checkMessageEligibility(ClientUser &sender, CommandMessage &message)
{
    std::string command = message.getCommandMessage();

    // Fully registered users can execute any command
    if (sender.confirmFullyRegistered())
        return true;

    // Users who have passed password check can execute registration-related commands
    if (sender.passwordEnabled() &&
        (command == "USER" || command == "NICK" || command == "CAP" ||
         command == "PASS" || command == "QUIT"))
        return true;

    // Users who have not passed password can only use CAP, PASS, or QUIT
    if (!sender.passwordEnabled() &&
        (command == "CAP" || command == "PASS" || command == "QUIT"))
        return true;

    // All other cases are not eligible
    return false;
}


// bool MessageHandler::checkMessageEligibility(ClientUser &sender, CommandMessage &message)
// {
// 	std::string command = message.getCommandMessage();
// 	if (sender.confirmFullyRegistered() == true)
// 	{
// 		return (true);
// 	}
// 	else if (sender.passwordEnabled() == true
// 	          && (command == "USER" || command == "NICK" || command == "CAP"
// 	               || command == "PASS" || command == "QUIT"))
// 	{
// 		return (true);
// 	}
// 	else if (sender.passwordEnabled() == false
// 	          && (command == "CAP" || command == "PASS" || command == "QUIT"))
// 	{
// 		return (true);
// 	}
// 	return (false);
// }


// Registers all supported IRC command handlers by mapping command names to their handler functions.
void MessageHandler::configureMessageHandlers()
{
	handle.insert(handlerPair("ADMIN", &MessageHandler::adminCommandHandler));
	handle.insert(handlerPair("CAP", &MessageHandler::capRequestHandler));
	handle.insert(handlerPair("INFO", &MessageHandler::infoCommandHandler));
	handle.insert(handlerPair("JOIN", &MessageHandler::joinCommandHandler));
	handle.insert(handlerPair("KICK", &MessageHandler::kickCommandHandler));
	handle.insert(handlerPair("LIST", &MessageHandler::listCommandHandler));
	handle.insert(handlerPair("MODE", &MessageHandler::modeCommandHandler));
	handle.insert(handlerPair("NAMES", &MessageHandler::namesCommandHandler));
	handle.insert(handlerPair("NICK", &MessageHandler::nickChangeHandler));
	handle.insert(handlerPair("PART", &MessageHandler::partCommandHandler));
	handle.insert(handlerPair("PASS", &MessageHandler::passCommandHandler));
	handle.insert(handlerPair("PRIVMSG", &MessageHandler::privateMessageHandler));
	handle.insert(handlerPair("QUIT", &MessageHandler::quitCommandHandler));
	handle.insert(handlerPair("SUMMON", &MessageHandler::summonCommandHandler));
	handle.insert(handlerPair("USER", &MessageHandler::userCommandHandler));
	handle.insert(handlerPair("USERS", &MessageHandler::usersCommandHandler));
	handle.insert(handlerPair("VERSION", &MessageHandler::versionRequestHandler));
	handle.insert(handlerPair("INVITE", &MessageHandler::inviteCommandHandler));
	handle.insert(handlerPair("TOPIC", &MessageHandler::topicCommandHandler));
	handle.insert(handlerPair("PING", &MessageHandler::pingCommandHandler));
	handle.insert(handlerPair("PONG", &MessageHandler::pongCommandHandler));
}

void MessageHandler::adminCommandHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();
	if (message.checkCommandArgument("modeTarget") && message.getCommandArgument("modeTarget") != SERVER_NAME)
	{
		sender.userBroadcast(rpl_msg::errNoSuchServer(sender, message.getCommandArgument("modeTarget")));
		return ;
	}
	sender.userBroadcast(rpl_msg::adminMe(sender));
	sender.userBroadcast(rpl_msg::adminLoc1(sender));
	sender.userBroadcast(rpl_msg::adminLoc2(sender));
	sender.userBroadcast(rpl_msg::adminEmail(sender));
}

void MessageHandler::inviteCommandHandler(CommandMessage &message)
{
	// TODO: no problem if no such channel
	ClientUser &sender = message.getMessageSender();
	std::string channelName = message.getCommandArgument("channel");
	std::string user_nickname = message.getCommandArgument("nickname");

	if (!context.checkUserNicknameExist(user_nickname))
	{
		sender.userBroadcast(rpl_msg::errNoSuchNick(sender, user_nickname));
		return;
	}
	ClientUser &user = context.getUserByNickname(user_nickname);
	if (!context.checkChannelExist(channelName))
	{
		sender.userBroadcast(rpl_msg::inviting(user, message));
		user.userBroadcast(rpl_msg::invite(sender, message));
		return;
	}
	Channel &channel = context.getChannel(channelName);
	if (!channel.confirmInChannelByUser(sender))
	{
		sender.userBroadcast(rpl_msg::errNotOnChannel(sender, channel.getChannelName()));
		return;
	}
	if (channel.checkInviteToChannelOnly() && !channel.checkChannelOperatorByUser(sender))
	{
		sender.userBroadcast(rpl_msg::errChanOprivsNeeded(sender, channelName));
		return;
	}
	if (channel.confirmInChannelByNickname(user_nickname))
	{
		sender.userBroadcast(rpl_msg::errUserOnChannel(sender, user_nickname,
		                   channel.getChannelName()));
		return;
	}
	sender.userBroadcast(rpl_msg::inviting(user, message));
	user.userBroadcast(rpl_msg::invite(sender, message));
	channel.inviteUser(user_nickname);
}

void MessageHandler::topicCommandHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();
	std::string channelName = message.getCommandArgument("channel");

	if (!context.checkChannelExist(channelName))
	{
		sender.userBroadcast(rpl_msg::errNotOnChannel(sender, channelName));
		return;
	}
	Channel &channel = context.getChannel(channelName);
	if (!channel.confirmInChannelByUser(sender))
	{
		sender.userBroadcast(rpl_msg::errNotOnChannel(sender, channel.getChannelName()));
		return;
	}
	if (!message.checkCommandArgument("topic"))
	{
		if (channel.getTopicMessage() == "")
		{
			sender.userBroadcast(rpl_msg::noTopic(message, channel));
			return;
		}
		else
		{
			sender.userBroadcast(rpl_msg::topic(message, channel));
			return;
		}
	}
	std::string new_topic = message.getCommandArgument("topic");
	if (channel.checkTopicRestricted() && !channel.checkChannelOperatorByUser(sender))
	{
		sender.userBroadcast(rpl_msg::errChanOprivsNeeded(sender, channel.getChannelName()));
		return;
	}
	channel.setTopicMessage(new_topic);
	channel.broadcast(rpl_msg::newTopic(sender, message));
}

void MessageHandler::capRequestHandler(CommandMessage &message)
{
	(void)message;
	return ;
}

void MessageHandler::infoCommandHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();
	if (message.checkCommandArgument("modeTarget") && message.getCommandArgument("modeTarget") != SERVER_NAME)
	{
		sender.userBroadcast(rpl_msg::errNoSuchServer(sender, message.getCommandArgument("modeTarget")));
		return ;
	}
	sender.userBroadcast(rpl_msg::info(sender, 0));
	sender.userBroadcast(rpl_msg::info(sender, 1));
	sender.userBroadcast(rpl_msg::info(sender, 2));
	sender.userBroadcast(rpl_msg::infoEnd(sender));
}

void MessageHandler::joinCommandHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();
	std::list<std::string> chan_names = message.getCommandArgumentList("channel");
	if (chan_names.empty())
	{
		throw std::runtime_error("JOIN command failed: No channel specified.");
	}
	if (chan_names.front() == "0")
	{
		std::list<Channel *> chan_list = context.getUserChannels(sender);
		std::list<Channel *>::iterator it = chan_list.begin();
		for (; it != chan_list.end(); it++)
		{
			if ((*it)->getChannelName() != FALLBACK_CHANNEL)
			{
				context.processClientCommand(sender, rpl_msg::createPartMessage(*(*it)));
			}
		}
		return ;
	}
	std::list<std::string> keys;
	for (size_t i = 0; i < chan_names.size(); i++)
	{
		keys.push_back("");
	}
	if (message.checkCommandArgumentList("key"))
	{
		keys = message.getCommandArgumentList("key");
	}
	std::list<std::string>::iterator chans = chan_names.begin();
	std::list<std::string>::iterator passes = keys.begin();
	for (; chans != chan_names.end(); chans++, passes++)
	{
		try
		{
			if (!context.checkChannelExist(*chans))
			{
				context.joinUserToChannel(sender, *chans);
				Channel &channel = context.getChannel(*chans);
				channel.broadcast(rpl_msg::joinChannel(sender, channel));
				sender.userBroadcast(rpl_msg::nameReply(sender, channel));
				sender.userBroadcast(rpl_msg::endOfNames(sender, channel.getChannelName()));
			}
			else
			{
				Channel &channel = context.getChannel(*chans);
				if (channel.checkInviteToChannelOnly() && !channel.checkInvitedByUser(sender))
				{
					sender.userBroadcast(rpl_msg::errInviteOnlyChannel(sender, channel.getChannelName()));
				}
				else if (channel.checkPasswordProtection()
				          && !channel.checkPassword(*passes))
				{
					sender.userBroadcast(rpl_msg::errBadChannelKey(sender, channel.getChannelName()));
				}
				else if (channel.checkRestrictionPoint())
				{
					sender.userBroadcast(rpl_msg::errChanneListFull(sender, channel.getChannelName()));
				}
				else
				{
					context.joinUserToChannel(sender, *chans);
					channel.revokeInvite(sender.getNickname());
					channel.broadcast(rpl_msg::joinChannel(sender, channel));
					sender.userBroadcast(rpl_msg::topic(message, channel));
					sender.userBroadcast(rpl_msg::nameReply(sender, channel));
					sender.userBroadcast(rpl_msg::endOfNames(sender, channel.getChannelName()));
				}
			}
		}
		catch (Channel::AlreadyInChannelException &e)
		{
			logActionUtils::warn("JOIN: ClientUser " + sender.getNickname() + " is already a member of channel " + *chans);
		}
		catch (std::exception &e)
		{
			logActionUtils::warn("CommandMessage Handler: JOIN: ", e.what());
			sender.userBroadcast(rpl_msg::errNoSuchChannel(sender, *chans));
		}
	}
}

void MessageHandler::kickCommandHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();
	std::list<std::string> channels = message.getCommandArgumentList("channel");
	std::list<std::string> users = message.getCommandArgumentList("user");
	if (channels.size() > 1 && channels.size() != users.size())
	{
		// TODO: maybe message.getCommandMessage() not correct here
		sender.userBroadcast(rpl_msg::errNeedMoreParams(sender, message.getCommandMessage()));
		return;
	}

	if (channels.size() == 1)
	{
		std::string channelName = channels.front();
		if (!context.checkChannelExist(channelName))
		{
			sender.userBroadcast(rpl_msg::errNoSuchChannel(sender, channelName));
			return;
		}
		Channel &channel = context.getChannel(channelName);
		if (!channel.confirmInChannelByUser(sender))
		{
			sender.userBroadcast(rpl_msg::errNotOnChannel(sender, channel.getChannelName()));
			return;
		}
		if (!channel.checkChannelOperatorByUser(sender))
		{
			sender.userBroadcast(rpl_msg::errChanOprivsNeeded(sender, channelName));
			return;
		}
		std::list<std::string>::iterator it = users.begin();
		for (; it != users.end(); it++)
		{
			if (!context.checkUserNicknameExist(*it))
			{
				sender.userBroadcast(rpl_msg::errUserNotInChannel(sender, *it,
				                   channel.getChannelName()));
				return;
			}
			ClientUser &user = context.getUserByNickname(*it);
			if (!channel.confirmInChannelByUser(user))
			{

				sender.userBroadcast(rpl_msg::errUserNotInChannel(sender, *it,
				                   channel.getChannelName()));
				return;
			}
			if (message.getCommandArgument("comment") == "")
			{
				channel.broadcast(rpl_msg::kick(sender, user, channel, sender.getNickname()));
			}
			else
			{
				channel.broadcast(rpl_msg::kick(sender, user, channel,
				                               message.getCommandArgument("comment")));
			}
			context.removeUserFromChannel(user, channel.getChannelName());
		}
	}
	else
	{
		std::list<std::string>::iterator uit = users.begin();
		std::list<std::string>::iterator cit = channels.begin();
		for (; uit != users.end(); uit++, cit++)
		{
			std::string channelName = *cit;
			std::string user_name = *uit;
			if (!context.checkChannelExist(channelName))
			{
				sender.userBroadcast(rpl_msg::errNoSuchChannel(sender, channelName));
				return;
			}
			Channel &channel = context.getChannel(channelName);
			if (!channel.confirmInChannelByUser(sender))
			{
				sender.userBroadcast(rpl_msg::errNotOnChannel(sender, channel.getChannelName()));
				return;
			}
			if (!channel.checkChannelOperatorByUser(sender))
			{
				sender.userBroadcast(rpl_msg::errChanOprivsNeeded(sender, channelName));
				return;
			}
			if (!context.checkUserNicknameExist(user_name))
			{
				sender.userBroadcast(rpl_msg::errUserNotInChannel(sender, user_name,
				                   channel.getChannelName()));
				return;
			}
			ClientUser &user = context.getUserByNickname(user_name);
			if (!channel.confirmInChannelByUser(user))
			{

				sender.userBroadcast(rpl_msg::errUserNotInChannel(sender, user_name,
				                   channel.getChannelName()));
				return;
			}
			if (message.getCommandArgument("comment") == "")
			{
				channel.broadcast(rpl_msg::kick(sender, user, channel, sender.getNickname()));
			}
			else
			{
				channel.broadcast(rpl_msg::kick(sender, user, channel,
				                               message.getCommandArgument("comment")));
			}
			context.removeUserFromChannel(user, channel.getChannelName());
		}

	}
}

void MessageHandler::listCommandHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();
	std::list<std::string> channels;

	sender.userBroadcast(rpl_msg::listStart(sender));
	if (message.checkCommandArgumentList("channel"))
	{
		channels = message.getCommandArgumentList("channel");
		std::list<std::string>::iterator it = channels.begin();
		for (; it != channels.end(); it++)
		{
			sender.userBroadcast(rpl_msg::list(sender, context.getChannel(*it)));
		}
	}
	else
	{
		channels = context.listAllChannelNames();
		std::list<std::string>::iterator it = channels.begin();
		for (; it != channels.end(); it++)
		{
			sender.userBroadcast(rpl_msg::list(sender, context.getChannel(*it)));
		}
	}
	sender.userBroadcast(rpl_msg::listEnd(sender));
}

void MessageHandler::modeCommandHandler(CommandMessage &message)
{

	ClientUser &sender = message.getMessageSender();

	ModeHandler handler(context, sender, message);
	return;
}

void MessageHandler::namesCommandHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();
	std::list<std::string> chan_names;
	bool show_default_chan = false;
	if (message.checkCommandArgumentList("channel"))
	{
		chan_names = message.getCommandArgumentList("channel");
	}
	else
	{
		chan_names = context.listAllChannelNames();
		show_default_chan = true;
	}
	std::list<std::string>::iterator it = chan_names.begin();
	std::list<std::string>::iterator last = (++chan_names.rbegin()).base();
	for (; it != chan_names.end(); it++)
	{
		try
		{
			Channel &channel = context.getChannel(*it);
			sender.userBroadcast(rpl_msg::nameReply(sender, channel));
		}
		catch(std::exception &e)
		{
			logActionUtils::warn("NAMES command error: ", e.what());
		}
		if (show_default_chan == false && it == last)
		{
			sender.userBroadcast(rpl_msg::endOfNames(sender, *it));
		}
	}
	if (show_default_chan == true)
	{
		Channel &channel = context.getDefaultChannel();
		if (channel.confirmChannelIsEmpty() == false)
		{
			sender.userBroadcast(rpl_msg::nameReply(sender, channel));
		}
		sender.userBroadcast(rpl_msg::endOfNames(sender, channel.getChannelName()));
	}
}

void MessageHandler::nickChangeHandler(CommandMessage &message)
{
    ClientUser &sender = message.getMessageSender();
    std::string nickname = message.getCommandArgument("nickname");
    if (context.checkUserNicknameExist(nickname) == true)
    {
        sender.userBroadcast(rpl_msg::errNicknameInUse(sender, nickname));
        return ;
    }
    try
    {
        bool user_is_already_registered = sender.confirmFullyRegistered();
        std::string old_id = sender.getIdentifier();
        sender.setNickname(nickname);

        // Only send confirmation if user was already registered
        if (user_is_already_registered)
        {
            sender.userBroadcast(rpl_msg::confirmation(old_id, message));
        }
        // Note: Registration will happen in USER command when all details are set
    }
    catch (ClientUser::InvalidNicknameException &e)
    {
        sender.userBroadcast(rpl_msg::errErroneousNickname(sender, nickname));
    }
    catch (ClientUser::NicknameTooLongException &e)
    {
        sender.userBroadcast(rpl_msg::errNicknameTooLong(sender, nickname));
    }
}

// void MessageHandler::nickChangeHandler(CommandMessage &message)
// {
// 	ClientUser &sender = message.getMessageSender();
// 	std::string nickname = message.getCommandArgument("nickname");
// 	if (context.checkUserNicknameExist(nickname) == true)
// 	{
// 		sender.userBroadcast(rpl_msg::errNicknameInUse(sender, nickname));
// 		return ;
// 	}
// 	try
// 	{
// 		bool user_is_already_registered = sender.confirmFullyRegistered();
// 		std::string old_id = sender.getIdentifier();
// 		sender.setNickname(nickname);
// 		if (user_is_already_registered)
// 		{
// 			sender.userBroadcast(rpl_msg::confirmation(old_id, message));
// 		}
// 		else
// 		{
// 			greetNewUser(sender);
// 		}
// 	}
// 	catch (ClientUser::InvalidNicknameException &e)
// 	{
// 		sender.userBroadcast(rpl_msg::errErroneousNickname(sender, nickname));
// 	}
// 	catch (ClientUser::NicknameTooLongException &e)
// 	{
// 		sender.userBroadcast(rpl_msg::errNicknameTooLong(sender, nickname));
// 	}
// }

void MessageHandler::partCommandHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();
	std::list<std::string> chan_names = message.getCommandArgumentList("channel");
	std::string part_msg = sender.getNickname();
	if (message.checkCommandArgument("Part CommandMessage"))
	{
		part_msg = message.getCommandArgument("Part CommandMessage");
	}
	std::list<std::string>::iterator it = chan_names.begin();
	for (; it != chan_names.end(); it++)
	{
		try
		{
			Channel &channel = context.getChannel(*it);
			if (channel.confirmInChannelByUser(sender) == false)
			{
				sender.userBroadcast(rpl_msg::errNotOnChannel(sender, channel.getChannelName()));
				continue ;
			}
			context.removeUserFromChannel(sender, *it);
			channel.broadcast(rpl_msg::part(sender, channel, message));
			sender.userBroadcast(rpl_msg::part(sender, channel, message));
			if (channel.confirmChannelIsEmpty() && channel.getChannelName() != FALLBACK_CHANNEL)
			{
				context.deleteChannel(channel);
			}
		}
		catch(std::exception &e)
		{
			sender.userBroadcast(rpl_msg::errNoSuchChannel(sender, *it));
		}
	}
}

void MessageHandler::passCommandHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();

	if (sender.confirmFullyRegistered())
	{
		sender.userBroadcast(rpl_msg::errAlreadyRegistered(sender));
		return ;
	}
	if (!message.checkCommandArgument("password"))
	{
		sender.userBroadcast(rpl_msg::errPasswdMismatch(sender));
		return;
	}
	try
	{
		context.verifyConnectionPassword(message.getCommandArgument("password"));
		sender.authorizePassword();
	}
	catch (PasswordManager::InvalidPasswordException &e)
	{
		logActionUtils::warn("MessageHandler: PASS: ", e.what());
		sender.userBroadcast(rpl_msg::errPasswdMismatch(sender));
	}
	catch (std::exception &e)
	{
		logActionUtils::info("MessageHandler: PASS: ", e.what());
	}
}

void MessageHandler::privateMessageHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();
	std::string toNickname = message.getCommandArgument("msgtarget");
	std::string text = "";
	if (message.checkCommandArgument("text to be sent"))
	{
		text = message.getCommandArgument("text to be sent");
	}
	else
	{
		sender.userBroadcast(rpl_msg::errNoTextToSend(sender));
		return;
	}
	if (context.checkUserNicknameExist(toNickname) == true)
	{
		ClientUser &toUser = context.getUserByNickname(toNickname);
		toUser.userBroadcast(rpl_msg::forward(sender, message));
	}
	else if (context.checkChannelExist(toNickname) == true)
	{
		Channel &toChannel = context.getChannel(toNickname);
		if (toChannel.confirmInChannelByUser(sender))
		{
			toChannel.broadcastExcept(rpl_msg::forward(sender, message), sender);
		}
		else
		{
			sender.userBroadcast(rpl_msg::errCannotSendToChan(sender, toChannel.getChannelName()));
		}
	}
	else
	{
		sender.userBroadcast(rpl_msg::errNoSuchNick(sender, toNickname));
	}
}

void MessageHandler::quitCommandHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();
	if (context.checkUserInAnyChannel(sender) == true)
	{
		std::list<ClientUser *> users_in_same_channels = context.getMutualChannelUsers(
		            sender);
		std::list<ClientUser *>::iterator it = users_in_same_channels.begin();
		for (; it != users_in_same_channels.end(); it++)
		{

			(*it)->userBroadcast(rpl_msg::quit(sender, message));
		}
	}
	context.forciblyDisconnect(sender);
}


void MessageHandler::userCommandHandler(CommandMessage &message)
{
    ClientUser &sender = message.getMessageSender();
    if (sender.confirmFullyRegistered())
    {
        sender.userBroadcast(rpl_msg::errAlreadyRegistered(sender));
        return ;
    }
    // Note: checkAllUserDetails() will return false for new users, which is expected
    // We only need to check if they're already registered
    try
    {
        // Set username and realname from message arguments
        // Note: hostname should be set from connection info, not from USER command
        std::string username = message.getCommandArgument("user");
        std::string realname = message.getCommandArgument("realname");
        sender.setUsername(username);
        sender.setRealname(realname);
        // Hostname will be set from connection information, not from USER command

        // Check if user has set nickname and passed password
        if (sender.passwordEnabled() && sender.getNickname() != "*" && sender.getUsername() != "*" && sender.getRealname() != "*")
        {
            // User is ready for registration - promote to active and send welcome messages
            context.promoteUserToActive(sender);
            greetNewUser(sender); // Send welcome messages
        }
    }
    catch (ClientUser::InvalidUsernameException &e)
    {
        sender.userBroadcast(rpl_msg::errNeedMoreParams(sender, "USER"));
    }
    catch (std::exception &e)
    {
        logActionUtils::warn("USER command error: ", e.what());
    }
}

void MessageHandler::summonCommandHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();
	sender.userBroadcast(rpl_msg::errSummonDisabled(sender));
}

// void MessageHandler::userCommandHandler(CommandMessage &message)
// {

// 	ClientUser &sender = message.getMessageSender();
// 	if (sender.confirmFullyRegistered())
// 	{
// 		sender.userBroadcast(rpl_msg::errAlreadyRegistered(sender));
// 		return ;
// 	}
// 	else if (sender.checkAllUserDetails())
// 	{
// 		sender.userBroadcast(rpl_msg::errNotRegistered(sender));
// 		return;
// 	}
// 	try
// 	{
// 		sender.setUsername(message.getCommandArgument("user"));
// 		sender.setHostname(message.getCommandArgument("unused"));
// 		sender.setRealname(message.getCommandArgument("realname"));
// 		greetNewUser(sender);
// 	}
// 	catch (ClientUser::InvalidUsernameException &e)
// 	{
// 		sender.userBroadcast(rpl_msg::errInvalidUsername());
// 	}
// 	catch (std::exception &e)
// 	{
// 		logActionUtils::warn("MessageHandler: USER:", e.what());
// 	}
// }

void MessageHandler::usersCommandHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();
	sender.userBroadcast(rpl_msg::errUsersDisabled(sender));
}

void MessageHandler::versionRequestHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();
	sender.userBroadcast(rpl_msg::serverVersion(sender));
}

void MessageHandler::greetNewUser(ClientUser &user)
{
	if (user.confirmNicknameExist() == false || user.checkAllUserDetails() == false)
	{
		return ;
	}
	try
	{
		// User is already promoted to active status, just send welcome messages
		user.userBroadcast(rpl_msg::welcome(user));
		user.userBroadcast(rpl_msg::yourHost(user));
		user.userBroadcast(rpl_msg::created(user));
		user.userBroadcast(rpl_msg::myInfo(user));
	}
	catch (std::exception &e)
	{
		logActionUtils::warn("CommandMessage Handler: Welcome: ", e.what());
	}
}

void MessageHandler::pingCommandHandler(CommandMessage &message)
{
	ClientUser &sender = message.getMessageSender();

	try
	{
		std::string token = message.getCommandArgument("token");
		// Remove leading colon if present (IRC protocol allows PING :token or PING token)
		if (!token.empty() && token[0] == ':')
		{
			token = token.substr(1);
		}
		
		if (token.empty())
		{
			sender.userBroadcast(rpl_msg::errNeedMoreParams(sender, "PING"));
			return;
		}
		sender.userBroadcast(rpl_msg::pong(sender, message));
	}
	catch (std::exception &e)
	{
		sender.userBroadcast(rpl_msg::errNeedMoreParams(sender, "PING"));
	}
}

void MessageHandler::pongCommandHandler(CommandMessage &message)
{
	// Dummy To be implemented
	(void) message;
}
