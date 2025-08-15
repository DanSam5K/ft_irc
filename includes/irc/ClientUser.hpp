#ifndef CLIENT_USER_HPP
#define CLIENT_USER_HPP

#include "ft_irc.hpp"

// Maximum allowed length for a user's nickname
#define MAX_NICKNAME_LENGHT 19

class ConnectionManager;

// Represents a connected IRC client and manages its state and identity
class ClientUser
{
    private:
        std::string _nickname;         // User's nickname
        std::string _username;         // Username provided by the client
        std::string _hostname;         // Hostname associated with the client
        std::string _realname;         // Real name field from IRC registration
        std::string _fullIdentifier;   // Full IRC identifier (nick!user@host)
        std::string _modeFlags;        // User mode flags (e.g., operator status)

        bool _isRegistered;            // True if user has completed registration
        bool _validPassword;           // True if password authentication succeeded
        ConnectionManager &_serverContext; // Reference to the server's connection manager
        int	_socketFd;                // Socket file descriptor for this client

        // Updates the user's full IRC identifier string
        void refreshIdentifier();

        // Checks if the provided username is valid according to IRC rules
        bool checkIsValidUsername(std::string username);

        // Checks if the provided nickname is valid according to IRC rules
        bool checkValidNickname(std::string nickname);

    public:
        ClientUser(ConnectionManager &context, int socket);
        virtual ~ClientUser();

        // Getters for user identity and state
        std::string const &getNickname() const;
        std::string const &getUsername() const;
        std::string const &getRealname() const;
        std::string const &getHostname() const;
        std::string const &getIdentifier() const;
        std::string const &getModeFlags() const;
        
        bool passwordEnabled() const;
        int const &getSocket() const;
        bool confirmFullyRegistered() const;

        // Setters for user identity fields
        void setNickname(std::string nickname);
        void setUsername(std::string username);
        void setRealname(std::string realname);
        void setHostname(std::string hostname);

        // Marks the user as having passed password authentication
        void authorizePassword();

        // Mode management for IRC user flags
        void configureModes(std::string modesToAdd, std::string modesToremove);
        void addModeFlags(std::string modeString);
        void removeModeFlags(std::string modeString);
        bool confirmModePresence(char flag) const ;
        bool checkUserOperator() const;

        // Marks the user as fully registered on the server
        void setRegistered();

        // Handles incoming data from the user's socket
        void handleSocketInput();

        // Sends a message to all users sharing a channel with this user
        void userBroadcast(std::string message);

        // Checks if the user's nickname already exists on the server
        bool confirmNicknameExist();

        // Verifies that all required user details are set and valid
        bool checkAllUserDetails();

        // Exception classes for invalid user input
        class InvalidUsernameException : public std::exception {};
        class InvalidNicknameException : public std::exception {};
        class NicknameTooLongException : public std::exception {};
};

// Allows easy printing of user details to output streams
std::ostream &operator<<(std::ostream &os, ClientUser const &obj);

#endif




// // Maximum allowed length for a user's nickname
// #define MAX_NICKNAME_LENGHT 19

// class ConnectionManager;

// // Represents a connected IRC client and manages its state and identity
// class ClientUser
// {

// 	private:
// 		std::string _nickname;
// 		std::string _username;
// 		std::string _hostname;
// 		std::string _realname;
// 		std::string _fullIdentifier;
// 		std::string _modeFlags;

// 		bool _isRegistered;
// 		bool _validPassword;
// 		ConnectionManager &_serverContext;
// 		int	_socketFd;

// 		// Updates the user's full IRC identifier string
// 		void refreshIdentifier();

// 		bool checkIsValidUsername(std::string username);
// 		bool checkValidNickname(std::string nickname);

// 	public:
// 		ClientUser(ConnectionManager &context, int socket);
// 		virtual ~ClientUser();

// 		std::string const &getNickname() const;
// 		std::string const &getUsername() const;
// 		std::string const &getRealname() const;
// 		std::string const &getHostname() const;
// 		std::string const &getIdentifier() const;
// 		std::string const &getModeFlags() const;
		
// 		bool passwordEnabled() const;
// 		int const &getSocket() const;
// 		bool confirmFullyRegistered() const;

// 		void setNickname(std::string nickname);
// 		void setUsername(std::string username);
// 		void setRealname(std::string realname);
// 		void setHostname(std::string hostname);
// 		void authorizePassword();
// 		void configureModes(std::string modesToAdd, std::string modesToremove);
// 		void addModeFlags(std::string modeString);
// 		void removeModeFlags(std::string modeString);
// 		bool confirmModePresence(char flag) const ;
// 		bool checkUserOperator() const;

// 		void setRegistered();

// 		void handleSocketInput();
// 		void userBroadcast(std::string message);
// 		bool confirmNicknameExist();
// 		bool checkAllUserDetails();

// 		class InvalidUsernameException : public std::exception {};
// 		class InvalidNicknameException : public std::exception {};
// 		class NicknameTooLongException : public std::exception {};
// };

// std::ostream &operator<<(std::ostream &os, ClientUser const &obj);

// #endif 
