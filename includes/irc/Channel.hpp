/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  						         Channel.hpp    	 					     #
#****************************************************************************/

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "ClientUser.hpp"
#include "ft_irc.hpp"
#include "PasswordManager.hpp"
#include <set> // For std::set, (std::vector but without duplicates and alphabetically ordered, fast search)

#define MAX_CHANNEL_NAME_LENGTH 50 // Maximum length for channel names

class Channel
{
	private:
		typedef std::pair<std::string, ClientUser *> userNicknamePair; // alias for user nickname and user pointer

		std::string channelName; 
		std::map<std::string, ClientUser *> usersRegistry; // User registry: maps nicknames to ClientUser pointers

		std::string modeFlags; // Channel mode flags ('i' for invite-only, 't' for topic-protected, 'k' for password-protected, 'o' for operator-only, 'l' for user limit)
		std::string creatorNickname; // Nickname of the channel creator

		std::string topicMessage; // Current topic message for the channel

		std::set<std::string> operatorList; 	  // Set of operator nicknames (ensures uniqueness and allows fast lookup)
		std::set<std::string> guestUserNicknames; // Set of guest user nicknames (ensures uniqueness and allows fast lookup)

		bool blockedTopic; // Indicates if the topic is blocked
		bool guestUsersOnly; // Indicates if only guest users are allowed
		bool passwordEnabled; // Indicates if password protection is enabled
		bool usersLimitEnabled; // Indicates if user limit is enabled

		size_t maximumUsers; // Maximum number of users allowed in the channel
		PasswordManager &passwordHandler; // Reference to the password manager for handling channel passwords
		std::string channelPassword; // Channel password (if set)


	public:
		// Constructors &Destructor
		Channel(std::string channelName, PasswordManager &passwordHandler);
		Channel(std::string channelName, ClientUser &creator, PasswordManager &passwordHandler);
		virtual ~Channel();

		// Accessors
		std::string getTopicMessage() const; // Get the current topic message
		std::string const &getChannelCreator() const; // Get the nickname of the channel creator
		std::string const &getModeFlags() const; // Get the channel mode flags
		std::string const &getChannelName() const; // Get the channel name
		std::string getModeString() const; // Get the channel mode as a string
		std::string getUserListString(); // Get the list of users in the channel as a string
		unsigned int getUserCount() const; // Get the number of users in the channel
		std::list<ClientUser *> getUserList(); // Get the list of users in the channel

		// Modifiers
		void setChannelName(std::string channelName); // Set the channel name
		void setChannelCreator(std::string nickname); // Set the channel creator
		void addModeFlags(std::string modeString); // Add mode flags
		void setTopicMessage(std::string newTopic); // Set the topic message
		void configureModes(std::string modesToUpdate, std::string modesToDiscard); // Configure channel modes
		void removeModeFlags(std::string mode_string); // Remove mode flags

		void setTopicLock(bool enabled); // Set topic lock
		void setInviteOnly(bool enabled); // Set invite-only mode
		void setUserLimit(int limit); // Set user limit
		void removeUserRestriction(); // Remove user restriction
		void setPassword(std::string password); // Set channel password
		void removePassword(); // Remove channel password
		void addUserToChannel(ClientUser &user); // Add user to channel
		void removeUserFromChannel(ClientUser &user); // Remove user from channel
		void updateUserNickname(ClientUser &user, std::string newNickname); // Update user nickname


		// Invites
		void inviteUser(std::string nickname); // Invite user
		void revokeInvite(std::string nickname); // Revoke invite
		void clearInvites(); // Clear all invites

		// Operator Controls
		void demoteOperatorByUser(ClientUser &user); // Demote operator by user
		void promoteOperatorByUser(ClientUser &user); // Promote operator by user
		void promoteOperatorByNickname(std::string nickname); // Promote operator by nickname
		void demoteOperatorByNickname(std::string nickname); // Demote operator by nickname
		void transferOperatorToNextUser(); // Transfer operator to next user

		// Checks
		bool checkTopicRestricted() const; // Check if topic is restricted
		bool checkInviteToChannelOnly() const; // Check if invite-only mode is enabled
		bool checkPasswordProtection() const; // Check if password protection is enabled
		bool checkPassword(std::string password) const; // Check if password is correct
		bool checkUserRestriction() const; // Check if user restriction is enabled
		bool checkRestrictionPoint() const; // Check if restriction point is reached

		bool checkInvitedByUser(ClientUser &user) const; // Check if invited by user
		bool checkInvitedByNickname(std::string nickname) const; // Check if invited by nickname

		bool confirmModePresence(char c) const; // Confirm mode presence
		bool checkChannelOperatorByUser(ClientUser &user) const; // Check if user is channel operator
		bool checkChannelCreatorByUser(ClientUser &user) const; // Check if user is channel creator
		bool checkChannelOperatorByNickname(std::string nickname) const; // Check if nickname is channel operator
		bool checkChannelCreatorByNickname(std::string nickname) const; // Check if nickname is channel creator
		bool confirmInChannelByUser(ClientUser &user); // Confirm user is in channel
		bool confirmInChannelByNickname(std::string nickname); // Confirm nickname is in channel
		bool confirmChannelIsEmpty(); // Confirm channel is empty

		void broadcast(std::string message); // Broadcast message to all users
		void broadcastExcept(std::string message, ClientUser &excludedUser); // Broadcast message to all users except excludedUser


		class AlreadyInChannelException: public std::exception // Exception thrown when a user is already in the channel
		{
			public:
				virtual const char* what() const throw();
		};
		class InvalidChannelNameException: public std::exception // Exception thrown when an invalid channel name is provided
		{
			public:
				virtual const char* what() const throw();
		};
};

#endif
