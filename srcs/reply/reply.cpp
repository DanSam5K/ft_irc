#include "reply.hpp"
#include "reply_macros.hpp"
#include "error_reply.hpp"
#include "CommandMessage.hpp"
#include "ft_irc.hpp"
#include "Channel.hpp"
#include <cstdlib>
#include <sstream>

std::string const rpl::liststart(ClientUser &user)
{
	std::string reply = SERVER_PREFIX " ";
	reply += "321 ";
	reply += user.getNickname();
	reply += " Channel : Users Name";
	reply += "\r\n";
	return (reply);
}

std::string const rpl::list(ClientUser &user, Channel &channel)
{
	std::ostringstream nbr_user_str;

	nbr_user_str << channel.getUserCount();

	std::string reply = SERVER_PREFIX " ";
	reply += "322 ";
	reply += user.getNickname();
	reply += " ";
	reply += channel.getChannelName();
	reply += " ";
	reply += nbr_user_str.str();
	reply += "\r\n";
	return (reply);
}

std::string const rpl::listend(ClientUser &user)
{
	std::string reply = SERVER_PREFIX " ";
	reply += "323 ";
	reply += user.getNickname();
	reply += " :End of LIST";
	reply += "\r\n";
	return (reply);
}

std::string const rpl::welcome(ClientUser &user)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_WELCOME " ";
	reply += user.getNickname();
	reply += " :Welcome to the Internet Relay Network ";
	reply += user.getIdentifier();
	reply += "\r\n";
	return (reply);
}

std::string const rpl::yourhost(ClientUser &user)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_YOURHOST " ";
	reply += user.getNickname();
	reply += " :Your host is ";
	reply += SERVER_NAME;
	reply += ", running version ";
	reply += SERVER_VERSION;
	reply += "\r\n";
	return (reply);
}

std::string const rpl::created(ClientUser &user)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_CREATED " ";
	reply += user.getNickname();
	reply += " :This server was created ";
	reply += SERVER_CREATION;
	reply += "\r\n";
	return (reply);
}

std::string const rpl::myinfo(ClientUser &user)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_MYINFO " ";
	reply += user.getNickname();
	reply += " :";
	reply += SERVER_NAME " ";
	reply += SERVER_VERSION " ";
	reply += "io i";
	reply += "\r\n";
	return (reply);
}

std::string const rpl::server_version(ClientUser &user)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_VERSION " ";
	reply += user.getNickname();
	reply += " " SERVER_VERSION " " SERVER_NAME;
	reply += " :N/A\r\n";
	return (reply);
}

/* When replying to an ADMIN message, a server */
/* is expected to use replies RPL_ADMINME */
/* through to RPL_ADMINEMAIL and provide a text */
/* message with each.  For RPL_ADMINLOC1 a */
/* description of what city, state and country */
/* the server is in is expected, followed by */
/* details of the institution (RPL_ADMINLOC2) */
/* and finally the administrative contact for the */
/* server (an email address here is REQUIRED) */
/* in RPL_ADMINEMAIL. */
std::string const rpl::adminme(ClientUser &user)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_ADMINME " ";
	reply += user.getNickname();
	reply += " " SERVER_NAME;
	reply += " :Administrative info\r\n";
	return (reply);
}

std::string const rpl::adminloc1(ClientUser &user)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_ADMINLOC1 " ";
	reply += user.getNickname();
	reply += " :";
	reply += ADMIN_INFO_1 "\r\n";
	return (reply);
}

std::string const rpl::adminloc2(ClientUser &user)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_ADMINLOC2 " ";
	reply += user.getNickname();
	reply += " :";
	reply += ADMIN_INFO_2 "\r\n";
	return (reply);
}

std::string const rpl::adminemail(ClientUser &user)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_ADMINEMAIL " ";
	reply += user.getNickname();
	reply += " :";
	reply += ADMIN_EMAIL "\r\n";
	return (reply);
}

/* A server responding to an STATUS message is required to */
/* send all its 'info' in a series of RPL_INFO messages */
/* with a RPL_ENDOFINFO reply to indicate the end of the */
/* replies. */
std::string const rpl::info(ClientUser &user, int line)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_INFO " ";
	reply += user.getNickname();
	reply += " :";
	switch (line)
	{
		case 0:
			reply += "Server name: " SERVER_NAME "\r\n";
			break;
		case 1:
			reply += "Version: " SERVER_VERSION "\r\n";
			break;
		case 2:
			reply += "Created on: " SERVER_CREATION "\r\n";
			break;
	}
	return (reply);
}

std::string const rpl::info_end(ClientUser &user)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_ENDOFINFO " ";
	reply += user.getNickname();
	reply += " :End of STATUS list\r\n";
	return (reply);
}

std::string const rpl::forward(ClientUser &sender, CommandMessage &message)
{
	std::string reply = ":";
	reply += sender.getIdentifier();
	reply += " ";
	reply += message.getCommandMessage();
	reply += " ";
	reply += message.getCommandArgument("msgtarget");
	reply += " :";
	if (message.checkCommandArgument("text to be sent"))
	{
		reply += message.getCommandArgument("text to be sent");
	}
	reply += "\r\n";
	return (reply);
}

std::string const rpl::confirmation(std::string const identifier,
                                     CommandMessage &message)
{
	std::string reply = ":";
	reply += identifier;
	reply += " ";
	reply += message.getCommandMessage();
	reply += " ";
	reply += message.getCommandArgument("nickname");
	reply += "\r\n";
	return (reply);
}

/* :coucou!~b@localhost JOIN :#test */
std::string const rpl::join_channel(ClientUser &user, Channel &channel)
{
	std::string reply = ":";
	reply += user.getIdentifier();
	reply += " JOIN :";
	reply += channel.getChannelName();
	reply += "\r\n";
	return (reply);
}

std::string const rpl::mode_channel(ClientUser &user, Channel &channel,
                                     std::string modeString)
{
	std::string reply = ":";
	reply += user.getIdentifier();
	reply += " MODE ";
	reply += channel.getChannelName();
	reply += " ";
	reply += modeString;
	reply += "\r\n";
	return (reply);
}

/* To reply to a NAMES message, a reply pair consisting */
/* of RPL_NAMREPLY and RPL_ENDOFNAMES is sent by the */
/* server back to the client.  If there is no channel */
/* found as in the query, then only RPL_ENDOFNAMES is */
/* returned.  The exception to this is when a NAMES */
/* message is sent with no parameters and all visible */
/* channels and contents are sent back in a series of */
/* RPL_NAMEREPLY messages with a RPL_ENDOFNAMES to mark */
/* the end. */
/* CommandMessage format: "("=" / "*" / "@") <channel> :[ "@" / "+" ] <nick> *(" " [ "@" / "+" ] <nick>)" */
/* "@" is used for secret channels, "*" for private */
/* channels, and "=" for others (public channels). */
std::string const rpl::namreply(ClientUser &user, Channel &channel)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_NAMREPLY " ";
	reply += user.getNickname();
	reply += " = ";
	reply += channel.getChannelName();
	reply += " :";
	reply += channel.getUserListString();
	reply += "\r\n";
	return (reply);
}

std::string const rpl::endofnames(ClientUser &user, std::string chan_name)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_ENDOFNAMES " ";
	reply += user.getNickname();
	reply += " ";
	reply += chan_name;
	reply += " :End of NAMES list\r\n";
	return (reply);
}

std::string const rpl::part(ClientUser &user, Channel &channel, CommandMessage &message)
{
	std::string reply = ":";
	reply += user.getIdentifier();
	reply += " ";
	reply += message.getCommandMessage();
	reply += " ";
	reply += channel.getChannelName();
	reply += " :";
	if (message.checkCommandArgument("Part CommandMessage"))
	{
		reply += message.getCommandArgument("Part CommandMessage");
	}
	else
	{
		reply += user.getNickname();
	}
	reply += "\r\n";
	return (reply);
}

std::string const rpl::create_part_message(Channel &channel)
{
	std::string msg = "PART ";
	msg += channel.getChannelName();
	msg += "\r\n";
	return (msg);
}

std::string const rpl::quit(ClientUser &user, CommandMessage &message)
{
	std::string reply = ":";
	reply += user.getIdentifier();
	reply += " ";
	reply += message.getCommandMessage();
	reply += " :";
	if (message.checkCommandArgument("Quit CommandMessage"))
	{
		reply += message.getCommandArgument("Quit CommandMessage");
	}
	else
	{
		reply += user.getNickname();
	}
	reply += "\r\n";
	return (reply);
}

std::string const rpl::inviting(ClientUser &user, CommandMessage &message)
{
	std::string reply = ":";
	reply += user.getIdentifier();
	reply += " ";
	reply += RPL_INVITING " ";
	reply += message.getMessageSender().getNickname();
	reply += " ";
	reply += message.getCommandArgument("nickname");
	reply += " ";
	reply += message.getCommandArgument("channel");
	reply += "\r\n";
	return (reply);
}

std::string const rpl::invite(ClientUser &sender, CommandMessage &message)
{
	std::string reply = ":";
	reply += sender.getIdentifier();
	reply += " ";
	reply += "INVITE";
	reply += " ";
	reply += message.getCommandArgument("nickname");
	reply += " ";
	reply += message.getCommandArgument("channel");
	reply += "\r\n";
	return (reply);
}

std::string const rpl::kick(ClientUser &sender, ClientUser &user, Channel &channel,
                             std::string comment)
{
	std::string reply = ":";
	reply += sender.getIdentifier();
	reply += " ";
	reply += "KICK";
	reply += " ";
	reply += channel.getChannelName();
	reply += " ";
	reply += user.getNickname();
	reply += " :";
	reply += comment;
	reply += "\r\n";
	return (reply);
}

std::string const rpl::notopic(CommandMessage &message, Channel &channel)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_NOTOPIC " ";
	reply += message.getMessageSender().getNickname();
	reply += " ";
	reply += channel.getChannelName();
	reply += " :No topic is set";
	reply += "\r\n";
	return (reply);
}

std::string const rpl::topic(CommandMessage &message, Channel &channel)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_TOPIC " ";
	reply += message.getMessageSender().getNickname();
	reply += " ";
	reply += channel.getChannelName();
	reply += " :";
	reply += channel.getTopicMessage();
	reply += "\r\n";
	return (reply);
}

std::string const rpl::umodeis(CommandMessage &message, ClientUser &user)
{
	(void) message;
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_UMODEIS " ";
	reply += user.getNickname();
	reply += " ";
	reply += user.getModeFlags();
	reply += "\r\n";
	return (reply);
}

std::string const rpl::channelmodeis(CommandMessage &message, Channel &channel)
{
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_CHANNELMODEIS " ";
	reply += message.getMessageSender().getNickname();
	reply += " ";
	reply += channel.getChannelName();
	reply += " ";
	reply += channel.getModeString();
	reply += "\r\n";
	return (reply);
}

std::string const rpl::newtopic(ClientUser &sender, CommandMessage &message)
{
	std::string reply = ":";
	reply += sender.getIdentifier();
	reply += " ";
	reply += "TOPIC";
	reply += " ";
	reply += message.getCommandArgument("channel");
	reply += " :";
	reply += message.getCommandArgument("topic");
	reply += "\r\n";
	return (reply);
}

std::string const rpl::pong(ClientUser &sender, CommandMessage &message)
{
	std::string reply = ":";
	reply += sender.getIdentifier();
	reply += " ";
	reply += "PONG";
	reply += " ";
	reply += SERVER_NAME;
	reply += " ";
	reply += message.getCommandArgument("token");
	reply += "\r\n";
	return (reply);
}

std::string const rpl::ping(std::string token)
{
	std::string reply = "";
	reply += "PING";
	reply += " ";
	reply += token;
	reply += "\r\n";
	return (reply);
}

std::string const rpl::invalidmodeparam(ClientUser &sender,
        std::string channel_name, char mode_char, std::string description)
{
	// "<client> <modeTarget chan/user> <mode char> <parameter> :<description>"
	std::string reply = SERVER_PREFIX " ";
	reply += RPL_INVALIDMODEPARAM " ";
	reply += sender.getNickname();
	reply += " ";
	reply += channel_name;
	reply += " ";
	reply += mode_char;
	reply += " :";
	reply += description;
	reply += "\r\n";
	return (reply);
}
