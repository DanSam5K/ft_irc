#include "ConnectionManager.hpp"
#include "Channel.hpp"
#include <iterator>
#include <sstream>
#include <stdexcept>
#include "PasswordManager.hpp"
#include "ft_irc.hpp"
#include "utils_logger.hpp"

ConnectionManager::ConnectionManager(Application &_appInstance,
                  PasswordManager &password) : _appInstance(_appInstance),
	_passHandler(password),
	_messHandler(NULL)
{
	logActionUtils::info("ConnectionManager: Creating context");
	_messHandler = new MessageHandler(*this);
	setupChannel(FALLBACK_CHANNEL);
}

ConnectionManager::~ConnectionManager()
{
	logActionUtils::info("ConnectionManager: Terminating context");
	delete (_messHandler);
	delete_map(_pendingUsers);
	delete_map(_activeUsers);
	delete_map(_channels);
}

void ConnectionManager::registerPendingUser(int socket)
{
	User *newUser = new User(*this, socket);
	_pendingUsers.insert(nickNameUserPair (socket, newUser));
}

void ConnectionManager::promoteUserToActive(User &user)
{
	if (user.passwordEnabled() == false || user.has_nickname() == false
	        || user.has_user_info() == false)
	{
		return ;
	}
	_activeUsers.insert(socketUserPair(user.get_nickname(), &user));
	_pendingUsers.erase(user.get_socket());
	user.set_registered();
	joinUserToChannel(user, FALLBACK_CHANNEL);
}

void ConnectionManager::disconnectUser(User &user)
{
	if (user.is_fully_registered())
	{
		this->removeActiveUser(user);
	}
	else
	{
		this->removePendingUser(user);
	}
}

void ConnectionManager::disconnectUserBySocket(int socket)
{
	User user = getUserBySocket(socket);
	disconnectUser(user);
}

void ConnectionManager::removeActiveUser(User &user)
{
	logActionUtils::info("ConnectionManager: Removing registered user");
	std::map<std::string, User *>::iterator it = _activeUsers.find(
	            user.get_nickname());
	if (it != _activeUsers.end())
	{
		removeUserFromAllChannels(*(it->second));
		delete(it->second);
		_activeUsers.erase(it->first);
	}
}

void ConnectionManager::removePendingUser(User &user)
{
	logActionUtils::info("ConnectionManager: Removing unregistered user");
	std::map<int, User *>::iterator it = _pendingUsers.find(
	        user.get_socket());
	if (it != _pendingUsers.end())
	{
		delete(it->second);
		_pendingUsers.erase(it->first);
	}
}

void ConnectionManager::setupChannel(std::string name)
{
	Channel *newChannel = new Channel(name, _passHandler);
	std::string channelName = string_to_lowercase(newChannel->getChannelName());
	_channels.insert(channelNamePair(channelName, newChannel));
}

void ConnectionManager::setupChannelForUser(User &user, const std::string &chanName)
{
	Channel *newChannel = new Channel(chanName, user, _passHandler);
	std::string channelName = string_to_lowercase(newChannel->getChannelName());
	_channels.insert(channelNamePair(channelName, newChannel));
	newChannel->promoteOperatorByUser(user);
}

void ConnectionManager::joinUserToChannel(User &user, const std::string &chanName)
{
	if (checkChannelExist(chanName) == false)
	{
		setupChannelForUser(user, chanName);
	}
	else
	{
		std::string channelName = string_to_lowercase(chanName);
		logActionUtils::info("ConnectionManager: Adding user \"" + user.get_nickname() +
		                 "\" to channel " + channelName);
		_channels[channelName]->addUserToChannel(user);
		if (chanName != FALLBACK_CHANNEL
		        && _channels[FALLBACK_CHANNEL]->confirmInChannelByUser(user))
		{
			_channels[FALLBACK_CHANNEL]->removeUserFromChannel(user);
		}
	}
}

void ConnectionManager::removeUserFromChannel(User &user, const std::string &chanName)
{
	if (checkChannelExist(chanName) == false)
	{
		throw std::out_of_range("ConnectionManager: Remove from channel: no such channel");
	}
	else
	{
		std::string channelName = string_to_lowercase(chanName);
		logActionUtils::info("ConnectionManager: Removing user \"" + user.get_nickname() +
		                 "\" from channel " + channelName);
		_channels[channelName]->removeUserFromChannel(user);
		_channels[channelName]->demoteOperatorByUser(user);
		if (checkUserInAnyChannel(user) == false && channelName != FALLBACK_CHANNEL)
		{
			logActionUtils::info("ConnectionManager: Adding user \"" + user.get_nickname() +
			                 "\" to channel *, because user is no longer in any channel");
			_channels[FALLBACK_CHANNEL]->addUserToChannel(user);
		}
	}
}

void ConnectionManager::removeUserFromAllChannels(User &user)
{
	std::list<Channel *> chans = getUserChannels(user);
	std::list<Channel *>::iterator it = chans.begin();
	for (; it != chans.end(); it++)
	{
		removeUserFromChannel(user, (*it)->getChannelName());
	}
}

bool ConnectionManager::checkUserInAnyChannel(User &user)
{
	std::map<std::string, Channel *>::iterator it = _channels.begin();
	for (; it != _channels.end(); it++)
	{
		if (it->second->confirmInChannelByUser(user) == true)
		{
			return (true);
		}
	}
	return (false);
}

void ConnectionManager::deleteChannel(Channel &channel)
{
	std::string channelName = string_to_lowercase(channel.getChannelName());
	std::map<std::string, Channel *>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
	{
		delete (it->second);
		_channels.erase(it->first);
	}
}

void ConnectionManager::processClientCommand(User &sender, const std::string &rawMessage)
{
	_messHandler->processClientCommand(sender, rawMessage);
}

User &ConnectionManager::getUserBySocket(int socket_fd)
{
	std::map<int, User *>::iterator u_it = _pendingUsers.find(socket_fd);
	if (u_it != _pendingUsers.end())
	{
		return (*_pendingUsers[socket_fd]);
	}
	std::map<std::string, User *>::iterator r_it = _activeUsers.begin();
	for (; r_it != _activeUsers.end(); r_it++)
	{
		if (r_it->second->get_socket() == socket_fd)
		{
			return (*r_it->second);
		}
	}
	std::stringstream ss;
	ss << socket_fd;
	throw UserNotFoundException();
}

User &ConnectionManager::getUserByNickname(const std::string &nickname)
{
	std::map<std::string, User *>::iterator it = _activeUsers.find(nickname);
	if (it != _activeUsers.end())
	{
		return (*_activeUsers[nickname]);
	}
	throw UserNotFoundException();
}

void ConnectionManager::updateUserNickname(User &user, const std::string &newNickname)
{
	std::map<std::string, User *>::iterator it = _activeUsers.find(
	            user.get_nickname());
	if (it != _activeUsers.end())
	{
		std::list<Channel *> chans = getUserChannels(user);
		std::list<Channel *>::iterator it = chans.begin();
		for (; it != chans.end(); it++)
		{
			(*it)->updateUserNickname(user, newNickname);
		}
		_activeUsers.insert(socketUserPair(newNickname, &user));
		_activeUsers.erase(user.get_nickname());
	}
}

bool ConnectionManager::checkUserNicknameExist(const std::string &nickname)
{
	try
	{
		getUserByNickname(nickname);
		return (true);
	}
	catch (std::exception &e)
	{
		return (false);
	}
}

Channel &ConnectionManager::getChannel(const std::string &name)
{
	if (name == FALLBACK_CHANNEL)
	{
		throw std::out_of_range("ConnectionManager: Could not find channel " + name +
		                         " by name");
	}
	std::string channelName = string_to_lowercase(name);
	std::map<std::string, Channel *>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
	{
		return (*_channels[channelName]);
	}
	throw std::out_of_range("ConnectionManager: Could not find channel " + name +
	                         " by name");
}

Channel &ConnectionManager::getDefaultChannel()
{
	return (*_channels[FALLBACK_CHANNEL]);
}

std::list<std::string> ConnectionManager::listAllChannelNames()
{
	std::list<std::string> channelNames;
	std::map<std::string, Channel *>::iterator it = _channels.begin();
	for (; it != _channels.end(); it++)
	{
		if (it->first != FALLBACK_CHANNEL)
		{
			channelNames.push_back(it->second->getChannelName());
		}
	}
	return (channelNames);
}

bool ConnectionManager::checkChannelExist(const std::string &name)
{
	std::string channelName = string_to_lowercase(name);
	std::map<std::string, Channel *>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
	{
		return (true);
	}
	return (false);
}

std::list<Channel *> ConnectionManager::getUserChannels(User &user)
{
	std::list<Channel *> user_chans;
	std::map<std::string, Channel *>::iterator it = _channels.begin();
	for (; it != _channels.end(); it++)
	{
		if (it->second->confirmInChannelByUser(user) == true)
		{
			user_chans.push_back(it->second);
		}
	}
	return (user_chans);
}

std::list<User *> ConnectionManager::getMutualChannelUsers(User &user)
{
	std::list<User *> usersInMutualChannel;
	std::list<Channel *> chans = getUserChannels(user);
	std::list<Channel *>::iterator it = chans.begin();
	for (; it != chans.end(); it++)
	{
		std::list<User *> chan_users = (*it)->getUserList();
		std::list<User *>::iterator uit = chan_users.begin();
		for (; uit != chan_users.end(); uit++)
		{
			if (*uit != &user && is_in_list(usersInMutualChannel, *uit) == false)
			{
				usersInMutualChannel.push_back(*uit);
			}
		}
	}
	return (usersInMutualChannel);
}

void ConnectionManager::printPendingUsers() const
{
	std::map<int, User *>::const_iterator it = _pendingUsers.begin();
	std::map<int, User *>::const_iterator it_end = _pendingUsers.end();
	std::cout << "[INFO] ConnectionManager: Unregistered users :" << std::endl;
	for (; it != it_end; it++)
	{
		std::cout << "\t[" << it->second->get_socket() << "] "
		          << it->second->get_identifier() << std::endl;
	}
}

void ConnectionManager::printActiveUsers() const
{
	std::map<std::string, User *>::const_iterator it = _activeUsers.begin();
	std::map<std::string, User *>::const_iterator it_end = _activeUsers.end();
	std::cout << "[INFO] ConnectionManager: Registered users :" << std::endl;
	for (; it != it_end; it++)
	{
		std::cout << "\t[" << it->second->get_socket() << "] "
		          << it->second->get_identifier() << std::endl;
	}
}

void ConnectionManager::printChannels() const
{
	std::map<std::string, Channel *>::const_iterator it = _channels.begin();
	std::map<std::string, Channel *>::const_iterator it_end = _channels.end();
	std::cout << "[INFO] ConnectionManager: Channels :" << std::endl;
	for (; it != it_end; it++)
	{
		std::cout << "\t[" << it->second->getChannelName() << "] ";
	}
	std::cout << std::endl;
}

void ConnectionManager::verifyConnectionPassword(const std::string &password)
{
	_passHandler.verifyConnectionPassword(password);
}

void ConnectionManager::sendDirectMessage(int socket_fd, const std::string &message)
{
	_appInstance.sendMessageToClient(socket_fd, message);
}

void ConnectionManager::forciblyDisconnect(User &user)
{
	_appInstance.removeClient(user.get_socket());
}

const char* ConnectionManager::UserNotFoundException::what() const throw()
{
	return ("Could not find user by name or socket");
}
