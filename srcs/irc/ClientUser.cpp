#include "ClientUser.hpp"
#include "ConnectionManager.hpp"
#include "utils_logger.hpp"
#include <cstddef>
#include <stdexcept>


// Initializes a new IRC user with default values and associates with a connection manager
ClientUser::ClientUser(ConnectionManager &context, int socket)
    : _nickname("*"), _username("*"), _hostname("*"), _realname("*"),
      _isRegistered(false), _validPassword(false), _serverContext(context),
      _socketFd(socket)
{
    refreshIdentifier();
}

// Cleans up the user object and closes the associated socket
ClientUser::~ClientUser()
{
    logActionUtils::info("ClientUser: closing socket for user \"" + this->_nickname + "\": socket", this->_socketFd);
    close(this->_socketFd);
}

// Returns the user's nickname
std::string const &ClientUser::getNickname() const { return (this->_nickname); }

// Returns the user's username
std::string const &ClientUser::getUsername() const { return (this->_username); }

// Returns the user's real name
std::string const &ClientUser::getRealname() const { return (this->_realname); }

// Returns the user's hostname
std::string const &ClientUser::getHostname() const { return (this->_hostname); }

// Returns the user's full IRC identifier (nick!user@host)
std::string const &ClientUser::getIdentifier() const { return (this->_fullIdentifier); }

// Checks if the user has operator privileges
bool ClientUser::checkUserOperator() const { return (confirmModePresence('o')); }

// Returns the user's socket file descriptor
int const &ClientUser::getSocket() const { return (this->_socketFd); }

// Returns the user's mode flags
std::string const &ClientUser::getModeFlags() const { return (this->_modeFlags); }

// Checks if the user has successfully authenticated with a password
bool ClientUser::passwordEnabled() const { return (this->_validPassword); }

// Sets the user's nickname, updating registration and identifier as needed
void ClientUser::setNickname(std::string nickname)
{
    if (checkValidNickname(nickname))
    {
        if (this->confirmFullyRegistered())
            _serverContext.updateUserNickname(*this, nickname);
        this->_nickname = nickname;
        this->refreshIdentifier();
    }
    else
    {
        throw InvalidNicknameException();
    }
}

// Sets the user's username, truncating if necessary and updating identifier
void ClientUser::setUsername(std::string username)
{
    if (checkIsValidUsername(username))
    {
        this->_username = username.substr(0, MAX_NICKNAME_LENGHT);
        this->refreshIdentifier();
    }
    else
    {
        throw InvalidUsernameException();
    }
}

// Sets the user's real name
void ClientUser::setRealname(std::string realname) { this->_realname = realname; }

// Sets the user's hostname and updates identifier
void ClientUser::setHostname(std::string hostname)
{
    this->_hostname = hostname;
    this->refreshIdentifier();
}

// Marks the user as registered; throws if already registered
void ClientUser::setRegistered()
{
    if (this->_isRegistered)
        throw std::runtime_error("User registration flag is already set; cannot reset.");
    this->_isRegistered = true;
}

// Marks the user as having supplied the correct password; throws if already set
void ClientUser::authorizePassword()
{
    if (this->_validPassword)
        throw std::runtime_error("Password has already been validated for this user.");
    this->_validPassword = true;
}

// Updates the user's mode flags by adding and removing specified flags
void ClientUser::configureModes(std::string modesToAdd, std::string modesToRemove)
{
    addModeFlags(modesToAdd);
    removeModeFlags(modesToRemove);
}

// Adds mode flags to the user if not already present
void ClientUser::addModeFlags(std::string modeString)
{
    for (std::string::iterator it = modeString.begin(); it != modeString.end(); ++it)
    {
        if (this->_modeFlags.find(*it) == std::string::npos)
            this->_modeFlags += *it;
    }
}

// Removes specified mode flags from the user
void ClientUser::removeModeFlags(std::string modeString)
{
    for (std::string::iterator it = modeString.begin(); it != modeString.end(); ++it)
    {
        size_t pos = this->_modeFlags.find(*it);
        if (pos != std::string::npos)
            this->_modeFlags.erase(pos, 1);
    }
}

// Checks if a specific mode flag is set for the user
bool ClientUser::confirmModePresence(char flag) const
{
    return (this->_modeFlags.find(flag) != std::string::npos);
}

// Updates the user's full IRC identifier string
void ClientUser::refreshIdentifier()
{
    this->_fullIdentifier = this->_nickname + "!" + this->_username + "@" + this->_hostname;
}

// Returns true if the user has completed registration
bool ClientUser::confirmFullyRegistered() const { return (this->_isRegistered); }

// Handles incoming data from the user's socket (to be implemented)
void ClientUser::handleSocketInput()
{
    std::string rawMessage;
    /* TODO: read from socket and store received string in rawMessage */
    /* ConnectionManager will create and handle the message */
    _serverContext.processClientCommand(*this, rawMessage);
}

// Sends a message to all users sharing a channel with this user
void ClientUser::userBroadcast(std::string message)
{
    _serverContext.sendDirectMessage(_socketFd, message);
    logActionUtils::reply(_socketFd, message);
}

// Checks if the user's nickname is set to a valid value
bool ClientUser::confirmNicknameExist()
{
    return (this->_nickname != "*");
}

// Checks if all required user details are set
bool ClientUser::checkAllUserDetails()
{
    return (this->_username != "*" && this->_hostname != "*" && this->_realname != "*");
}

// Validates the username according to allowed characters and rules
bool ClientUser::checkIsValidUsername(std::string username)
{
    const std::string accepted_chars = "@._";
    if (std::isdigit(username[0]))
        return (false);
    for (size_t i = 0; i < username.size(); i++)
    {
        if (!std::isalnum(username[i]) && !confirmPresence(username[i], accepted_chars))
            return (false);
    }
    return (true);
}

// Validates the nickname according to allowed characters and rules
bool ClientUser::checkValidNickname(std::string nickname)
{
    const std::string accepted_chars = "_\\^|[]{}`";
    if (nickname.size() == 0 || nickname.size() > 9)
        throw NicknameTooLongException();
    if (std::isdigit(nickname[0]))
        return (false);
    for (size_t i = 0; i < nickname.size(); i++)
    {
        if (!std::isalnum(nickname[i]) && !confirmPresence(nickname[i], accepted_chars))
            return (false);
    }
    return (true);
}

// Allows easy printing of user details to output streams
std::ostream &operator<<(std::ostream &os, ClientUser const &obj)
{
    os << "[" << obj.getSocket() << "][" << obj.getIdentifier() << "]";
    return (os);
}









// ClientUser::ClientUser(ConnectionManager &context, int socket) : _nickname("*"), _username("*"),
// 	_hostname("*"), _realname("*"), _isRegistered(false),
// 	_validPassword(false), _serverContext(context),
// 	_socketFd(socket)
// {
// 	refreshIdentifier();
// }

// ClientUser::~ClientUser()
// {
// 	logActionUtils::info("ClientUser: closing socket for user \"" + this->_nickname + "\": socket", this->_socketFd);
// 	close(this->_socketFd);
// }

// std::string const &ClientUser::getNickname() const
// {
// 	return (this->_nickname);
// }

// std::string const &ClientUser::getUsername() const
// {
// 	return (this->_username);
// }

// std::string const &ClientUser::getRealname() const
// {
// 	return (this->_realname);
// }

// std::string const &ClientUser::getHostname() const
// {
// 	return (this->_hostname);
// }

// std::string const &ClientUser::getIdentifier() const
// {
// 	return (this->_fullIdentifier);
// }

// bool ClientUser::checkUserOperator() const
// {
// 	return (confirmModePresence('o'));
// }

// int const &ClientUser::getSocket() const
// {
// 	return (this->_socketFd);
// }

// std::string const &ClientUser::getModeFlags() const
// {
// 	return (this->_modeFlags);
// }

// bool ClientUser::passwordEnabled() const
// {
// 	return (this->_validPassword);
// }

// void ClientUser::setNickname(std::string nickname)
// {
// 	if (checkValidNickname(nickname))
// 	{
// 		if (this->confirmFullyRegistered() == true)
// 		{
// 			_serverContext.updateUserNickname(*this, nickname);
// 		}
// 		this->_nickname = nickname;
// 		this->refreshIdentifier();
// 	}
// 	else
// 	{
// 		throw InvalidNicknameException();
// 	}
// }

// void ClientUser::setUsername(std::string username)
// {
// 	if (checkIsValidUsername(username))
// 	{
// 		this->_username = username.substr(0, MAX_NICKNAME_LENGHT);
// 		this->refreshIdentifier();
// 	}
// 	else
// 	{
// 		throw InvalidUsernameException();
// 	}
// }

// void ClientUser::setRealname(std::string realname)
// {
// 	this->_realname = realname;
// }

// void ClientUser::setHostname(std::string hostname)
// {
// 	this->_hostname = hostname;
// 	this->refreshIdentifier();
// }

// void ClientUser::setRegistered()
// {
// 	if (this->_isRegistered == true)
// 	{
// 		throw std::runtime_error("ClientUser: Attempting to reset already set user registered flag !");
// 	}
// 	this->_isRegistered = true;
// }

// void ClientUser::authorizePassword()
// {
// 	if (this->_validPassword == true)
// 	{
// 		throw std::runtime_error("ClientUser: Correct password already supplied.");
// 	}
// 	this->_validPassword = true;
// }

// void ClientUser::configureModes(std::string modesToAdd, std::string modesToRemove)
// {
// 	addModeFlags(modesToAdd);
// 	removeModeFlags(modesToRemove);
// }


// void ClientUser::addModeFlags(std::string modeString)
// {
// 	std::string::iterator it = modeString.begin();
// 	for (; it != modeString.end(); it++)
// 	{
// 		size_t pos = this->_modeFlags.find(*it, 0);
// 		if (pos == std::string::npos)
// 		{
// 			this->_modeFlags += *it;
// 		}
// 	}
// }

// void ClientUser::removeModeFlags(std::string modeString)
// {
// 	std::string::iterator it = modeString.begin();
// 	for (; it != modeString.end(); it++)
// 	{
// 		size_t pos = this->_modeFlags.find(*it, 0);
// 		if (pos != std::string::npos)
// 		{
// 			this->_modeFlags.erase(pos, 1);
// 		}
// 	}
// }

// bool ClientUser::confirmModePresence(char flag) const
// {
// 	size_t pos = this->_modeFlags.find(flag, 0);
// 	if (pos != std::string::npos)
// 	{
// 		return (true);
// 	}
// 	return (false);
// }

// void ClientUser::refreshIdentifier()
// {
// 	this->_fullIdentifier = this->_nickname + "!" + this->_username + "@" + this->_hostname;
// }

// bool ClientUser::confirmFullyRegistered() const
// {
// 	return (this->_isRegistered);
// }

// void ClientUser::handleSocketInput()
// {
// 	std::string rawMessage;
// 	/* TODO: read from socket and store received string in rawMessage */
// 	/* ConnectionManager will create and handle the message */
// 	_serverContext.processClientCommand(*this, rawMessage);
// }

// void ClientUser::userBroadcast(std::string message)
// {
// 	_serverContext.sendDirectMessage(_socketFd, message);
// 	logActionUtils::reply(_socketFd, message);
// }

// bool ClientUser::confirmNicknameExist()
// {
// 	if (this->_nickname != "*")
// 	{
// 		return (true);
// 	}
// 	return (false);
// }

// bool ClientUser::checkAllUserDetails()
// {
// 	if (this->_username != "*" && this->_hostname != "*"
// 	        && this->_realname != "*")
// 	{
// 		return (true);
// 	}
// 	return (false);
// }

// bool ClientUser::checkIsValidUsername(std::string username)
// {
// 	const std::string accepted_chars = "@._";

// 	if (std::isdigit(username[0]))
// 	{
// 		return (false);
// 	}
// 	for (size_t i = 0; i < username.size(); i++)
// 	{
// 		if (!std::isalnum(username[i]) && !confirmPresence(username[i], accepted_chars))
// 		{
// 			return (false);
// 		}
// 	}
// 	return (true);
// }

// bool ClientUser::checkValidNickname(std::string nickname)
// {
// 	const std::string accepted_chars = "_\\^|[]{}`";

// 	if (nickname.size() == 0 || nickname.size() > 9)
// 	{
// 		throw NicknameTooLongException();
// 	}
// 	if (std::isdigit(nickname[0]))
// 	{
// 		return (false);
// 	}
// 	for (size_t i = 0; i < nickname.size(); i++)
// 	{
// 		if (!std::isalnum(nickname[i]) && !confirmPresence(nickname[i], accepted_chars))
// 		{
// 			return (false);
// 		}
// 	}
// 	return (true);
// }

// std::ostream &operator<<(std::ostream &os, ClientUser const &obj)
// {
// 	os << "[" << obj.getSocket() << "][" << obj.getIdentifier() << "]";
// 	return (os);
// }
