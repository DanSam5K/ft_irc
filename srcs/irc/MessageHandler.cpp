#include "MessageHandler.hpp"
#include "ModeHandler.hpp"
#include "ModeParser.hpp"
#include "ConnectionManager.hpp"
#include "Channel.hpp"
#include "RequestParser.hpp"
#include "PasswordManager.hpp"
#include "utils_logger.hpp"
#include "reply.hpp"
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

void MessageHandler::processClientCommand(User &sender, const std::string &rawMessage)
{
	CommandMessage *message = NULL;

	try
	{
		message = buildCommandMessage(sender, rawMessage);
		checkMessageValidity(sender, *message);
		if (checkMessageEligibility(sender, *message))
		{
			handler function = handle[message->getCommandMessage()];
			(*this.*function)(*message);
		}
		else
		{
			sender.userBroadcast(rpl::err_notregistered(sender));
		}
		delete (message);
	}
	catch (std::exception &e)
	{
		logActionUtils::warn("CommandMessage Handler: CommandMessage creation error: ", e.what());
		logActionUtils::warn("--> CommandMessage was", rawMessage);
		if (message != NULL)
		{
			delete (message);
		}
		return ;
	}
}

CommandMessage *MessageHandler::buildCommandMessage(User &sender,
        std::string rawMessage)
{
	CommandMessage *message = NULL;
	try
	{
		message = new CommandMessage(sender, rawMessage);
	}
	catch (RequestParser::UnknownCommandException &e)
	{
		sender.userBroadcast(rpl::err_unknowncommand(sender, ""));
		throw std::runtime_error(e.what());
	}
	catch (std::exception &e)
	{
		throw std::runtime_error(e.what());
	}
	return (message);
}

void MessageHandler::checkMessageValidity(User &sender, CommandMessage &message)
{
	try
	{
		message.processInput();
	}
	catch (RequestParser::TooManyParamsException &e)
	{
		sender.userBroadcast(rpl::err_toomanyparams(sender, message.getCommandMessage()));
		throw std::runtime_error(e.what());
	}
	catch (RequestParser::NeedMoreParamsException &e)
	{
		sender.userBroadcast(rpl::err_needmoreparams(sender, message.getCommandMessage()));
		throw std::runtime_error(e.what());
	}
	catch (RequestParser::UnknownCommandException &e)
	{
		sender.userBroadcast(rpl::err_unknowncommand(sender, message.getCommandMessage()));
		throw std::runtime_error(e.what());
	}
	catch (std::exception &e)
	{
		throw std::runtime_error(e.what());
	}
}

bool MessageHandler::checkMessageEligibility(User &sender, CommandMessage &message)
{
	std::string command = message.getCommandMessage();
	if (sender.is_fully_registered() == true)
	{
		return (true);
	}
	else if (sender.passwordEnabled() == true
	          && (command == "USER" || command == "NICK" || command == "CAP"
	               || command == "PASS" || command == "QUIT"))
	{
		return (true);
	}
	else if (sender.passwordEnabled() == false
	          && (command == "CAP" || command == "PASS" || command == "QUIT"))
	{
		return (true);
	}
	return (false);
}

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
	User &sender = message.getMessageSender();
	if (message.checkCommandArgument("modeTarget") && message.getCommandArgument("modeTarget") != SERVER_NAME)
	{
		sender.userBroadcast(rpl::err_nosuchserver(sender, message.getCommandArgument("modeTarget")));
		return ;
	}
	sender.userBroadcast(rpl::adminme(sender));
	sender.userBroadcast(rpl::adminloc1(sender));
	sender.userBroadcast(rpl::adminloc2(sender));
	sender.userBroadcast(rpl::adminemail(sender));
}

void MessageHandler::inviteCommandHandler(CommandMessage &message)
{
	// TODO: no problem if no such channel
	User &sender = message.getMessageSender();
	std::string channel_name = message.getCommandArgument("channel");
	std::string user_nickname = message.getCommandArgument("nickname");

	if (!context.checkUserNicknameExist(user_nickname))
	{
		sender.userBroadcast(rpl::err_nosuchnick(sender, user_nickname));
		return;
	}
	User &user = context.getUserByNickname(user_nickname);
	if (!context.checkChannelExist(channel_name))
	{
		sender.userBroadcast(rpl::inviting(user, message));
		user.userBroadcast(rpl::invite(sender, message));
		return;
	}
	Channel &channel = context.getChannel(channel_name);
	if (!channel.confirmInChannelByUser(sender))
	{
		sender.userBroadcast(rpl::err_notonchannel(sender, channel.getChannelName()));
		return;
	}
	if (channel.checkInviteToChannelOnly() && !channel.checkChannelOperatorByUser(sender))
	{
		sender.userBroadcast(rpl::err_chanoprivsneeded(sender, channel_name));
		return;
	}
	if (channel.confirmInChannelByNickname(user_nickname))
	{
		sender.userBroadcast(rpl::err_useronchannel(sender, user_nickname,
		                   channel.getChannelName()));
		return;
	}
	sender.userBroadcast(rpl::inviting(user, message));
	user.userBroadcast(rpl::invite(sender, message));
	channel.inviteUser(user_nickname);
}

void MessageHandler::topicCommandHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();
	std::string channel_name = message.getCommandArgument("channel");

	if (!context.checkChannelExist(channel_name))
	{
		sender.userBroadcast(rpl::err_notonchannel(sender, channel_name));
		return;
	}
	Channel &channel = context.getChannel(channel_name);
	if (!channel.confirmInChannelByUser(sender))
	{
		sender.userBroadcast(rpl::err_notonchannel(sender, channel.getChannelName()));
		return;
	}
	if (!message.checkCommandArgument("topic"))
	{
		if (channel.getTopicMessage() == "")
		{
			sender.userBroadcast(rpl::notopic(message, channel));
			return;
		}
		else
		{
			sender.userBroadcast(rpl::topic(message, channel));
			return;
		}
	}
	std::string new_topic = message.getCommandArgument("topic");
	if (channel.checkTopicRestricted() && !channel.checkChannelOperatorByUser(sender))
	{
		sender.userBroadcast(rpl::err_chanoprivsneeded(sender, channel.getChannelName()));
		return;
	}
	channel.setTopicMessage(new_topic);
	channel.broadcast(rpl::newtopic(sender, message));
}

void MessageHandler::capRequestHandler(CommandMessage &message)
{
	(void)message;
	return ;
}

void MessageHandler::infoCommandHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();
	if (message.checkCommandArgument("modeTarget") && message.getCommandArgument("modeTarget") != SERVER_NAME)
	{
		sender.userBroadcast(rpl::err_nosuchserver(sender, message.getCommandArgument("modeTarget")));
		return ;
	}
	sender.userBroadcast(rpl::info(sender, 0));
	sender.userBroadcast(rpl::info(sender, 1));
	sender.userBroadcast(rpl::info(sender, 2));
	sender.userBroadcast(rpl::info_end(sender));
}

void MessageHandler::joinCommandHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();
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
				context.processClientCommand(sender, rpl::create_part_message(*(*it)));
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
				channel.broadcast(rpl::join_channel(sender, channel));
				sender.userBroadcast(rpl::namreply(sender, channel));
				sender.userBroadcast(rpl::endofnames(sender, channel.getChannelName()));
			}
			else
			{
				Channel &channel = context.getChannel(*chans);
				if (channel.checkInviteToChannelOnly() && !channel.checkInvitedByUser(sender))
				{
					sender.userBroadcast(rpl::err_inviteonlychan(sender, channel.getChannelName()));
				}
				else if (channel.checkPasswordProtection()
				          && !channel.checkPassword(*passes))
				{
					sender.userBroadcast(rpl::err_badchannelkey(sender, channel.getChannelName()));
				}
				else if (channel.checkRestrictionPoint())
				{
					sender.userBroadcast(rpl::err_channelisfull(sender, channel.getChannelName()));
				}
				else
				{
					context.joinUserToChannel(sender, *chans);
					channel.revokeInvite(sender.get_nickname());
					channel.broadcast(rpl::join_channel(sender, channel));
					sender.userBroadcast(rpl::topic(message, channel));
					sender.userBroadcast(rpl::namreply(sender, channel));
					sender.userBroadcast(rpl::endofnames(sender, channel.getChannelName()));
				}
			}
		}
		catch (Channel::AlreadyInChannelException &e)
		{
			logActionUtils::warn("JOIN: User " + sender.get_nickname() + " is already a member of channel " + *chans);
		}
		catch (std::exception &e)
		{
			logActionUtils::warn("CommandMessage Handler: JOIN: ", e.what());
			sender.userBroadcast(rpl::err_nosuchchannel(sender, *chans));
		}
	}
}

void MessageHandler::kickCommandHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();
	std::list<std::string> channels = message.getCommandArgumentList("channel");
	std::list<std::string> users = message.getCommandArgumentList("user");
	if (channels.size() > 1 && channels.size() != users.size())
	{
		// TODO: maybe message.getCommandMessage() not correct here
		sender.userBroadcast(rpl::err_needmoreparams(sender, message.getCommandMessage()));
		return;
	}

	if (channels.size() == 1)
	{
		std::string channel_name = channels.front();
		if (!context.checkChannelExist(channel_name))
		{
			sender.userBroadcast(rpl::err_nosuchchannel(sender, channel_name));
			return;
		}
		Channel &channel = context.getChannel(channel_name);
		if (!channel.confirmInChannelByUser(sender))
		{
			sender.userBroadcast(rpl::err_notonchannel(sender, channel.getChannelName()));
			return;
		}
		if (!channel.checkChannelOperatorByUser(sender))
		{
			sender.userBroadcast(rpl::err_chanoprivsneeded(sender, channel_name));
			return;
		}
		std::list<std::string>::iterator it = users.begin();
		for (; it != users.end(); it++)
		{
			if (!context.checkUserNicknameExist(*it))
			{
				sender.userBroadcast(rpl::err_usernotinchannel(sender, *it,
				                   channel.getChannelName()));
				return;
			}
			User &user = context.getUserByNickname(*it);
			if (!channel.confirmInChannelByUser(user))
			{

				sender.userBroadcast(rpl::err_usernotinchannel(sender, *it,
				                   channel.getChannelName()));
				return;
			}
			if (message.getCommandArgument("comment") == "")
			{
				channel.broadcast(rpl::kick(sender, user, channel, sender.get_nickname()));
			}
			else
			{
				channel.broadcast(rpl::kick(sender, user, channel,
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
			std::string channel_name = *cit;
			std::string user_name = *uit;
			if (!context.checkChannelExist(channel_name))
			{
				sender.userBroadcast(rpl::err_nosuchchannel(sender, channel_name));
				return;
			}
			Channel &channel = context.getChannel(channel_name);
			if (!channel.confirmInChannelByUser(sender))
			{
				sender.userBroadcast(rpl::err_notonchannel(sender, channel.getChannelName()));
				return;
			}
			if (!channel.checkChannelOperatorByUser(sender))
			{
				sender.userBroadcast(rpl::err_chanoprivsneeded(sender, channel_name));
				return;
			}
			if (!context.checkUserNicknameExist(user_name))
			{
				sender.userBroadcast(rpl::err_usernotinchannel(sender, user_name,
				                   channel.getChannelName()));
				return;
			}
			User &user = context.getUserByNickname(user_name);
			if (!channel.confirmInChannelByUser(user))
			{

				sender.userBroadcast(rpl::err_usernotinchannel(sender, user_name,
				                   channel.getChannelName()));
				return;
			}
			if (message.getCommandArgument("comment") == "")
			{
				channel.broadcast(rpl::kick(sender, user, channel, sender.get_nickname()));
			}
			else
			{
				channel.broadcast(rpl::kick(sender, user, channel,
				                               message.getCommandArgument("comment")));
			}
			context.removeUserFromChannel(user, channel.getChannelName());
		}

	}
}

void MessageHandler::listCommandHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();
	std::list<std::string> channels;

	sender.userBroadcast(rpl::liststart(sender));
	if (message.checkCommandArgumentList("channel"))
	{
		channels = message.getCommandArgumentList("channel");
		std::list<std::string>::iterator it = channels.begin();
		for (; it != channels.end(); it++)
		{
			sender.userBroadcast(rpl::list(sender, context.getChannel(*it)));
		}
	}
	else
	{
		channels = context.listAllChannelNames();
		std::list<std::string>::iterator it = channels.begin();
		for (; it != channels.end(); it++)
		{
			sender.userBroadcast(rpl::list(sender, context.getChannel(*it)));
		}
	}
	sender.userBroadcast(rpl::listend(sender));
}

void MessageHandler::modeCommandHandler(CommandMessage &message)
{

	User &sender = message.getMessageSender();

	ModeHandler handler(context, sender, message);
	return;
}

void MessageHandler::namesCommandHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();
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
			sender.userBroadcast(rpl::namreply(sender, channel));
		}
		catch(std::exception &e)
		{
			logActionUtils::warn("NAMES command error: ", e.what());
		}
		if (show_default_chan == false && it == last)
		{
			sender.userBroadcast(rpl::endofnames(sender, *it));
		}
	}
	if (show_default_chan == true)
	{
		Channel &channel = context.getDefaultChannel();
		if (channel.confirmChannelIsEmpty() == false)
		{
			sender.userBroadcast(rpl::namreply(sender, channel));
		}
		sender.userBroadcast(rpl::endofnames(sender, channel.getChannelName()));
	}
}

void MessageHandler::nickChangeHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();
	std::string nickname = message.getCommandArgument("nickname");
	if (context.checkUserNicknameExist(nickname) == true)
	{
		sender.userBroadcast(rpl::err_nicknameinuse(sender, nickname));
		return ;
	}
	try
	{
		bool user_is_already_registered = sender.is_fully_registered();
		std::string old_id = sender.get_identifier();
		sender.set_nickname(nickname);
		if (user_is_already_registered)
		{
			sender.userBroadcast(rpl::confirmation(old_id, message));
		}
		else
		{
			greetNewUser(sender);
		}
	}
	catch (User::InvalidNicknameException &e)
	{
		sender.userBroadcast(rpl::err_erroneusnickname(sender, nickname));
	}
	catch (User::NicknameTooLongException &e)
	{
		sender.userBroadcast(rpl::err_nicknametoolong(sender, nickname));
	}
}

void MessageHandler::partCommandHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();
	std::list<std::string> chan_names = message.getCommandArgumentList("channel");
	std::string part_msg = sender.get_nickname();
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
				sender.userBroadcast(rpl::err_notonchannel(sender, channel.getChannelName()));
				continue ;
			}
			context.removeUserFromChannel(sender, *it);
			channel.broadcast(rpl::part(sender, channel, message));
			sender.userBroadcast(rpl::part(sender, channel, message));
			if (channel.confirmChannelIsEmpty() && channel.getChannelName() != FALLBACK_CHANNEL)
			{
				context.deleteChannel(channel);
			}
		}
		catch(std::exception &e)
		{
			sender.userBroadcast(rpl::err_nosuchchannel(sender, *it));
		}
	}
}

void MessageHandler::passCommandHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();

	if (sender.is_fully_registered())
	{
		sender.userBroadcast(rpl::err_alreadyregistred(sender));
		return ;
	}
	if (!message.checkCommandArgument("password"))
	{
		sender.userBroadcast(rpl::err_passwdmismatch(sender));
		return;
	}
	try
	{
		context.verifyConnectionPassword(message.getCommandArgument("password"));
		sender.set_correct_password();
	}
	catch (PasswordManager::InvalidPasswordException &e)
	{
		logActionUtils::warn("MessageHandler: PASS: ", e.what());
		sender.userBroadcast(rpl::err_passwdmismatch(sender));
	}
	catch (std::exception &e)
	{
		logActionUtils::info("MessageHandler: PASS: ", e.what());
	}
}

void MessageHandler::privateMessageHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();
	std::string toNickname = message.getCommandArgument("msgtarget");
	std::string text = "";
	if (message.checkCommandArgument("text to be sent"))
	{
		text = message.getCommandArgument("text to be sent");
	}
	else
	{
		sender.userBroadcast(rpl::err_notexttosend(sender));
		return;
	}
	if (context.checkUserNicknameExist(toNickname) == true)
	{
		User &toUser = context.getUserByNickname(toNickname);
		toUser.userBroadcast(rpl::forward(sender, message));
	}
	else if (context.checkChannelExist(toNickname) == true)
	{
		Channel &toChannel = context.getChannel(toNickname);
		if (toChannel.confirmInChannelByUser(sender))
		{
			toChannel.broadcastExcept(rpl::forward(sender, message), sender);
		}
		else
		{
			sender.userBroadcast(rpl::err_cannotsendtochan(sender, toChannel.getChannelName()));
		}
	}
	else
	{
		sender.userBroadcast(rpl::err_nosuchnick(sender, toNickname));
	}
}

void MessageHandler::quitCommandHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();
	if (context.checkUserInAnyChannel(sender) == true)
	{
		std::list<User *> users_in_same_channels = context.getMutualChannelUsers(
		            sender);
		std::list<User *>::iterator it = users_in_same_channels.begin();
		for (; it != users_in_same_channels.end(); it++)
		{

			(*it)->userBroadcast(rpl::quit(sender, message));
		}
	}
	context.forciblyDisconnect(sender);
}

void MessageHandler::summonCommandHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();
	sender.userBroadcast(rpl::err_summondisabled(sender));
}

void MessageHandler::userCommandHandler(CommandMessage &message)
{

	User &sender = message.getMessageSender();
	if (sender.is_fully_registered())
	{
		sender.userBroadcast(rpl::err_alreadyregistred(sender));
		return ;
	}
	else if (sender.has_user_info())
	{
		sender.userBroadcast(rpl::err_notregistered(sender));
		return;
	}
	try
	{
		sender.set_username(message.getCommandArgument("user"));
		sender.set_hostname(message.getCommandArgument("unused"));
		sender.set_realname(message.getCommandArgument("realname"));
		greetNewUser(sender);
	}
	catch (User::InvalidUsernameException &e)
	{
		sender.userBroadcast(rpl::err_invalidusername());
	}
	catch (std::exception &e)
	{
		logActionUtils::warn("MessageHandler: USER:", e.what());
	}
}

void MessageHandler::usersCommandHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();
	sender.userBroadcast(rpl::err_usersdisabled(sender));
}

void MessageHandler::versionRequestHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();
	sender.userBroadcast(rpl::server_version(sender));
}

void MessageHandler::greetNewUser(User &user)
{
	if (user.has_nickname() == false || user.has_user_info() == false)
	{
		return ;
	}
	try
	{
		context.promoteUserToActive(user);
		user.userBroadcast(rpl::welcome(user));
		user.userBroadcast(rpl::yourhost(user));
		user.userBroadcast(rpl::created(user));
		user.userBroadcast(rpl::myinfo(user));
	}
	catch (std::exception &e)
	{
		logActionUtils::warn("CommandMessage Handler: Welcome: ", e.what());
	}
}

void MessageHandler::pingCommandHandler(CommandMessage &message)
{
	User &sender = message.getMessageSender();

	if (message.getCommandArgument("token") == "")
	{
		sender.userBroadcast(rpl::err_needmoreparams(sender, "PING"));
		return;
	}
	sender.userBroadcast(rpl::pong(sender, message));
}

void MessageHandler::pongCommandHandler(CommandMessage &message)
{
	// Dummy To be implemented
	(void) message;
}
