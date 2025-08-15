#include "ConnectionManager.hpp"
#include "Channel.hpp"
#include <iterator>
#include <sstream>
#include <stdexcept>
#include "PasswordManager.hpp"
#include "ft_irc.hpp"
#include "utils_logger.hpp"

ConnectionManager::ConnectionManager(Application &appInstance,
                  PasswordManager &password) : _appInstance(appInstance),
	_passHandler(password),
	_messHandler(NULL)
{
	// logActionUtils::info("ConnectionManager: Creating connection");
	logActionUtils::info("ConnectionManager initialized");
	_messHandler = new MessageHandler(*this);
	setupChannel(FALLBACK_CHANNEL);
}

ConnectionManager::~ConnectionManager()
{
	// logActionUtils::info("ConnectionManager: Terminating connection");
	logActionUtils::info("ConnectionManager terminated");
	delete (_messHandler);
	delete_map(_pendingUsers);
	delete_map(_activeUsers);
	delete_map(_channels);
}

void ConnectionManager::registerPendingUser(int socket, const struct sockaddr_in& clientAddr)
{
	ClientUser *newUser = new ClientUser(*this, socket);
	
	// Set hostname from client address
	char hostname[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(clientAddr.sin_addr), hostname, INET_ADDRSTRLEN);
	newUser->setHostname(hostname);
	
	_pendingUsers.insert(nickNameUserPair (socket, newUser));
}

void ConnectionManager::promoteUserToActive(ClientUser &user)
{
	if (user.passwordEnabled() == false || user.confirmNicknameExist() == false
	        || user.checkAllUserDetails() == false)
	{
		return ;
	}
	_activeUsers.insert(socketUserPair(user.getNickname(), &user));
	_pendingUsers.erase(user.getSocket());
	user.setRegistered();
	joinUserToChannel(user, FALLBACK_CHANNEL);
}

void ConnectionManager::disconnectUser(ClientUser &user)
{
	if (user.confirmFullyRegistered())
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
	ClientUser &user = getUserBySocket(socket);
	disconnectUser(user);
}

void ConnectionManager::removeActiveUser(ClientUser &user)
{
	// logActionUtils::info("ConnectionManager: Removing registered user");
	logActionUtils::info("Registered user removed from the server");
	std::map<std::string, ClientUser *>::iterator it = _activeUsers.find(
	            user.getNickname());
	if (it != _activeUsers.end())
	{
		removeUserFromAllChannels(*(it->second));
		delete(it->second);
		_activeUsers.erase(it->first);
	}
}

void ConnectionManager::removePendingUser(ClientUser &user)
{
	// logActionUtils::info("ConnectionManager: Removing unregistered user");
	logActionUtils::info("Unregistered user removed from the server");
	std::map<int, ClientUser *>::iterator it = _pendingUsers.find(
	        user.getSocket());
	if (it != _pendingUsers.end())
	{
		delete(it->second);
		_pendingUsers.erase(it->first);
	}
}

void ConnectionManager::setupChannel(std::string name)
{
	Channel *newChannel = new Channel(name, _passHandler);
	std::string channelName = stringToLowercase(newChannel->getChannelName());
	_channels.insert(channelNamePair(channelName, newChannel));
}

void ConnectionManager::setupChannelForUser(ClientUser &user, std::string channelName)
{
	Channel *newChannel = new Channel(channelName, user, _passHandler);
	std::string lowercaseChannelName = stringToLowercase(newChannel->getChannelName());
	_channels.insert(channelNamePair(lowercaseChannelName, newChannel));
	newChannel->promoteOperatorByUser(user);
}

void ConnectionManager::joinUserToChannel(ClientUser &user, std::string channelName)
{
	if (checkChannelExist(channelName) == false)
	{
		setupChannelForUser(user, channelName);
	}
	else
	{
		std::string lowerChannelName = stringToLowercase(channelName);
		// logActionUtils::info("ConnectionManager: Adding user \"" + user.getNickname() +
		//                  "\" to channel " + lowerChannelName);
		logActionUtils::info("User \"" + user.getNickname() + "\" added to channel " + lowerChannelName);
		_channels[lowerChannelName]->addUserToChannel(user);
		if (lowerChannelName != FALLBACK_CHANNEL
		        && _channels[FALLBACK_CHANNEL]->confirmInChannelByUser(user))
		{
			_channels[FALLBACK_CHANNEL]->removeUserFromChannel(user);
		}
	}
}

void ConnectionManager::removeUserFromChannel(ClientUser &user, std::string channelName)
{
	if (checkChannelExist(channelName) == false)
	{
		throw std::out_of_range("Channel removal failed: channel does not exist");
		// throw std::out_of_range("ConnectionManager: Remove from channel: no such channel");
	}
	else
	{
		std::string lowerChannelName = stringToLowercase(channelName);
		// logActionUtils::info("ConnectionManager: Removing user \"" + user.getNickname() +
		//                  "\" from channel " + lowerChannelName);
		logActionUtils::info("User \"" + user.getNickname() + "\" removed from channel " + lowerChannelName);

		_channels[lowerChannelName]->removeUserFromChannel(user);
		_channels[lowerChannelName]->demoteOperatorByUser(user);
		if (checkUserInAnyChannel(user) == false && lowerChannelName != FALLBACK_CHANNEL)
		{
			// logActionUtils::info("ConnectionManager: Adding user \"" + user.getNickname() +
			//                  "\" to channel *, because user is no longer in any channel");
			logActionUtils::info("User \"" + user.getNickname() +
				"\" has been added to the fallback channel since they are not present in any other channel.");
			_channels[FALLBACK_CHANNEL]->addUserToChannel(user);
		}
	}
}

void ConnectionManager::removeUserFromAllChannels(ClientUser &user)
{
	std::list<Channel *> chans = getUserChannels(user);
	std::list<Channel *>::iterator it = chans.begin();
	for (; it != chans.end(); it++)
	{
		removeUserFromChannel(user, (*it)->getChannelName());
	}
}

bool ConnectionManager::checkUserInAnyChannel(ClientUser &user)
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
	std::string channelName = stringToLowercase(channel.getChannelName());
	std::map<std::string, Channel *>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
	{
		delete (it->second);
		_channels.erase(it->first);
	}
}

void ConnectionManager::processClientCommand(ClientUser &sender, std::string rawMessage)
{
	_messHandler->processClientCommand(sender, rawMessage);
}

ClientUser &ConnectionManager::getUserBySocket(int socket_fd)
{
	std::map<int, ClientUser *>::iterator u_it = _pendingUsers.find(socket_fd);
	if (u_it != _pendingUsers.end())
	{
		return (*_pendingUsers[socket_fd]);
	}
	std::map<std::string, ClientUser *>::iterator r_it = _activeUsers.begin();
	for (; r_it != _activeUsers.end(); r_it++)
	{
		if (r_it->second->getSocket() == socket_fd)
		{
			return (*r_it->second);
		}
	}
	std::stringstream ss;
	ss << socket_fd;
	throw UserNotFoundException();
}

ClientUser &ConnectionManager::getUserByNickname(std::string nickname)
{
	std::map<std::string, ClientUser *>::iterator it = _activeUsers.find(nickname);
	if (it != _activeUsers.end())
	{
		return (*_activeUsers[nickname]);
	}
	throw UserNotFoundException();
}

void ConnectionManager::updateUserNickname(ClientUser &user, std::string newNickname)
{
	std::map<std::string, ClientUser *>::iterator it = _activeUsers.find(
	            user.getNickname());
	if (it != _activeUsers.end())
	{
		std::list<Channel *> chans = getUserChannels(user);
		std::list<Channel *>::iterator it = chans.begin();
		for (; it != chans.end(); it++)
		{
			(*it)->updateUserNickname(user, newNickname);
		}
		_activeUsers.insert(socketUserPair(newNickname, &user));
		_activeUsers.erase(user.getNickname());
	}
}

bool ConnectionManager::checkUserNicknameExist(std::string nickname)
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
		throw std::out_of_range("Channel not found: " + name);
	}
	std::string channelName = stringToLowercase(name);
	std::map<std::string, Channel *>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
	{
		return (*_channels[channelName]);
	}
	throw std::out_of_range("Channel lookup failed: \"" + name +
	                         "\" not found");
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

bool ConnectionManager::checkChannelExist(std::string name)
{
	std::string channelName = stringToLowercase(name);
	std::map<std::string, Channel *>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
	{
		return (true);
	}
	return (false);
}

std::list<Channel *> ConnectionManager::getUserChannels(ClientUser &user)
{
	std::list<Channel *> userChannels;
	std::map<std::string, Channel *>::iterator it = _channels.begin();
	for (; it != _channels.end(); it++)
	{
		if (it->second->confirmInChannelByUser(user) == true)
		{
			userChannels.push_back(it->second);
		}
	}
	return (userChannels);
}

std::list<ClientUser *> ConnectionManager::getMutualChannelUsers(ClientUser &user)
{
	std::list<ClientUser *> usersInMutualChannel;
	std::list<Channel *> chans = getUserChannels(user);
	std::list<Channel *>::iterator it = chans.begin();
	for (; it != chans.end(); it++)
	{
		std::list<ClientUser *> channelUsers = (*it)->getUserList();
		std::list<ClientUser *>::iterator uit = channelUsers.begin();
		for (; uit != channelUsers.end(); uit++)
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
	std::map<int, ClientUser *>::const_iterator it = _pendingUsers.begin();
	std::map<int, ClientUser *>::const_iterator it_end = _pendingUsers.end();
	std::cout << "[STATUS] Users pending registration:" << std::endl;
	for (; it != it_end; it++)
	{
		std::cout << "\t[" << it->second->getSocket() << "] "
		          << it->second->getIdentifier() << std::endl;
	}
}

void ConnectionManager::printActiveUsers() const
{
	std::map<std::string, ClientUser *>::const_iterator it = _activeUsers.begin();
	std::map<std::string, ClientUser *>::const_iterator it_end = _activeUsers.end();
	std::cout << "[STATUS] list of currently Registered users :" << std::endl;
	for (; it != it_end; it++)
	{
		std::cout << "\t[" << it->second->getSocket() << "] "
		          << it->second->getIdentifier() << std::endl;
	}
}

void ConnectionManager::printChannels() const
{
	std::map<std::string, Channel *>::const_iterator it = _channels.begin();
	std::map<std::string, Channel *>::const_iterator it_end = _channels.end();
	std::cout << "[STATUS] Current channel list:" << std::endl;
	for (; it != it_end; it++)
	{
		std::cout << "\t[" << it->second->getChannelName() << "] ";
	}
	std::cout << std::endl;
}

void ConnectionManager::verifyConnectionPassword(std::string password)
{
	_passHandler.verifyConnectionPassword(password);
}

void ConnectionManager::sendDirectMessage(int socket_fd, std::string message)
{
	_appInstance.sendMessageToClient(socket_fd, message);
}

void ConnectionManager::forciblyDisconnect(ClientUser &user)
{
	_appInstance.removeClient(user.getSocket());
}

const char* ConnectionManager::UserNotFoundException::what() const throw()
{
	 // Returns a message indicating that no user was found for the given identifier.
    return "User lookup failed: no matching name or socket found";
}
