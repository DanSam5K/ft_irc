/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  						     ConnectionManager.cpp 	 					     #
#****************************************************************************/

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
	logActionUtils::info("ConnectionManager initialized");
	_messHandler = new MessageHandler(*this);
	setupChannel(FALLBACK_CHANNEL);
}

ConnectionManager::~ConnectionManager()
{
	logActionUtils::info("ConnectionManager terminated");
	delete (_messHandler);
	delete_map(_pendingUsers);
	delete_map(_activeUsers);
	delete_map(_channels);
}

void ConnectionManager::registerPendingUser(int socket, const struct sockaddr_in& clientAddr) // create a new ClientUser, set hostname from client address, insert into pending users
{
	ClientUser *newUser = new ClientUser(*this, socket);
	
	// Set hostname from client address
	char hostname[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(clientAddr.sin_addr), hostname, INET_ADDRSTRLEN);
	newUser->setHostname(hostname);
	
	_pendingUsers.insert(nickNameUserPair (socket, newUser));
}

void ConnectionManager::promoteUserToActive(ClientUser &user) // Promote a pending user to active status
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

void ConnectionManager::disconnectUser(ClientUser &user) // Disconnect a user from the server
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

void ConnectionManager::disconnectUserBySocket(int socket) // Disconnect a user by their socket
{
	ClientUser &user = getUserBySocket(socket);
	disconnectUser(user);
}

void ConnectionManager::removeActiveUser(ClientUser &user) // Remove an active user from the server
{
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

void ConnectionManager::removePendingUser(ClientUser &user) // Remove a pending user from the server
{
	logActionUtils::info("Unregistered user removed from the server");
	std::map<int, ClientUser *>::iterator it = _pendingUsers.find(
	        user.getSocket());
	if (it != _pendingUsers.end())
	{
		delete(it->second);
		_pendingUsers.erase(it->first);
	}
}

void ConnectionManager::setupChannel(std::string name) // Create a new channel
{
	Channel *newChannel = new Channel(name, _passHandler);
	std::string channelName = stringToLowercase(newChannel->getChannelName());
	_channels.insert(channelNamePair(channelName, newChannel));
}

void ConnectionManager::setupChannelForUser(ClientUser &user, std::string channelName) // Create a new channel for a user
{
	Channel *newChannel = new Channel(channelName, user, _passHandler);
	std::string lowercaseChannelName = stringToLowercase(newChannel->getChannelName());
	_channels.insert(channelNamePair(lowercaseChannelName, newChannel));
	newChannel->promoteOperatorByUser(user);
}

void ConnectionManager::joinUserToChannel(ClientUser &user, std::string channelName) // Join a user to a channel
{
	if (checkChannelExist(channelName) == false)
	{
		setupChannelForUser(user, channelName);
	}
	else
	{
		std::string lowerChannelName = stringToLowercase(channelName);
		logActionUtils::info("User \"" + user.getNickname() + "\" added to channel " + lowerChannelName);
		_channels[lowerChannelName]->addUserToChannel(user);
		if (lowerChannelName != FALLBACK_CHANNEL
		        && _channels[FALLBACK_CHANNEL]->confirmInChannelByUser(user))
		{
			_channels[FALLBACK_CHANNEL]->removeUserFromChannel(user);
		}
	}
}

void ConnectionManager::removeUserFromChannel(ClientUser &user, std::string channelName) // Remove a user from a channel
{
	if (checkChannelExist(channelName) == false)
	{
		throw std::out_of_range("Channel removal failed: channel does not exist");
	}
	else
	{
		std::string lowerChannelName = stringToLowercase(channelName);
		logActionUtils::info("User \"" + user.getNickname() + "\" removed from channel " + lowerChannelName);

		_channels[lowerChannelName]->removeUserFromChannel(user);
		_channels[lowerChannelName]->demoteOperatorByUser(user);
		if (checkUserInAnyChannel(user) == false && lowerChannelName != FALLBACK_CHANNEL)
		{
			logActionUtils::info("User \"" + user.getNickname() +
				"\" has been added to the fallback channel since they are not present in any other channel.");
			_channels[FALLBACK_CHANNEL]->addUserToChannel(user);
		}
	}
}

void ConnectionManager::removeUserFromAllChannels(ClientUser &user) // Remove a user from all channels
{
	std::list<Channel *> chans = getUserChannels(user);
	std::list<Channel *>::iterator it = chans.begin();
	for (; it != chans.end(); it++)
	{
		removeUserFromChannel(user, (*it)->getChannelName());
	}
}

bool ConnectionManager::checkUserInAnyChannel(ClientUser &user) // Check if a user is in any channel.
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

void ConnectionManager::deleteChannel(Channel &channel) // remove a channel from the server
{
	std::string channelName = stringToLowercase(channel.getChannelName());
	std::map<std::string, Channel *>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
	{
		delete (it->second);
		_channels.erase(it->first);
	}
}

void ConnectionManager::processClientCommand(ClientUser &sender, std::string rawMessage) // Process a command from a client.
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

ClientUser &ConnectionManager::getUserByNickname(std::string nickname) // Get a user by their nickname
{
	std::map<std::string, ClientUser *>::iterator it = _activeUsers.find(nickname);
	if (it != _activeUsers.end())
	{
		return (*_activeUsers[nickname]);
	}
	throw UserNotFoundException();
}

void ConnectionManager::updateUserNickname(ClientUser &user, std::string newNickname) // Update a user's nickname
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

bool ConnectionManager::checkUserNicknameExist(std::string nickname) // Check if a user nickname exists
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

Channel &ConnectionManager::getChannel(const std::string &name) // Get a channel by its name
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

Channel &ConnectionManager::getDefaultChannel() // Get the default channel
{
	return (*_channels[FALLBACK_CHANNEL]);
}

std::list<std::string> ConnectionManager::listAllChannelNames() // List all channel names
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

bool ConnectionManager::checkChannelExist(std::string name) // Check if a channel exists
{
	std::string channelName = stringToLowercase(name);
	std::map<std::string, Channel *>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
	{
		return (true);
	}
	return (false);
}

std::list<Channel *> ConnectionManager::getUserChannels(ClientUser &user) // Get all channels a user is in
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

std::list<ClientUser *> ConnectionManager::getMutualChannelUsers(ClientUser &user) // Get all users in the same channels as the given user
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

void ConnectionManager::printPendingUsers() const // Print all pending users
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

void ConnectionManager::printActiveUsers() const // Print all active users
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

void ConnectionManager::printChannels() const // Print all channels
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

void ConnectionManager::verifyConnectionPassword(std::string password) // Verify the connection password
{
	_passHandler.verifyConnectionPassword(password);
}

void ConnectionManager::sendDirectMessage(int socket_fd, std::string message) // Send a direct message to a user
{
	_appInstance.sendMessageToClient(socket_fd, message);
}

void ConnectionManager::forciblyDisconnect(ClientUser &user) // Forcibly disconnect a user
{
	_appInstance.removeClient(user.getSocket());
}

const char* ConnectionManager::UserNotFoundException::what() const throw() 	 // Returns a message indicating that no user was found for the given identifier.
{
    return "User lookup failed: no matching name or socket found";
}
