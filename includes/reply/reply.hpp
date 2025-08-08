#ifndef REPLY_HPP
#define REPLY_HPP

#include "reply_macros.hpp"
#include "error_reply.hpp"
#include "ft_irc.hpp"
#include "ClientUser.hpp"
#include "CommandMessage.hpp"
#include "Channel.hpp"

namespace rpl
{
std::string const welcome(ClientUser &user);
std::string const yourhost(ClientUser &user);
std::string const created(ClientUser &user);
std::string const myinfo(ClientUser &user);
std::string const server_version(ClientUser &user);
std::string const adminme(ClientUser &user);
std::string const adminloc1(ClientUser &user);
std::string const adminloc2(ClientUser &user);
std::string const adminemail(ClientUser &user);
std::string const info(ClientUser &user, int line);
std::string const info_end(ClientUser &user);
std::string const confirmation(std::string const identifier,
                                CommandMessage &message);
std::string const join_channel(ClientUser &user, Channel &channel);
std::string const mode_channel(ClientUser &user, Channel &channel,
                                std::string modeString);
std::string const namreply(ClientUser &user, Channel &channel);
std::string const endofnames(ClientUser &user, std::string chan_name);
std::string const part(ClientUser &user, Channel &channel, CommandMessage &message);
std::string const create_part_message(Channel &channel);
std::string const quit(ClientUser &user, CommandMessage &message);

std::string const forward(ClientUser &sender, CommandMessage &message);

std::string const err_nosuchnick(ClientUser &user, std::string name_or_channel);
std::string const err_nosuchnick(ClientUser &user, std::string name_or_channel);
std::string const err_nosuchserver(ClientUser &user, std::string server_name);
std::string const err_nosuchchannel(ClientUser &user, std::string channel_name);
std::string const err_cannotsendtochan(ClientUser &user, std::string channel_name);
std::string const err_toomanychannels(ClientUser &user, std::string channel_name);
std::string const err_wasnosuchnick(ClientUser &user, std::string nickname);
std::string const err_toomanytargets(ClientUser &user, std::string modeTarget,
                                      int error_code,
                                      std::string abort_message);
std::string const err_nosuchservice(ClientUser &user, std::string service_name);
std::string const err_noorigin(ClientUser &user);
std::string const err_norecipient(ClientUser &user, std::string command);
std::string const err_notexttosend(ClientUser &user);
std::string const err_notoplevel(ClientUser &user, std::string mask);
std::string const err_wildtoplevel(ClientUser &user, std::string mask);
std::string const err_badmask(ClientUser &user, std::string mask);
std::string const err_unknowncommand(ClientUser &user, std::string command);
std::string const err_nomotd(ClientUser &user);
std::string const err_noadmininfo(ClientUser &user, std::string server);
std::string const err_fileerror(ClientUser &user, std::string file_operation,
                                 std::string filename);
std::string const err_nonicknamegiven(ClientUser &user);
std::string const err_erroneusnickname(ClientUser &user, std::string nickname);
std::string const err_nicknametoolong(ClientUser &user, std::string nickname);
std::string const err_nicknameinuse(ClientUser &user, std::string nickname);
std::string const err_unavailresource(ClientUser &user,
                                       std::string nickname_or_channel);
std::string const err_usernotinchannel(ClientUser &user, std::string nickname,
                                        std::string channel);
std::string const err_notonchannel(ClientUser &user, std::string channel);
std::string const err_useronchannel(ClientUser &user, std::string username,
                                     std::string channel);
std::string const err_nologin(ClientUser &user);
std::string const err_summondisabled(ClientUser &user);
std::string const err_usersdisabled(ClientUser &user);
std::string const err_notregistered(ClientUser &user);
std::string const err_needmoreparams(ClientUser &user, std::string command);
std::string const err_toomanyparams(ClientUser &user, std::string command);
std::string const err_alreadyregistred(ClientUser &user);
std::string const err_nopermforhost(ClientUser &user);
std::string const err_passwdmismatch(ClientUser &user);
std::string const err_yourebannedcreep(ClientUser &user);
std::string const err_youwillbebanned(ClientUser &user);
std::string const err_keyset(ClientUser &user, std::string channel);
std::string const err_channelisfull(ClientUser &user, std::string channel);
std::string const err_unknownmode(ClientUser &user, std::string channel,
                                   char modechar);
std::string const err_inviteonlychan(ClientUser &user, std::string channel);
std::string const err_bannedfromchan(ClientUser &user, std::string channel);
std::string const err_badchannelkey(ClientUser &user, std::string channel);
std::string const err_badchanmask(ClientUser &user, std::string channel);
std::string const err_nochanmodes(ClientUser &user, std::string channel);
std::string const err_banlistfull(ClientUser &user, std::string channel,
                                   std::string modechar);
std::string const err_noprivileges(ClientUser &user);
std::string const err_chanoprivsneeded(ClientUser &user, std::string channel);
std::string const err_cantkillserver(ClientUser &user);
std::string const err_restricted(ClientUser &user);
std::string const err_uniqopprivsneeded(ClientUser &user);
std::string const err_nooperhost(ClientUser &user);
std::string const err_umodeunknownflag(ClientUser &user);
std::string const err_usersdontmatch(ClientUser &user);
std::string const err_invalidusername();
std::string const err_invalidmodestring();
std::string const err_generic(std::string msg);
std::string const liststart(ClientUser &user);
std::string const list(ClientUser &user, Channel &channel);
std::string const listend(ClientUser &user);
std::string const kick(ClientUser &sender, ClientUser &user, Channel &channel,
                        std::string comment);
std::string const inviting(ClientUser &user, CommandMessage &message);
std::string const invite(ClientUser &sender, CommandMessage &message);
std::string const pong(ClientUser &sender, CommandMessage &message);
std::string const notopic(CommandMessage &message, Channel &channel);
std::string const topic(CommandMessage &message, Channel &channel);
std::string const umodeis(CommandMessage &message, ClientUser &user);
std::string const channelmodeis(CommandMessage &message, Channel &channel);
std::string const newtopic(ClientUser &sender, CommandMessage &message);
std::string const ping(std::string token);
std::string const invalidmodeparam(ClientUser &sender, std::string channel_name,
                                    char mode_char, std::string description);
}

#endif
