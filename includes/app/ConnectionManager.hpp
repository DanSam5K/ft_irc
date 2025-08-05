#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include "Password.hpp"
#include "ft_irc.hpp"
#include "Message.hpp"
#include "MessageHandler.hpp"
#include "reply.hpp"
#include "Application.hpp"

# define FALLBACK_CHANNEL "*"

class Application;
class User;
class Channel;

class ConnectionManager
{
	private:
		typedef std::pair<std::string, User *> socketUserPair;
		typedef std::pair<int, User *> nickNameUserPair;
		typedef std::pair<std::string, Channel *> channelNamePair;

		std::map<int, User *> _pendingUsers;
		std::map<std::string, User *> _activeUsers;
		std::map<std::string, Channel *> _channels;

		Application & _appInstance;
		Password & _passHandler;
		MessageHandler * _messHandler;

		// Disallow copying
		ConnectionManager( const ConnectionManager &src );
		ConnectionManager & operator=( const ConnectionManager &other );

		void removeActiveUser( User &user );
		void removePendingUser( User &user );
		void setupChannel( std::string name );

	public:
		ConnectionManager( Application & _appInstance, Password & _passHandler );
		virtual ~ConnectionManager();

		// User lifecycle
		void registerPendingUser( int socket_fd );
		void promoteUserToActive( User &user );
		void disconnectUser( User &user );
		void disconnectUserBySocket( int socket_fd );
		void forciblyDisconnect( User &user );

		// Channel Management
		void setupChannelForUser( User &user, const std::string &chanName );
		void deleteChannel( Channel &channel );
		void updateUserNickname( User &user, const std::string &newNickname );

		// IRC command dispatcher
		void processClientCommand( User &sender, const std::string &rawMessage );

		// Accessors
		User &getUserBySocket( int socket_fd );
		User &getUserByNickname( const std::string &nickname );
		bool checkUserNicknameExist( const std::string &nickname );

		Channel &getChannel( const std::string &name );
		Channel &getDefaultChannel();
		std::list<std::string> listAllChannelNames();

		// User <-> Channel Relations
		void joinUserToChannel( User &user, const std::string &channel_name );
		void removeUserFromChannel( User &user, const std::string &channel_name );
		void removeUserFromAllChannels( User &user );

		bool checkChannelExist( const std::string &name );
		bool checkUserInAnyChannel( User &user );

		std::list<Channel *> getUserChannels( User &user );
		std::list<User *> getMutualChannelUsers( User &user );

		// Security
		void verifyConnectionPassword( const std::string &password );

		// Messaging
		void sendDirectMessage( int socket_fd, const std::string &message );

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
