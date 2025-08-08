#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "User.hpp"
#include "ft_irc.hpp"
#include "PasswordManager.hpp"
#include <set>

#define MAX_CHANNEL_NAME_LENGTH 50

class Channel
{
	private:
		typedef std::pair<std::string, User *> userNicknamePair;
		// typedef std::pair<std::string, std::string> pair_nick_mode;

		std::string channelName;
		std::map<std::string, User *> usersRegistry;

		std::string modeFlags;
		std::string creatorNickname;

		std::string topicMessage;

		std::set<std::string> operatorList;
		std::set<std::string> guestUserNicknames;

		bool blockedTopic;
		bool guestUsersOnly;
		bool passwordEnabled;
		bool usersLimitEnabled;

		size_t maximumUsers;
		PasswordManager &passwordHandler;
		std::string channelPassword;


	public:
		// Constructors &Destructor
		Channel(std::string channelName, PasswordManager &passwordHandler);
		Channel(std::string channelName, User &creator, PasswordManager &passwordHandler);
		virtual ~Channel();

		// Accessors
		std::string getTopicMessage() const;
		std::string const &getChannelCreator() const;
		std::string const &getModeFlags() const;
		std::string const &getChannelName() const;
		std::string getModeString() const;
		std::string getUserListString();
		unsigned int getUserCount();
		std::list<User *> getUserList();
		
		// Modifiers
		void setChannelName(std::string channelName);
		void setChannelCreator(std::string nickname);
		void addModeFlags(std::string modeString);
		void setTopicMessage(std::string newTopic);
		void configureModes(std::string modesToUpdate, std::string modesToDiscard);
		void removeModeFlags(std::string mode_string);
		
		void setTopicLock(bool enabled);
		void setInviteOnly(bool enabled);
		void setUserLimit(int limit);
		void removeUserRestriction();
		void setPassword(std::string password);
		void removePassword();
		void addUserToChannel(User &user);
		void removeUserFromChannel(User &user);
		void updateUserNickname(User &user, std::string newNickname);


		// Invites
		void inviteUser(std::string nickname);
		void revokeInvite(std::string nickname);
		void clearInvites();

		
		// Operator Controls
		void demoteOperatorByUser(User &user);
		void promoteOperatorByUser(User &user);
		void promoteOperatorByNickname(std::string nickname);
		void demoteOperatorByNickname(std::string nickname);

		// Checks
		bool checkTopicRestricted() const;
		bool checkInviteToChannelOnly() const;
		bool checkPasswordProtection() const;
		bool checkPassword(std::string password) const;
		bool checkUserRestriction() const;
		bool checkRestrictionPoint() const;

		bool checkInvitedByUser(User &user) const;
		bool checkInvitedByNickname(std::string nickname) const;

		bool confirmModePresence(char c);
		bool checkChannelOperatorByUser(User &user);
		bool checkChannelCreatorByUser(User &user);
		bool checkChannelOperatorByNickname(std::string nickname);
		bool checkChannelCreatorByNickname(std::string nickname);
		bool confirmInChannelByUser(User &user);
		bool confirmInChannelByNickname(std::string nickname);
		bool confirmChannelIsEmpty();

		void broadcast(std::string message);
		void broadcastExcept(std::string message, User &excludedUser);


		class AlreadyInChannelException: public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
		class InvalidChannelNameException: public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
};

#endif
