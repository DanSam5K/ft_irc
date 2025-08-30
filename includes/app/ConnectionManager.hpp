/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  						     ConnectionManager.hpp	 					     #
#****************************************************************************/

#ifndef CONNECTION_MANAGER_HPP
# define CONNECTION_MANAGER_HPP

#include "PasswordManager.hpp"
#include "ft_irc.hpp"
#include "CommandMessage.hpp"
#include "MessageHandler.hpp"
#include "reply_message.hpp"
#include "Application.hpp"

# define FALLBACK_CHANNEL "*" // Default channel for users without a specified channel

class Application; // Forward declarations: allows us to use these classes without including their headers.
class ClientUser;
class Channel;

class ConnectionManager
{
	private:
		typedef std::pair<std::string, ClientUser *> socketUserPair; // alias for easier readability
		typedef std::pair<int, ClientUser *> nickNameUserPair; // alias for easier readability
		typedef std::pair<std::string, Channel *> channelNamePair; // alias for easier readability

		std::map<int, ClientUser *> _pendingUsers; // mapping pending user sockets to user objects (socket_fd -> user)
		std::map<std::string, ClientUser *> _activeUsers; // mapping active user nicknames to user objects (nickname -> user)
		std::map<std::string, Channel *> _channels; // mapping channel names to channel objects (channel_name -> channel)

		// References to other managers
		Application &_appInstance;
		PasswordManager &_passHandler;
		MessageHandler *_messHandler;

		// Disallow copying (private). (If copied, it could lead to multiple instances trying to manage the same user session)
		ConnectionManager(const ConnectionManager &src);
		ConnectionManager &operator=(const ConnectionManager &other);

		void removeActiveUser(ClientUser &user); // Remove user from _activeUsers map
		void removePendingUser(ClientUser &user); // Remove user from _pendingUsers map
		void setupChannel(std::string name); // Create a new channel, and insert it on _channels map

	public:
		ConnectionManager(Application &appInstance, PasswordManager &password); // constructor, takes as parameters the application and password managers
		virtual ~ConnectionManager();

		// ClientUser lifecycle
		void registerPendingUser(int socket_fd, const struct sockaddr_in& clientAddr); // Register a new pending user (create user object, add to _pendingUsers map)
		void promoteUserToActive(ClientUser &user); // Promote a pending user to active status (move from _pendingUsers to _activeUsers map)
		void disconnectUser(ClientUser &user); // Disconnect a user (confirm if is pending or active, then remove from appropriate map)
		void disconnectUserBySocket(int socket_fd); // Disconnect a user by their socket (gets user socket_fd, then calls disconnectUser)
		void forciblyDisconnect(ClientUser &user); // Forcefully disconnect a user (remove from all maps without confirmation)

		// Channel Management
		void setupChannelForUser(ClientUser &user, std::string channelName); // Create a new channel and add user to it, promote user to operator
		void deleteChannel(Channel &channel); // Remove a channel and all its users (delete <Channel> object, remove from _channels map)
		void updateUserNickname(ClientUser &user, std::string newNickname); // Update a user's nickname (update in _activeUsers map, and in all channels the user is part of)

		// IRC command dispatcher
		void processClientCommand(ClientUser &sender, std::string rawMessage); // Process a command received from a client (parses the command and calls the appropriate handler).

		// ClientUser Accessors
		ClientUser &getUserBySocket(int socket_fd); // Get a user by their socket (searches both _pendingUsers and _activeUsers maps)
		ClientUser &getUserByNickname(std::string nickname); // Get a user by their nickname (searches _activeUsers map)
		bool checkUserNicknameExist(std::string nickname); // Check if a user with the given nickname exists

		// Channel Accessors
		Channel &getChannel(const std::string &name); // Get a channel by its name (searches _channels map)
		Channel &getDefaultChannel(); // Get the default fallback channel (used for users not in any specific channel)
		std::list<std::string> listAllChannelNames(); // List all existing channel names (returns a list of keys from _channels map)

		// ClientUser <-> Channel Relations
		void joinUserToChannel(ClientUser &user, std::string channelName); // Add a user to a channel (creates channel if it doesn't exist, adds user to channel's user list)
		void removeUserFromChannel(ClientUser &user, std::string channelName); // Remove a user from a channel (removes user from channel's user list, deletes channel if empty)
		void removeUserFromAllChannels(ClientUser &user); // Remove a user from all channels they are part of

		bool checkChannelExist(std::string name); // Check if a channel with the given name exists
		bool checkUserInAnyChannel(ClientUser &user); // Check if a user is in any channel

		std::list<Channel *> getUserChannels(ClientUser &user); // List with all channels a user is part of
		std::list<ClientUser *> getMutualChannelUsers(ClientUser &user); // List with all users in the same channels as the given user

		// Security
		void verifyConnectionPassword(std::string password); // Verify the password provided by a connecting user (calls PasswordManager to validate)

		// Messaging
		void sendDirectMessage(int socket_fd, std::string message); // Send a direct message to a specific user (finds user by socket, sends message)

		// printing for debugging purposes
		void printPendingUsers() const; // Print all pending users
		void printActiveUsers() const; // Print all active users
		void printChannels() const; // Print all channels

		// Exception
		class UserNotFoundException : public std::exception // Exception thrown when a user is not found
		{
			public:
				virtual const char* what() const throw();
		};
};

#endif
