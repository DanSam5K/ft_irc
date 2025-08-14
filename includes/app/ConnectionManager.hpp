#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include "PasswordManager.hpp"
#include "ft_irc.hpp"
#include "CommandMessage.hpp"
#include "MessageHandler.hpp"
#include "reply_message.hpp"
#include "Application.hpp"

# define FALLBACK_CHANNEL "*"

class Application;
class ClientUser;
class Channel;

class ConnectionManager
{
	private:
		typedef std::pair<std::string, ClientUser *> socketUserPair;
		typedef std::pair<int, ClientUser *> nickNameUserPair;
		typedef std::pair<std::string, Channel *> channelNamePair;

		std::map<int, ClientUser *> _pendingUsers;
		std::map<std::string, ClientUser *> _activeUsers;
		std::map<std::string, Channel *> _channels;

		Application &_appInstance;
		PasswordManager &_passHandler;
		MessageHandler *_messHandler;

		// Disallow copying
		ConnectionManager(const ConnectionManager &src);
		ConnectionManager &operator=(const ConnectionManager &other);

		void removeActiveUser(ClientUser &user);
		void removePendingUser(ClientUser &user);
		void setupChannel(std::string name);

	public:
		ConnectionManager(Application &appInstance, PasswordManager &password);
		virtual ~ConnectionManager();

		// ClientUser lifecycle
		void registerPendingUser(int socket_fd);
		void promoteUserToActive(ClientUser &user);
		void disconnectUser(ClientUser &user);
		void disconnectUserBySocket(int socket_fd);
		void forciblyDisconnect(ClientUser &user);

		// Channel Management
		void setupChannelForUser(ClientUser &user, std::string channelName);
		void deleteChannel(Channel &channel);
		void updateUserNickname(ClientUser &user, std::string newNickname);

		// IRC command dispatcher
		void processClientCommand(ClientUser &sender, std::string rawMessage);

		// Accessors
		ClientUser &getUserBySocket(int socket_fd);
		ClientUser &getUserByNickname(std::string nickname);
		bool checkUserNicknameExist(std::string nickname);

		Channel &getChannel(const std::string &name);
		Channel &getDefaultChannel();
		std::list<std::string> listAllChannelNames();

		// ClientUser <-> Channel Relations
		void joinUserToChannel(ClientUser &user, std::string channelName);
		void removeUserFromChannel(ClientUser &user, std::string channelName);
		void removeUserFromAllChannels(ClientUser &user);

		bool checkChannelExist(std::string name);
		bool checkUserInAnyChannel(ClientUser &user);

		std::list<Channel *> getUserChannels(ClientUser &user);
		std::list<ClientUser *> getMutualChannelUsers(ClientUser &user);

		// Security
		void verifyConnectionPassword(std::string password);

		// Messaging
		void sendDirectMessage(int socket_fd, std::string message);

		// printing for debugging purposes
		void printPendingUsers() const;
		void printActiveUsers() const;
		void printChannels() const;

		// Exception
		class UserNotFoundException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
};

#endif
