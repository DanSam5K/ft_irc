#include "ClientUser.hpp"
#include "ConnectionManager.hpp"
#include "utils_logger.hpp"
#include <cstddef>
#include <stdexcept>

/* ClientUser::ClientUser() {} */

ClientUser::ClientUser(ConnectionManager &context, int socket) : _nickname("*"), _username("*"),
	_hostname("*"), _realname("*"), _isRegistered(false),
	_validPassword(false), _serverContext(context),
	_socketFd(socket)
{
	refreshIdentifier();
}

ClientUser::~ClientUser()
{
	logActionUtils::info("ClientUser: closing socket for user \"" + this->_nickname + "\": socket", this->_socketFd);
	close(this->_socketFd);
}

std::string const &ClientUser::getNickname() const
{
	return (this->_nickname);
}

std::string const &ClientUser::getUsername() const
{
	return (this->_username);
}

std::string const &ClientUser::getRealname() const
{
	return (this->_realname);
}

std::string const &ClientUser::getHostname() const
{
	return (this->_hostname);
}

std::string const &ClientUser::getIdentifier() const
{
	return (this->_fullIdentifier);
}

bool ClientUser::checkUserOperator() const
{
	return (confirmModePresence('o'));
}

int const &ClientUser::getSocket() const
{
	return (this->_socketFd);
}

std::string const &ClientUser::getModeFlags() const
{
	return (this->_modeFlags);
}

bool ClientUser::passwordEnabled() const
{
	return (this->_validPassword);
}

void ClientUser::setNickname(const std::string &nickname)
{
	if (nickname_is_valid(nickname))
	{
		if (this->confirmFullyRegistered() == true)
		{
			_serverContext.updateUserNickname(*this, nickname);
		}
		this->_nickname = nickname;
		this->refreshIdentifier();
	}
	else
	{
		throw InvalidNicknameException();
	}
}

void ClientUser::setUsername(const std::string &username)
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

void ClientUser::setRealname(const std::string &realname)
{
	this->_realname = realname;
}

void ClientUser::setHostname(const std::string &hostname)
{
	this->_hostname = hostname;
	this->refreshIdentifier();
}

void ClientUser::setRegistered()
{
	if (this->_isRegistered == true)
	{
		throw std::runtime_error("ClientUser: Attempting to reset already set user registered flag !");
	}
	this->_isRegistered = true;
}

void ClientUser::authorizePassword()
{
	if (this->_validPassword == true)
	{
		throw std::runtime_error("ClientUser: Correct password already supplied.");
	}
	this->_validPassword = true;
}

void ClientUser::configureModes(const std::string &modesToAdd, const std::string &modesToRemove)
{
	addModeFlags(modesToAdd);
	removeModeFlags(modesToRemove);
}


void ClientUser::addModeFlags(std::string modeString)
{
	std::string::iterator it = modeString.begin();
	for (; it != modeString.end(); it++)
	{
		size_t pos = this->_modeFlags.find(*it, 0);
		if (pos == std::string::npos)
		{
			this->_modeFlags += *it;
		}
	}
}

void ClientUser::removeModeFlags(std::string modeString)
{
	std::string::iterator it = modeString.begin();
	for (; it != modeString.end(); it++)
	{
		size_t pos = this->_modeFlags.find(*it, 0);
		if (pos != std::string::npos)
		{
			this->_modeFlags.erase(pos, 1);
		}
	}
}

bool ClientUser::confirmModePresence(char flag) const
{
	size_t pos = this->_modeFlags.find(flag, 0);
	if (pos != std::string::npos)
	{
		return (true);
	}
	return (false);
}

void ClientUser::refreshIdentifier()
{
	this->_fullIdentifier = this->_nickname + "!" + this->_username + "@" + this->_hostname;
}

bool ClientUser::confirmFullyRegistered() const
{
	return (this->_isRegistered);
}

void ClientUser::handleSocketInput()
{
	std::string rawMessage;
	/* TODO: read from socket and store received string in rawMessage */
	/* ConnectionManager will create and handle the message */
	_serverContext.processClientCommand(*this, rawMessage);
}

void ClientUser::userBroadcast(std::string message)
{
	_serverContext.sendDirectMessage(_socketFd, message);
	logActionUtils::reply(_socketFd, message);
}

bool ClientUser::confirmNicknameExist()
{
	if (this->_nickname != "*")
	{
		return (true);
	}
	return (false);
}

bool ClientUser::checkAllUserDetails()
{
	if (this->_username != "*" && this->_hostname != "*"
	        && this->_realname != "*")
	{
		return (true);
	}
	return (false);
}

bool ClientUser::checkIsValidUsername(const std::string &username)
{
	const std::string accepted_chars = "@._";

	if (std::isdigit(username[0]))
	{
		return (false);
	}
	for (size_t i = 0; i < username.size(); i++)
	{
		if (!std::isalnum(username[i]) && !confirmPresence(username[i], accepted_chars))
		{
			return (false);
		}
	}
	return (true);
}

bool ClientUser::nickname_is_valid(const std::string &nickname)
{
	const std::string accepted_chars = "_\\^|[]{}`";

	if (nickname.size() == 0 || nickname.size() > 9)
	{
		throw NicknameTooLongException();
	}
	if (std::isdigit(nickname[0]))
	{
		return (false);
	}
	for (size_t i = 0; i < nickname.size(); i++)
	{
		if (!std::isalnum(nickname[i]) && !confirmPresence(nickname[i], accepted_chars))
		{
			return (false);
		}
	}
	return (true);
}

std::ostream &operator<<(std::ostream &os, ClientUser const &obj)
{
	os << "[" << obj.getSocket() << "][" << obj.getIdentifier() << "]";
	return (os);
}
