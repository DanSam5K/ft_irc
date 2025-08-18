#include "Channel.hpp"
#include "ConnectionManager.hpp"
#include "PasswordManager.hpp"
#include "ClientUser.hpp"
#include "utils_logger.hpp"
#include <cctype>
#include <stdexcept>

Channel::Channel(std::string channelName,
                  PasswordManager &passwordHandler) : blockedTopic(false),
	guestUsersOnly(false), passwordEnabled(false), usersLimitEnabled(false),
	passwordHandler(passwordHandler)
{
	if (channelName == FALLBACK_CHANNEL)
	{
		this->channelName = FALLBACK_CHANNEL;
		return ;
	}
	setChannelName(channelName);
	logActionUtils::info("Initialized new channel", this->channelName);

}

Channel::Channel(std::string channelName,
                  ClientUser &creator, PasswordManager &passwordHandler) : blockedTopic(false),
	guestUsersOnly(false),
	passwordEnabled(false), usersLimitEnabled(false),
	passwordHandler(passwordHandler)
{
	setChannelName(channelName);
	setChannelCreator(creator.getNickname());
	addUserToChannel(creator);
	logActionUtils::info("ClientUser \"" + this->creatorNickname + "\" checkCommandArgument initialized channel",
					 this->channelName);

}

Channel::~Channel() {}

void Channel::setTopicMessage(std::string new_topic)
{
	topicMessage = new_topic;
}

std::string Channel::getTopicMessage() const
{
	return (topicMessage);
}

bool is_chan_name_valid(std::string channelName)
{
	if (channelName.length() > MAX_CHANNEL_NAME_LENGTH)
	{
		return (false);
	}
	std::string required_first_char = "&#+!";
	std::string forbidden_chars = " ,:";
	size_t fist_char_check = channelName.find_first_of(required_first_char, 0);
	size_t forbbidden_check = channelName.find_first_of(forbidden_chars, 0);
	size_t ctrl_g_check = channelName.find(7, 0);
	if (fist_char_check == 0 && forbbidden_check == std::string::npos
	        && ctrl_g_check == std::string::npos)
	{
		return (true);
	}
	return (false);
}

void Channel::setChannelName(std::string channelName)
{
	if (is_chan_name_valid(channelName) == true)
	{
		this->channelName = channelName;
		return ;
	}
	throw InvalidChannelNameException();
}

std::string const &Channel::getChannelName() const
{
	return (this->channelName);
}

void Channel::setChannelCreator(std::string nick)
{
	this->creatorNickname = nick;
	return ;
}

std::string const &Channel::getChannelCreator() const
{
	return (this->creatorNickname);
}

std::string const &Channel::getModeFlags() const
{
	return (this->modeFlags);
}

void Channel::addUserToChannel(ClientUser &user)
{
	logActionUtils::info("Channel \"" + this->channelName + "\": Adding user \"" 
						+ user.getNickname() + "\" to the channel");
	if (confirmInChannelByUser(user) == true)
	{
		throw Channel::AlreadyInChannelException();
	}
	usersRegistry.insert(userNicknamePair(user.getNickname(), &user));
	if (user.getNickname() == this->creatorNickname)
	{
		promoteOperatorByUser(user);
	}
}

void Channel::removeUserFromChannel(ClientUser &user)
{
	logActionUtils::info("Channel \"" + this->channelName + "\": ClientUser \"" + 
					 user.getNickname() + "\" has been removed");
	
	bool wasOperator = checkChannelOperatorByUser(user);
	bool wasCreator = checkChannelCreatorByUser(user);
	
	usersRegistry.erase(user.getNickname());
	operatorList.erase(user.getNickname());
	
	// If the user was an operator and there are still users in the channel
	// but no operators left, transfer operator status to the first user
	if ((wasOperator || wasCreator) && !usersRegistry.empty() && operatorList.empty())
	{
		transferOperatorToNextUser();
	}
}

void Channel::updateUserNickname(ClientUser &user, std::string new_nick)
{
	std::map<std::string, ClientUser *>::iterator it = usersRegistry.find(user.getNickname());
	bool is_chan_creator = checkChannelCreatorByUser(user);
	if (it != usersRegistry.end())
	{
		usersRegistry.insert(userNicknamePair(new_nick, &user));
		usersRegistry.erase(user.getNickname());
		if (is_chan_creator == true)
		{
			setChannelCreator(new_nick);
		}
	}
	int is_op = operatorList.erase(user.getNickname());
	if (is_op)
	{
		operatorList.insert(new_nick);
	}
}

void Channel::configureModes(std::string modes_to_add, std::string modes_to_remove)
{
	addModeFlags(modes_to_add);
	removeModeFlags(modes_to_remove);
}

void Channel::addModeFlags(std::string mode_string)
{
	std::string::iterator it = mode_string.begin();
	for (; it != mode_string.end(); it++)
	{
		size_t pos = this->modeFlags.find(*it, 0);
		if (pos == std::string::npos)
		{
			this->modeFlags += *it;
		}
	}
}

void Channel::removeModeFlags(std::string mode_string)
{
	std::string::iterator it = mode_string.begin();
	for (; it != mode_string.end(); it++)
	{
		size_t pos = this->modeFlags.find(*it, 0);
		if (pos != std::string::npos)
		{
			this->modeFlags.erase(pos, 1);
		}
	}
}


bool Channel::confirmModePresence(char c)
{
	size_t pos = this->modeFlags.find(c, 0);
	if (pos != std::string::npos)
	{
		return (true);
	}
	return (false);
}

void Channel::promoteOperatorByUser(ClientUser &user)
{
	operatorList.insert(user.getNickname());
}

void Channel::demoteOperatorByUser(ClientUser &user)
{
	operatorList.erase(user.getNickname());
}

void Channel::demoteOperatorByNickname(std::string nick)
{
	operatorList.erase(nick);
}


bool Channel::checkChannelOperatorByUser(ClientUser &user)
{
	return (operatorList.count(user.getNickname()) || checkChannelCreatorByUser(user));
}

bool Channel::checkChannelCreatorByUser(ClientUser &user)
{
	if (user.getNickname() == this->creatorNickname)
	{
		return (true);
	}
	return (false);
}

void Channel::promoteOperatorByNickname(std::string nickname)
{
	operatorList.insert(nickname);
}

void Channel::inviteUser(std::string nick)
{
	guestUserNicknames.insert(nick);
}

void Channel::revokeInvite(std::string nick)
{
	guestUserNicknames.erase(nick);
}

void Channel::clearInvites()
{
	guestUserNicknames.clear();
}

bool Channel::checkChannelOperatorByNickname(std::string nickname)
{
	return (operatorList.count(nickname) || checkChannelCreatorByNickname(nickname));
}

bool Channel::checkChannelCreatorByNickname(std::string nickname)
{
	if (nickname == this->creatorNickname)
	{
		return (true);
	}
	return (false);
}

void Channel::broadcast(std::string message)
{
	std::map<std::string, ClientUser *>::iterator it = usersRegistry.begin();
	for (; it != usersRegistry.end(); it++)
	{
		it->second->userBroadcast(message);
	}
}

void Channel::broadcastExcept(std::string message, ClientUser &excludedUser)
{
	std::map<std::string, ClientUser *>::iterator it = usersRegistry.begin();
	for (; it != usersRegistry.end(); it++)
	{
		if (it->first != excludedUser.getNickname())
		{
			it->second->userBroadcast(message);
		}
	}
}

bool Channel::confirmInChannelByNickname(std::string nickname)
{
	std::map<std::string, ClientUser *>::iterator it = usersRegistry.find(nickname);
	if (it != usersRegistry.end())
	{
		return (true);
	}
	return (false);
}

bool Channel::confirmInChannelByUser(ClientUser &user)
{
	std::map<std::string, ClientUser *>::iterator it = usersRegistry.find(user.getNickname());
	if (it != usersRegistry.end())
	{
		return (true);
	}
	return (false);
}

bool Channel::confirmChannelIsEmpty()
{
	std::map<std::string, ClientUser *>::iterator it = usersRegistry.begin();
	if (it == usersRegistry.end())
	{
		return (true);
	}
	return (false);
}

std::list<ClientUser *> Channel::getUserList()
{
	std::list<ClientUser *> user_list;
	std::map<std::string, ClientUser *>::iterator it = usersRegistry.begin();
	for (; it != usersRegistry.end(); it++)
	{
		user_list.push_back(it->second);
	}
	return (user_list);
}

unsigned int Channel::getUserCount()
{
	return (usersRegistry.size());
}


std::string Channel::getUserListString()
{
	std::string user_list;
	std::map<std::string, ClientUser *>::iterator it = usersRegistry.begin();
	for (; it != usersRegistry.end(); it++)
	{
		if (it != usersRegistry.begin())
		{
			user_list += " ";
		}
		if (checkChannelOperatorByNickname(it->first))
		{
			user_list += "@";
		}
		user_list += it->first;
	}
	return (user_list);
}

void Channel::setTopicLock(bool enabled)
{
	blockedTopic = enabled;
}

bool Channel::checkTopicRestricted() const
{
	return blockedTopic;
}

void Channel::setInviteOnly(bool enabled)
{
	guestUsersOnly = enabled;
}

bool Channel::checkInviteToChannelOnly() const
{
	return guestUsersOnly;
}

void Channel::setPassword(std::string password)
{
	channelPassword = passwordHandler.generateHash(password);
	passwordEnabled = true;
}

void Channel::removePassword()
{
	channelPassword = "";
	passwordEnabled = false;
}

bool Channel::checkPasswordProtection() const
{
	return passwordEnabled;
}

bool Channel::checkPassword(std::string password) const
{
	if (!passwordEnabled)
	{
		return true;
	}
	try
	{
		passwordHandler.verifyPassword(channelPassword, password);
		return (true);
	}
	catch (PasswordManager::InvalidPasswordException &e)
	{
		return (false);
	}
}

void Channel::setUserLimit(int limit)
{
	maximumUsers = limit;
	usersLimitEnabled = true;
}

void Channel::removeUserRestriction()
{
	usersLimitEnabled = false;
}

bool Channel::checkUserRestriction() const
{
	return usersLimitEnabled;
}

bool Channel::checkRestrictionPoint() const
{
	if (usersLimitEnabled && usersRegistry.size() >= maximumUsers)
	{
		return (true);
	}
	return (false);
}


bool Channel::checkInvitedByUser(ClientUser &user) const
{
	return guestUserNicknames.count(user.getNickname());
}

bool Channel::checkInvitedByNickname(std::string nickname) const
{
	return guestUserNicknames.count(nickname);
}

const char* Channel::AlreadyInChannelException::what() const throw()
{
	return ("ClientUser is already a member of the channel.");
}

const char* Channel::InvalidChannelNameException::what() const throw()
{
	return ("ChannelName is not a recognized channel");
}

std::string Channel::getModeString() const
{
	std::string rawModeString = "+";

	if (usersLimitEnabled)
	{
		rawModeString += "l";
	}
	if (guestUsersOnly)
	{
		rawModeString += "i";
	}
	if (passwordEnabled)
	{
		rawModeString += "k";
	}
	if (blockedTopic)
	{
		rawModeString += "t";
	}
	return rawModeString;
}

void Channel::transferOperatorToNextUser()
{
	if (usersRegistry.empty())
	{
		return;
	}
	
	// Find the first user in the channel and make them an operator
	std::map<std::string, ClientUser *>::iterator it = usersRegistry.begin();
	if (it != usersRegistry.end())
	{
		promoteOperatorByUser(*(it->second));
		logActionUtils::info("Channel \"" + this->channelName + "\": Operator status transferred to \"" + 
							it->first + "\"");
		
		// Broadcast the operator change to all users in the channel
		broadcast(rpl_msg::modeChannel(*(it->second), *this, "+o " + it->first));
	}
}
