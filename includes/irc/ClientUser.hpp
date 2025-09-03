/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  						         ClientUser.hpp    	 					     #
#****************************************************************************/

#ifndef CLIENT_USER_HPP
# define CLIENT_USER_HPP

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
        ClientUser(ConnectionManager &context, int socket); // Constructor that takes a reference to the connection manager and a socket file descriptor
        virtual ~ClientUser();

        // Getters for user identity and state
        std::string const &getNickname() const; // Get the user's nickname
        std::string const &getUsername() const; // Get the user's username
        std::string const &getRealname() const; // Get the user's real name
        std::string const &getHostname() const; // Get the user's hostname
        std::string const &getIdentifier() const; // Get the user's full identifier
        std::string const &getModeFlags() const; // Get the user's mode flags

        bool passwordEnabled() const; // Check if password protection is enabled
        int const &getSocket() const; // Get the user's socket file descriptor
        bool confirmFullyRegistered() const; // Check if the user is fully registered

        // Setters for user identity fields
        void setNickname(std::string nickname); // Set the user's nickname
        void setUsername(std::string username); // Set the user's username
        void setRealname(std::string realname); // Set the user's real name
        void setHostname(std::string hostname); // Set the user's hostname

        // Marks the user as having passed password authentication
        void authorizePassword(); // Marks the user as having passed password authentication

        // Mode management for IRC user flags
        void configureModes(std::string modesToAdd, std::string modesToremove); // Configure user modes
        void addModeFlags(std::string modeString); // Add user mode flags
        void removeModeFlags(std::string modeString); // Remove user mode flags
        bool confirmModePresence(char flag) const; // Check if a specific mode flag is present
        bool checkUserOperator() const; // Check if the user is an operator

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
        class InvalidUsernameException : public std::exception {}; // Exception thrown when an invalid username is provided
        class InvalidNicknameException : public std::exception {}; // Exception thrown when an invalid nickname is provided
        class NicknameTooLongException : public std::exception {}; // Exception thrown when a nickname is too long
};

// Allows easy printing of user details to output streams
std::ostream &operator<<(std::ostream &os, ClientUser const &obj);

#endif