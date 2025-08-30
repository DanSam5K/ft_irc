/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  						        error_reply.cpp 	 					     #
#****************************************************************************/

#include "reply_message.hpp"
#include "reply_macros.hpp"
#include "error_reply.hpp"

// Reply when a requested nickname or channel does not exist
std::string const rpl_msg::errNoSuchNick(ClientUser &user, std::string nameOrChannel)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOSUCHNICK " ";
    reply += user.getNickname();
    reply += " ";
    reply += nameOrChannel;
    reply += " :No such nick/channel\r\n";
    return (reply);
}

// Reply for invalid or malformed username input
std::string const rpl_msg::errInvalidUsername()
{
    std::string reply = "ERROR :Invalid user name\r\n";
    return (reply);
}

// Reply for invalid or malformed mode string input
std::string const rpl_msg::errInvalidModeString()
{
    std::string reply = "ERROR :Invalid mode string\r\n";
    return (reply);
}

// Reply when a specified server does not exist
std::string const rpl_msg::errNoSuchServer(ClientUser &user, std::string serverName)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOSUCHSERVER " ";
    reply += user.getNickname();
    reply += " ";
    reply += serverName;
    reply += " :No such server\r\n";
    return (reply);
}

// Reply when a specified channel does not exist
std::string const rpl_msg::errNoSuchChannel(ClientUser &user, std::string channelName)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOSUCHCHANNEL " ";
    reply += user.getNickname();
    reply += " ";
    reply += channelName;
    reply += " :No such channel\r\n";
    return (reply);
}

// Reply when a user is not allowed to send messages to a channel
std::string const rpl_msg::errCannotSendToChan(ClientUser &user, std::string channelName)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_CANNOTSENDTOCHAN " ";
    reply += user.getNickname();
    reply += " ";
    reply += channelName;
    reply += " :Cannot send to channel\r\n";
    return (reply);
}

// Reply when a user tries to join more channels than allowed
std::string const rpl_msg::errTooManyChannels(ClientUser &user, std::string channelName)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_TOOMANYCHANNELS " ";
    reply += user.getNickname();
    reply += " ";
    reply += channelName;
    reply += " :You have joined too many channels\r\n";
    return (reply);
}

// Reply when there is no history for a requested nickname
std::string const rpl_msg::errWasNoSuchNick(ClientUser &user, std::string nickname)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_WASNOSUCHNICK " ";
    reply += user.getNickname();
    reply += " ";
    reply += nickname;
    reply += " :There was no such nickname\r\n";
    return (reply);
}

// Reply for too many targets in a command (e.g., too many recipients)
std::string const rpl_msg::errTooManyTargets(ClientUser &user, std::string modeTarget, int error_code, std::string abortMessage)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_TOOMANYTARGETS " ";
    reply += user.getNickname();
    reply += " ";
    reply += modeTarget;
    reply += " :";
    reply += error_code;
    reply += " recipients. ";
    reply += abortMessage;
    reply += "\r\n";
    return (reply);
}

// Reply when a requested service does not exist
std::string const rpl_msg::errNoSuchService(ClientUser &user, std::string service_name)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOSUCHSERVICE " ";
    reply += user.getNickname();
    reply += " ";
    reply += service_name;
    reply += " :No such service\r\n";
    return (reply);
}

// Reply when a PING or PONG command is missing the origin parameter
std::string const rpl_msg::errNoOrigin(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOORIGIN " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :No origin specified\r\n";
    return (reply);
}

// Reply when a command is missing a recipient parameter
std::string const rpl_msg::errNoRecipient(ClientUser &user, std::string command)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NORECIPIENT " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :No recipient given (";
    reply += command;
    reply += ")\r\n";
    return (reply);
}

// Reply when a command is missing the message text
std::string const rpl_msg::errNoTextToSend(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOTEXTTOSEND " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :No text to send\r\n";
    return (reply);
}

// Reply for invalid use of top-level domain in a mask
std::string const rpl_msg::errNoTopLevel(ClientUser &user, std::string mask)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOTOPLEVEL " ";
    reply += user.getNickname();
    reply += " ";
    reply += mask;
    reply += " :No toplevel domain specified\r\n";
    return (reply);
}

// Reply for wildcard in top-level domain in a mask
std::string const rpl_msg::errWildTopLevel(ClientUser &user, std::string mask)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_WILDTOPLEVEL " ";
    reply += user.getNickname();
    reply += " ";
    reply += mask;
    reply += " :Wildcard in toplevel domain\r\n";
    return (reply);
}

// Reply for invalid or malformed server/host mask
std::string const rpl_msg::errBadMask(ClientUser &user, std::string mask)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_BADMASK " ";
    reply += user.getNickname();
    reply += " ";
    reply += mask;
    reply += " :Bad Server/host mask\r\n";
    return (reply);
}

// Reply for unknown or unsupported command
std::string const rpl_msg::errUnknownCommand(ClientUser &user, std::string command)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_UNKNOWNCOMMAND " ";
    reply += user.getNickname();
    reply += " ";
    reply += command;
    reply += " :Unknown command\r\n";
    return (reply);
}

// Reply when the server's MOTD file is missing or cannot be opened
std::string const rpl_msg::errNoMotd(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOMOTD " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :MOTD File is missing\r\n";
    return (reply);
}

// Reply when admin info for a server is unavailable
std::string const rpl_msg::errNoAdminInfo(ClientUser &user, std::string server)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOADMININFO " ";
    reply += user.getNickname();
    reply += " ";
    reply += server;
    reply += " :No administrative info available\r\n";
    return (reply);
}

// Reply for file operation errors (e.g., file not found, permission denied)
std::string const rpl_msg::errFileError(ClientUser &user, std::string file_operation, std::string filename)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_FILEERROR " ";
    reply += user.getNickname();
    reply += " ";
    reply += ":File error doing ";
    reply += file_operation;
    reply += " on ";
    reply += filename;
    reply += "\r\n";
    return (reply);
}

// Reply when a nickname is missing in a command
std::string const rpl_msg::errNoNicknameGiven(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NONICKNAMEGIVEN " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :No nickname given\r\n";
    return (reply);
}

// Reply when a nickname contains invalid characters
std::string const rpl_msg::errErroneousNickname(ClientUser &user, std::string nickname)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_ERRONEUSNICKNAME " ";
    reply += user.getNickname();
    reply += " ";
    reply += nickname;
    reply += " :Erroneous nickname\r\n";
    return (reply);
}

// Reply when a nickname is too long
std::string const rpl_msg::errNicknameTooLong(ClientUser &user, std::string nickname)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NICKNAMETOOLONG " ";
    reply += user.getNickname();
    reply += " ";
    reply += nickname;
    reply += " :Nickname too long, max. 9 characters\r\n";
    return (reply);
}

// Reply when a nickname is already in use
std::string const rpl_msg::errNicknameInUse(ClientUser &user, std::string nickname)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NICKNAMEINUSE " ";
    reply += user.getNickname();
    reply += " ";
    reply += nickname;
    reply += " :Nickname is already in use\r\n";
    return (reply);
}

// Reply when a nickname or channel is temporarily unavailable (e.g., due to delay)
std::string const rpl_msg::errUnavailResource(ClientUser &user, std::string nickname_or_channel)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_UNAVAILRESOURCE " ";
    reply += user.getNickname();
    reply += " ";
    reply += nickname_or_channel;
    reply += " :Nick/channel is temporarily unavailable\r\n";
    return (reply);
}

// Reply when a user is not present in a specified channel
std::string const rpl_msg::errUserNotInChannel(ClientUser &user, std::string nickname, std::string channel)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_USERNOTINCHANNEL " ";
    reply += user.getNickname();
    reply += " ";
    reply += nickname;
    reply += " ";
    reply += channel;
    reply += " :They aren't on that channel\r\n";
    return (reply);
}

// Reply when a user tries to interact with a channel they are not a member of
std::string const rpl_msg::errNotOnChannel(ClientUser &user, std::string channel)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOTONCHANNEL " ";
    reply += user.getNickname();
    reply += " ";
    reply += channel;
    reply += " :You're not on that channel\r\n";
    return (reply);
}

// Reply when a user tries to invite someone already in the channel
std::string const rpl_msg::errUserOnChannel(ClientUser &user, std::string username, std::string channel)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_USERONCHANNEL " ";
    reply += user.getNickname();
    reply += " ";
    reply += username;
    reply += " ";
    reply += channel;
    reply += " :is already on channel\r\n";
    return (reply);
}

// Reply when a SUMMON command fails because the user is not logged in
std::string const rpl_msg::errNoLogin(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOLOGIN " ";
    reply += user.getNickname();
    reply += " :ClientUser not logged in\r\n";
    return (reply);
}

// Reply when the SUMMON command is disabled on the server
std::string const rpl_msg::errSummonDisabled(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_SUMMONDISABLED " ";
    reply += user.getNickname();
    reply += " :SUMMON has been disabled\r\n";
    return (reply);
}

// Reply when the USERS command is disabled on the server
std::string const rpl_msg::errUsersDisabled(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_USERSDISABLED " ";
    reply += user.getNickname();
    reply += " :USERS has been disabled\r\n";
    return (reply);
}

// Reply when a user tries to execute a command before registering
std::string const rpl_msg::errNotRegistered(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOTREGISTERED " ";
    reply += user.getNickname();
    reply += " :You have not registered\r\n";
    return (reply);
}

// Reply when a command is missing required parameters
std::string const rpl_msg::errNeedMoreParams(ClientUser &user, std::string command)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NEEDMOREPARAMS " ";
    reply += user.getNickname();
    reply += " ";
    reply += command;
    reply += " :Not enough parameters\r\n";
    return (reply);
}

// Reply when a command has too many parameters
std::string const rpl_msg::errTooManyParams(ClientUser &user, std::string command)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NEEDMOREPARAMS " ";
    reply += user.getNickname();
    reply += " ";
    reply += command;
    reply += " :Too many parameters\r\n";
    return (reply);
}

// Reply when a user tries to register again after already being registered
std::string const rpl_msg::errAlreadyRegistered(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_ALREADYREGISTERED " ";
    reply += user.getNickname();
    reply += " :Unauthorized command (already registered)\r\n";
    return (reply);
}

// Reply when a user's host is not permitted to connect
std::string const rpl_msg::errNoPermForHost(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOPERMFORHOST " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :Your host isn't among the privileged\r\n";
    return (reply);
}

// Reply when a password is required or incorrect during registration
std::string const rpl_msg::errPasswdMismatch(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_PASSWDMISMATCH " ";
    reply += user.getNickname();
    reply += " :Password incorrect\r\n";
    return (reply);
}

// Reply when a user is banned from the server
std::string const rpl_msg::errYourBannedCreep(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_YOUREBANNEDCREEP " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :You are banned from this server\r\n";
    return (reply);
}

// Reply to warn a user that they will soon be banned
std::string const rpl_msg::errYouWillBeBanned(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_YOUWILLBEBANNED " ";
    reply += user.getNickname();
    reply += " ";
    reply += "\r\n";
    return (reply);
}

// Reply when a channel key is already set
std::string const rpl_msg::errKeyset(ClientUser &user, std::string channel)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_KEYSET " ";
    reply += user.getNickname();
    reply += " ";
    reply += channel;
    reply += " :Channel key already set\r\n";
    return (reply);
}

// Reply when a channel is full and cannot accept more users
std::string const rpl_msg::errChanneListFull(ClientUser &user, std::string channel)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_CHANNELISFULL " ";
    reply += user.getNickname();
    reply += " ";
    reply += channel;
    reply += " :Cannot join channel (+l)\r\n";
    return (reply);
}

// Reply when an unknown mode character is used in a command
std::string const rpl_msg::errUnknownMode(ClientUser &user, std::string channel, char modechar)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_UNKNOWNMODE " ";
    reply += user.getNickname();
    reply += " ";
    reply += modechar;
    reply += " :is unknown mode char for ";
    reply += channel;
    reply += " \r\n";
    return (reply);
}

// Reply when a channel is invite-only and the user is not invited
std::string const rpl_msg::errInviteOnlyChannel(ClientUser &user, std::string channel)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_INVITEONLYCHAN " ";
    reply += user.getNickname();
    reply += " ";
    reply += channel;
    reply += " :Cannot join channel (+i)\r\n";
    return (reply);
}

// Reply when a user is banned from a channel
std::string const rpl_msg::errBannedFromchannel(ClientUser &user, std::string channel)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_BANNEDFROMCHAN " ";
    reply += user.getNickname();
    reply += " ";
    reply += channel;
    reply += " :Cannot join channel (+b)\r\n";
    return (reply);
}

// Reply when a channel key is required to join a channel
std::string const rpl_msg::errBadChannelKey(ClientUser &user, std::string channel)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_BADCHANNELKEY " ";
    reply += user.getNickname();
    reply += " ";
    reply += channel;
    reply += " :Cannot join channel (+k)\r\n";
    return (reply);
}

// Reply when a channel mask is invalid
std::string const rpl_msg::errBadChannelMask(ClientUser &user, std::string channel)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_BADCHANMASK " ";
    reply += user.getNickname();
    reply += " ";
    reply += channel;
    reply += " :Bad Channel Mask\r\n";
    return (reply);
}

// Reply when a channel does not support modes
std::string const rpl_msg::errNoChannelModes(ClientUser &user, std::string channel)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOCHANMODES " ";
    reply += user.getNickname();
    reply += " ";
    reply += channel;
    reply += " :Channel doesn't support modes\r\n";
    return (reply);
}

// Reply when a channel's ban list is full
std::string const rpl_msg::errBanListFull(ClientUser &user, std::string channel, std::string modechar)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_BANLISTFULL " ";
    reply += user.getNickname();
    reply += " ";
    reply += channel;
    reply += " ";
    reply += modechar;
    reply += " :Channel list is full\r\n";
    return (reply);
}

// Reply when a user lacks IRC operator privileges
std::string const rpl_msg::errNoPrivileges(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOPRIVILEGES " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :Permission Denied- You're not an IRC operator\r\n";
    return (reply);
}

// Reply when a user lacks channel operator privileges
std::string const rpl_msg::errChanOprivsNeeded(ClientUser &user, std::string channel)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_CHANOPRIVSNEEDED " ";
    reply += user.getNickname();
    reply += " ";
    reply += channel;
    reply += " :You're not channel operator\r\n";
    return (reply);
}

// Reply when a user tries to use the KILL command on a server
std::string const rpl_msg::errCantKillServer(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_CANTKILLSERVER " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :You can't kill a server!\r\n";
    return (reply);
}

// Reply when a user's connection is restricted (e.g., user mode "+r")
std::string const rpl_msg::errRestricted(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_RESTRICTED " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :Your connection is restricted!\r\n";
    return (reply);
}

// Reply when a user lacks "channel creator" privileges
std::string const rpl_msg::errUniqOpprivsNeeded(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_UNIQOPPRIVSNEEDED " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :You're not the original channel operator\r\n";
    return (reply);
}

// Reply when a user's host is not configured for IRC operator privileges
std::string const rpl_msg::errNoOperHost(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_NOOPERHOST " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :No O-lines for your host\r\n";
    return (reply);
}

// Reply when an unknown user mode flag is used
std::string const rpl_msg::errUserModeUnknownFlag(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_UMODEUNKNOWNFLAG " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :Unknown MODE flag\r\n";
    return (reply);
}

// Reply when a user tries to change another user's mode
std::string const rpl_msg::errUsersDontMatch(ClientUser &user)
{
    std::string reply = SERVER_PREFIX " ";
    reply += ERR_USERSDONTMATCH " ";
    reply += user.getNickname();
    reply += " ";
    reply += " :Cannot change mode for other users\r\n";
    return (reply);
}

// Generic error reply for custom error messages
std::string const rpl_msg::errGeneric(std::string msg)
{
    std::string reply = "ERROR :";
    reply += msg;
    reply += "\r\n";
	return (reply);
}
