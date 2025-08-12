#ifndef REPLY_HPP
#define REPLY_HPP

#include "reply_macros.hpp"
#include "error_reply.hpp"
#include "ft_irc.hpp"
#include "ClientUser.hpp"
#include "CommandMessage.hpp"
#include "Channel.hpp"

namespace rpl_msg
{
    // ---- Server greeting & info ----
    std::string const welcome(ClientUser &user);
    std::string const yourHost(ClientUser &user);
    std::string const created(ClientUser &user);
    std::string const myInfo(ClientUser &user);
    std::string const serverVersion(ClientUser &user);

    // ---- Admin information ----
    std::string const adminMe(ClientUser &user);
    std::string const adminLoc1(ClientUser &user);
    std::string const adminLoc2(ClientUser &user);
    std::string const adminEmail(ClientUser &user);

    // ---- Info replies ----
    std::string const info(ClientUser &user, int line);
    std::string const infoEnd(ClientUser &user);

    // ---- Confirmation ----
    std::string const confirmation(std::string const identifier, CommandMessage &message);

    // ---- Channel actions ----
    std::string const joinChannel(ClientUser &user, Channel &channel);
    std::string const modeChannel(ClientUser &user, Channel &channel, std::string modeString);
    std::string const nameReply(ClientUser &user, Channel &channel);
    std::string const endOfNames(ClientUser &user, std::string channelName); 
    std::string const part(ClientUser &user, Channel &channel, CommandMessage &message);
    std::string const quit(ClientUser &user, CommandMessage &message);
    std::string const createPartMessage(Channel &channel);

    // ---- Generic forwarding ----
    std::string const forward(ClientUser &sender, CommandMessage &message);

    // ---- Error messages ----
    std::string const errNoSuchNick(ClientUser &user, std::string nameOrChannel);
    // std::string const errNoSuchNick(ClientUser &user, std::string nameOrChannel);
    std::string const errNoSuchServer(ClientUser &user, std::string serverName);
    std::string const errNoSuchChannel(ClientUser &user, std::string channelName);
    std::string const errCannotSendToChan(ClientUser &user, std::string channelName);
    std::string const errTooManyChannels(ClientUser &user, std::string channelName);
    std::string const errWasNoSuchNick(ClientUser &user, std::string nickname);
    std::string const errTooManyTargets(ClientUser &user, std::string modeTarget, int error_code, std::string abortMessage);
    
    std::string const errNoSuchService(ClientUser &user, std::string service_name);
    std::string const errNoOrigin(ClientUser &user);
    std::string const errNoRecipient(ClientUser &user, std::string command);
    std::string const errNoTextToSend(ClientUser &user);
    std::string const errNoTopLevel(ClientUser &user, std::string mask);
    std::string const errWildTopLevel(ClientUser &user, std::string mask);
    std::string const errBadMask(ClientUser &user, std::string mask);
    std::string const errUnknownCommand(ClientUser &user, std::string command);
    std::string const errNoMotd(ClientUser &user);
    std::string const errNoAdminInfo(ClientUser &user, std::string server);
    std::string const errFileError(ClientUser &user, std::string file_operation, std::string filename);
    std::string const errNoNicknameGiven(ClientUser &user);
    std::string const errErroneousNickname(ClientUser &user, std::string nickname);
    std::string const errNicknameTooLong(ClientUser &user, std::string nickname);
    std::string const errNicknameInUse(ClientUser &user, std::string nickname);
    std::string const errUnavailResource(ClientUser &user, std::string nickname_or_channel);
    std::string const errUserNotInChannel(ClientUser &user, std::string nickname, std::string channel);
    std::string const errNotOnChannel(ClientUser &user, std::string channel);
    std::string const errUserOnChannel(ClientUser &user, std::string username, std::string channel);

    std::string const errNoLogin(ClientUser &user);
    std::string const errSummonDisabled(ClientUser &user);
    std::string const errUsersDisabled(ClientUser &user);
    std::string const errNotRegistered(ClientUser &user);
    std::string const errNeedMoreParams(ClientUser &user, std::string command);
    std::string const errTooManyParams(ClientUser &user, std::string command);
    std::string const errAlreadyRegistered(ClientUser &user);
    std::string const errNoPermForHost(ClientUser &user);
    std::string const errPasswdMismatch(ClientUser &user);
    std::string const errYourBannedCreep(ClientUser &user);
    std::string const errYouWillBeBanned(ClientUser &user);
    std::string const errKeyset(ClientUser &user, std::string channel);
    std::string const errChanneListFull(ClientUser &user, std::string channel);
    std::string const errUnknownMode(ClientUser &user, std::string channel, char modechar);

    std::string const errInviteOnlyChannel(ClientUser &user, std::string channel);
    std::string const errBannedFromchannel(ClientUser &user, std::string channel);
    std::string const errBadChannelKey(ClientUser &user, std::string channel);
    std::string const errBadChannelMask(ClientUser &user, std::string channel);
    std::string const errNoChannelModes(ClientUser &user, std::string channel);
    std::string const errBanListFull(ClientUser &user, std::string channel, std::string modechar);

    std::string const errNoPrivileges(ClientUser &user);
    std::string const errChanOprivsNeeded(ClientUser &user, std::string channel);
    std::string const errCantKillServer(ClientUser &user);
    std::string const errRestricted(ClientUser &user);
    std::string const errUniqOpprivsNeeded(ClientUser &user);
    std::string const errNoOperHost(ClientUser &user);
    std::string const errUserModeUnknownFlag(ClientUser &user);
    std::string const errUsersDontMatch(ClientUser &user);
    std::string const errInvalidUsername();
    std::string const errInvalidModeString();
    std::string const errGeneric(std::string msg);

    // ---- List replies ---- 
    std::string const listStart(ClientUser &user);
    std::string const list(ClientUser &user, Channel &channel);
    std::string const listEnd(ClientUser &user);

    // ---- Kick / Invite ----
    std::string const kick(ClientUser &sender, ClientUser &user, Channel &channel, std::string comment);
    std::string const inviting(ClientUser &user, CommandMessage &message);
    std::string const invite(ClientUser &sender, CommandMessage &message);

    // ---- PING / PONG ----
    std::string const pong(ClientUser &sender, CommandMessage &message);
    std::string const ping(std::string token);

    // ---- Topic ----
    std::string const noTopic(CommandMessage &message, Channel &channel);
    std::string const topic(CommandMessage &message, Channel &channel);
    std::string const newTopic(ClientUser &sender, CommandMessage &message);

    // ---- Mode ----
    std::string const umodeIs(CommandMessage &message, ClientUser &user);
    std::string const channelModeIs(CommandMessage &message, Channel &channel);
    std::string const invalidModeParam(ClientUser &sender, std::string channelName, char mode_char, std::string description);
}

#endif
