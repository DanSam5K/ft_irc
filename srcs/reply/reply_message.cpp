#include "reply_message.hpp"
#include "reply_macros.hpp"
#include "error_reply.hpp"
#include "CommandMessage.hpp"
#include "ft_irc.hpp"
#include "Channel.hpp"
#include <cstdlib>
#include <sstream>

// Generates the header for the channel list response
std::string const rpl_msg::listStart(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += "321 ";
    reply += user.getNickname();
    reply += " Channel : Users Name";
    reply += "\r\n";
    return (reply);
}

// Constructs a line for each channel in the channel list, including user count
std::string const rpl_msg::list(ClientUser &user, Channel &channel)
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

// Indicates the end of the channel list
std::string const rpl_msg::listEnd(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += "323 ";
    reply += user.getNickname();
    reply += " :End of LIST";
    reply += "\r\n";
    return (reply);
}

// Sends a welcome message to a user upon successful registration
std::string const rpl_msg::welcome(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += RPL_WELCOME " ";
    reply += user.getNickname();
    reply += " :Welcome to the Internet Relay Network ";
    reply += user.getIdentifier();
    reply += "\r\n";
    return (reply);
}

// Informs the user about the server host and version
std::string const rpl_msg::yourHost(ClientUser &user)
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

// Provides the server creation date to the user
std::string const rpl_msg::created(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += RPL_CREATED " ";
    reply += user.getNickname();
    reply += " :This server was created ";
    reply += SERVER_CREATION;
    reply += "\r\n";
    return (reply);
}

// Shares server details such as name, version, and supported modes
std::string const rpl_msg::myInfo(ClientUser &user)
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

// Returns the server version and name
std::string const rpl_msg::serverVersion(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += RPL_VERSION " ";
    reply += user.getNickname();
    reply += " " SERVER_VERSION " " SERVER_NAME;
    reply += " :N/A\r\n";
    return (reply);
}

// Administrative info: server name and a generic message
std::string const rpl_msg::adminMe(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += RPL_ADMINME " ";
    reply += user.getNickname();
    reply += " " SERVER_NAME;
    reply += " :Administrative info\r\n";
    return (reply);
}

// Administrative info: server location (city, country, etc.)
std::string const rpl_msg::adminLoc1(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += RPL_ADMINLOC1 " ";
    reply += user.getNickname();
    reply += " :";
    reply += ADMIN_INFO_1 "\r\n";
    return (reply);
}

// Administrative info: institution or organization details
std::string const rpl_msg::adminLoc2(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += RPL_ADMINLOC2 " ";
    reply += user.getNickname();
    reply += " :";
    reply += ADMIN_INFO_2 "\r\n";
    return (reply);
}

// Administrative info: contact email for server admin
std::string const rpl_msg::adminEmail(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += RPL_ADMINEMAIL " ";
    reply += user.getNickname();
    reply += " :";
    reply += ADMIN_EMAIL "\r\n";
    return (reply);
}

// Provides server information lines in response to an INFO command
std::string const rpl_msg::info(ClientUser &user, int line)
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

// Marks the end of the server information response
std::string const rpl_msg::infoEnd(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += RPL_ENDOFINFO " ";
    reply += user.getNickname();
    reply += " :End of STATUS list\r\n";
    return (reply);
}

// Formats a message to forward to another user or channel
std::string const rpl_msg::forward(ClientUser &sender, CommandMessage &message)
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

// Confirms an action such as a successful command execution
std::string const rpl_msg::confirmation(std::string const identifier, CommandMessage &message)
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

// Notifies all users in a channel when a user joins
std::string const rpl_msg::joinChannel(ClientUser &user, Channel &channel)
{
    std::string reply = ":";
    reply += user.getIdentifier();
    reply += " JOIN :";
    reply += channel.getChannelName();
    reply += "\r\n";
    return (reply);
}

// Announces a mode change in a channel
std::string const rpl_msg::modeChannel(ClientUser &user, Channel &channel, std::string modeString)
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

// Lists users in a channel in response to a NAMES command
std::string const rpl_msg::nameReply(ClientUser &user, Channel &channel)
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

// Indicates the end of the NAMES list for a channel
std::string const rpl_msg::endOfNames(ClientUser &user, std::string channelName)
{
    std::string reply = SERVER_PREFIX " ";
    reply += RPL_ENDOFNAMES " ";
    reply += user.getNickname();
    reply += " ";
    reply += channelName;
    reply += " :End of NAMES list\r\n";
    return (reply);
}

// Notifies users when someone leaves a channel
std::string const rpl_msg::part(ClientUser &user, Channel &channel, CommandMessage &message)
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

// Creates a PART command message for a channel
std::string const rpl_msg::createPartMessage(Channel &channel)
{
    std::string msg = "PART ";
    msg += channel.getChannelName();
    msg += "\r\n";
    return (msg);
}

// Notifies users when someone disconnects from the server
std::string const rpl_msg::quit(ClientUser &user, CommandMessage &message)
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

// Informs a user that they have invited another user to a channel
std::string const rpl_msg::inviting(ClientUser &user, CommandMessage &message)
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

// Sends an invitation message to a user for a channel
std::string const rpl_msg::invite(ClientUser &sender, CommandMessage &message)
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

// Notifies a user that they have been kicked from a channel
std::string const rpl_msg::kick(ClientUser &sender, ClientUser &user, Channel &channel, std::string comment)
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

// Informs a user that a channel does not have a topic set
std::string const rpl_msg::noTopic(CommandMessage &message, Channel &channel)
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

// Sends the current topic of a channel to a user
std::string const rpl_msg::topic(CommandMessage &message, Channel &channel)
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

// Returns the mode flags set for a user
std::string const rpl_msg::umodeIs(CommandMessage &message, ClientUser &user)
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

// Returns the mode settings for a channel
std::string const rpl_msg::channelModeIs(CommandMessage &message, Channel &channel)
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

// Notifies users of a new topic set for a channel
std::string const rpl_msg::newTopic(ClientUser &sender, CommandMessage &message)
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

// Responds to a PING command with a PONG reply
std::string const rpl_msg::pong(ClientUser &sender, CommandMessage &message)
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

// Generates a PING message with a given token
std::string const rpl_msg::ping(std::string token)
{
    std::string reply = "";
    reply += "PING";
    reply += " ";
    reply += token;
    reply += "\r\n";
    return (reply);
}

// Informs the user of an invalid mode parameter for a channel or user
std::string const rpl_msg::invalidModeParam(ClientUser &sender, std::string channelName, char mode_char, std::string description)
{
    std::string reply = SERVER_PREFIX " ";
    reply += RPL_INVALIDMODEPARAM " ";
    reply += sender.getNickname();
    reply += " ";
    reply += channelName;
    reply += " ";
    reply += mode_char;
    reply += " :";
    reply += description;
    reply += "\r\n";
    return (reply);
}









// #include "reply_message.hpp"
// #include "reply_macros.hpp"
// #include "error_reply.hpp"
// #include "CommandMessage.hpp"
// #include "ft_irc.hpp"
// #include "Channel.hpp"
// #include <cstdlib>
// #include <sstream>

// std::string const rpl_msg::listStart(ClientUser &user)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += "321 ";
// 	reply += user.getNickname();
// 	reply += " Channel : Users Name";
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::list(ClientUser &user, Channel &channel)
// {
// 	std::ostringstream nbr_user_str;

// 	nbr_user_str << channel.getUserCount();

// 	std::string reply = SERVER_PREFIX " ";
// 	reply += "322 ";
// 	reply += user.getNickname();
// 	reply += " ";
// 	reply += channel.getChannelName();
// 	reply += " ";
// 	reply += nbr_user_str.str();
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::listEnd(ClientUser &user)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += "323 ";
// 	reply += user.getNickname();
// 	reply += " :End of LIST";
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::welcome(ClientUser &user)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_WELCOME " ";
// 	reply += user.getNickname();
// 	reply += " :Welcome to the Internet Relay Network ";
// 	reply += user.getIdentifier();
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::yourHost(ClientUser &user)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_YOURHOST " ";
// 	reply += user.getNickname();
// 	reply += " :Your host is ";
// 	reply += SERVER_NAME;
// 	reply += ", running version ";
// 	reply += SERVER_VERSION;
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::created(ClientUser &user)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_CREATED " ";
// 	reply += user.getNickname();
// 	reply += " :This server was created ";
// 	reply += SERVER_CREATION;
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::myInfo(ClientUser &user)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_MYINFO " ";
// 	reply += user.getNickname();
// 	reply += " :";
// 	reply += SERVER_NAME " ";
// 	reply += SERVER_VERSION " ";
// 	reply += "io i";
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::serverVersion(ClientUser &user)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_VERSION " ";
// 	reply += user.getNickname();
// 	reply += " " SERVER_VERSION " " SERVER_NAME;
// 	reply += " :N/A\r\n";
// 	return (reply);
// }

// /* When replying to an ADMIN message, a server */
// /* is expected to use replies RPL_ADMINME */
// /* through to RPL_ADMINEMAIL and provide a text */
// /* message with each.  For RPL_ADMINLOC1 a */
// /* description of what city, state and country */
// /* the server is in is expected, followed by */
// /* details of the institution (RPL_ADMINLOC2) */
// /* and finally the administrative contact for the */
// /* server (an email address here is REQUIRED) */
// /* in RPL_ADMINEMAIL. */
// std::string const rpl_msg::adminMe(ClientUser &user)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_ADMINME " ";
// 	reply += user.getNickname();
// 	reply += " " SERVER_NAME;
// 	reply += " :Administrative info\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::adminLoc1(ClientUser &user)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_ADMINLOC1 " ";
// 	reply += user.getNickname();
// 	reply += " :";
// 	reply += ADMIN_INFO_1 "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::adminLoc2(ClientUser &user)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_ADMINLOC2 " ";
// 	reply += user.getNickname();
// 	reply += " :";
// 	reply += ADMIN_INFO_2 "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::adminEmail(ClientUser &user)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_ADMINEMAIL " ";
// 	reply += user.getNickname();
// 	reply += " :";
// 	reply += ADMIN_EMAIL "\r\n";
// 	return (reply);
// }

// /* A server responding to an STATUS message is required to */
// /* send all its 'info' in a series of RPL_INFO messages */
// /* with a RPL_ENDOFINFO reply to indicate the end of the */
// /* replies. */
// std::string const rpl_msg::info(ClientUser &user, int line)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_INFO " ";
// 	reply += user.getNickname();
// 	reply += " :";
// 	switch (line)
// 	{
// 		case 0:
// 			reply += "Server name: " SERVER_NAME "\r\n";
// 			break;
// 		case 1:
// 			reply += "Version: " SERVER_VERSION "\r\n";
// 			break;
// 		case 2:
// 			reply += "Created on: " SERVER_CREATION "\r\n";
// 			break;
// 	}
// 	return (reply);
// }

// std::string const rpl_msg::infoEnd(ClientUser &user)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_ENDOFINFO " ";
// 	reply += user.getNickname();
// 	reply += " :End of STATUS list\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::forward(ClientUser &sender, CommandMessage &message)
// {
// 	std::string reply = ":";
// 	reply += sender.getIdentifier();
// 	reply += " ";
// 	reply += message.getCommandMessage();
// 	reply += " ";
// 	reply += message.getCommandArgument("msgtarget");
// 	reply += " :";
// 	if (message.checkCommandArgument("text to be sent"))
// 	{
// 		reply += message.getCommandArgument("text to be sent");
// 	}
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::confirmation(std::string const identifier,
//                                      CommandMessage &message)
// {
// 	std::string reply = ":";
// 	reply += identifier;
// 	reply += " ";
// 	reply += message.getCommandMessage();
// 	reply += " ";
// 	reply += message.getCommandArgument("nickname");
// 	reply += "\r\n";
// 	return (reply);
// }

// /* :coucou!~b@localhost JOIN :#test */
// std::string const rpl_msg::joinChannel(ClientUser &user, Channel &channel)
// {
// 	std::string reply = ":";
// 	reply += user.getIdentifier();
// 	reply += " JOIN :";
// 	reply += channel.getChannelName();
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::modeChannel(ClientUser &user, Channel &channel,
//                                      std::string modeString)
// {
// 	std::string reply = ":";
// 	reply += user.getIdentifier();
// 	reply += " MODE ";
// 	reply += channel.getChannelName();
// 	reply += " ";
// 	reply += modeString;
// 	reply += "\r\n";
// 	return (reply);
// }

// /* To reply to a NAMES message, a reply pair consisting */
// /* of RPL_NAMREPLY and RPL_ENDOFNAMES is sent by the */
// /* server back to the client.  If there is no channel */
// /* found as in the query, then only RPL_ENDOFNAMES is */
// /* returned.  The exception to this is when a NAMES */
// /* message is sent with no parameters and all visible */
// /* channels and contents are sent back in a series of */
// /* RPL_NAMEREPLY messages with a RPL_ENDOFNAMES to mark */
// /* the end. */
// /* CommandMessage format: "("=" / "*" / "@") <channel> :[ "@" / "+" ] <nick> *(" " [ "@" / "+" ] <nick>)" */
// /* "@" is used for secret channels, "*" for private */
// /* channels, and "=" for others (public channels). */
// std::string const rpl_msg::nameReply(ClientUser &user, Channel &channel)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_NAMREPLY " ";
// 	reply += user.getNickname();
// 	reply += " = ";
// 	reply += channel.getChannelName();
// 	reply += " :";
// 	reply += channel.getUserListString();
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::endOfNames(ClientUser &user, std::string channelName)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_ENDOFNAMES " ";
// 	reply += user.getNickname();
// 	reply += " ";
// 	reply += channelName;
// 	reply += " :End of NAMES list\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::part(ClientUser &user, Channel &channel, CommandMessage &message)
// {
// 	std::string reply = ":";
// 	reply += user.getIdentifier();
// 	reply += " ";
// 	reply += message.getCommandMessage();
// 	reply += " ";
// 	reply += channel.getChannelName();
// 	reply += " :";
// 	if (message.checkCommandArgument("Part CommandMessage"))
// 	{
// 		reply += message.getCommandArgument("Part CommandMessage");
// 	}
// 	else
// 	{
// 		reply += user.getNickname();
// 	}
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::createPartMessage(Channel &channel)
// {
// 	std::string msg = "PART ";
// 	msg += channel.getChannelName();
// 	msg += "\r\n";
// 	return (msg);
// }

// std::string const rpl_msg::quit(ClientUser &user, CommandMessage &message)
// {
// 	std::string reply = ":";
// 	reply += user.getIdentifier();
// 	reply += " ";
// 	reply += message.getCommandMessage();
// 	reply += " :";
// 	if (message.checkCommandArgument("Quit CommandMessage"))
// 	{
// 		reply += message.getCommandArgument("Quit CommandMessage");
// 	}
// 	else
// 	{
// 		reply += user.getNickname();
// 	}
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::inviting(ClientUser &user, CommandMessage &message)
// {
// 	std::string reply = ":";
// 	reply += user.getIdentifier();
// 	reply += " ";
// 	reply += RPL_INVITING " ";
// 	reply += message.getMessageSender().getNickname();
// 	reply += " ";
// 	reply += message.getCommandArgument("nickname");
// 	reply += " ";
// 	reply += message.getCommandArgument("channel");
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::invite(ClientUser &sender, CommandMessage &message)
// {
// 	std::string reply = ":";
// 	reply += sender.getIdentifier();
// 	reply += " ";
// 	reply += "INVITE";
// 	reply += " ";
// 	reply += message.getCommandArgument("nickname");
// 	reply += " ";
// 	reply += message.getCommandArgument("channel");
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::kick(ClientUser &sender, ClientUser &user, Channel &channel,
//                              std::string comment)
// {
// 	std::string reply = ":";
// 	reply += sender.getIdentifier();
// 	reply += " ";
// 	reply += "KICK";
// 	reply += " ";
// 	reply += channel.getChannelName();
// 	reply += " ";
// 	reply += user.getNickname();
// 	reply += " :";
// 	reply += comment;
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::noTopic(CommandMessage &message, Channel &channel)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_NOTOPIC " ";
// 	reply += message.getMessageSender().getNickname();
// 	reply += " ";
// 	reply += channel.getChannelName();
// 	reply += " :No topic is set";
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::topic(CommandMessage &message, Channel &channel)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_TOPIC " ";
// 	reply += message.getMessageSender().getNickname();
// 	reply += " ";
// 	reply += channel.getChannelName();
// 	reply += " :";
// 	reply += channel.getTopicMessage();
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::umodeIs(CommandMessage &message, ClientUser &user)
// {
// 	(void) message;
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_UMODEIS " ";
// 	reply += user.getNickname();
// 	reply += " ";
// 	reply += user.getModeFlags();
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::channelModeIs(CommandMessage &message, Channel &channel)
// {
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_CHANNELMODEIS " ";
// 	reply += message.getMessageSender().getNickname();
// 	reply += " ";
// 	reply += channel.getChannelName();
// 	reply += " ";
// 	reply += channel.getModeString();
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::newTopic(ClientUser &sender, CommandMessage &message)
// {
// 	std::string reply = ":";
// 	reply += sender.getIdentifier();
// 	reply += " ";
// 	reply += "TOPIC";
// 	reply += " ";
// 	reply += message.getCommandArgument("channel");
// 	reply += " :";
// 	reply += message.getCommandArgument("topic");
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::pong(ClientUser &sender, CommandMessage &message)
// {
// 	std::string reply = ":";
// 	reply += sender.getIdentifier();
// 	reply += " ";
// 	reply += "PONG";
// 	reply += " ";
// 	reply += SERVER_NAME;
// 	reply += " ";
// 	reply += message.getCommandArgument("token");
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::ping(std::string token)
// {
// 	std::string reply = "";
// 	reply += "PING";
// 	reply += " ";
// 	reply += token;
// 	reply += "\r\n";
// 	return (reply);
// }

// std::string const rpl_msg::invalidModeParam(ClientUser &sender,
//         std::string channelName, char mode_char, std::string description)
// {
// 	// "<client> <modeTarget chan/user> <mode char> <parameter> :<description>"
// 	std::string reply = SERVER_PREFIX " ";
// 	reply += RPL_INVALIDMODEPARAM " ";
// 	reply += sender.getNickname();
// 	reply += " ";
// 	reply += channelName;
// 	reply += " ";
// 	reply += mode_char;
// 	reply += " :";
// 	reply += description;
// 	reply += "\r\n";
// 	return (reply);
// }
