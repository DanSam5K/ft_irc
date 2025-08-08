#ifndef USER_HPP
#define USER_HPP

#include "ft_irc.hpp"

#define MAX_NICKNAME_LENGHT 19

class ConnectionManager;

class ClientUser
{
	private:
		std::string _nickname;
		std::string _username;
		std::string _hostname;
		std::string _realname;
		std::string _fullIdentifier;
		std::string _modeFlags;

		bool _isRegistered;
		bool _validPassword;
		ConnectionManager &_serverContext;
		int	_socketFd;

		/* ClientUser(); */
		void refreshIdentifier();

		bool checkIsValidUsername(const std::string &username);
		bool nickname_is_valid(const std::string &nickname);

	public:
		ClientUser(ConnectionManager &context, int socket);
		virtual ~ClientUser();

		std::string const &getNickname() const;
		std::string const &getUsername() const;
		std::string const &getRealname() const;
		std::string const &getHostname() const;
		std::string const &getIdentifier() const;
		std::string const &getModeFlags() const;
		
		bool passwordEnabled() const;
		int const &getSocket() const;
		bool confirmFullyRegistered() const;

		void setNickname(const std::string &nickname);
		void setUsername(const std::string &username);
		void setRealname(const std::string &realname);
		void setHostname(const std::string &hostname);
		void authorizePassword();
		void configureModes(const std::string &modesToAdd, const std::string &modesToremove);
		void addModeFlags(std::string modeString);
		void removeModeFlags(std::string modeString);
		bool confirmModePresence(char flag) const ;
		bool checkUserOperator() const;

		void setRegistered();

		void handleSocketInput();
		void userBroadcast(std::string message);
		bool confirmNicknameExist();
		bool checkAllUserDetails();

		class InvalidUsernameException : public std::exception {};
		class InvalidNicknameException : public std::exception {};
		class NicknameTooLongException : public std::exception {};
};

std::ostream &operator<<(std::ostream &os, ClientUser const &obj);

#endif /* USER_HPP */
