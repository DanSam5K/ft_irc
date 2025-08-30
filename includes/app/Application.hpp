/****************************************************************************#
#- - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - - -#
#- - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - - -#
#****************************************************************************#
#  						        Application.hpp     					     #
#****************************************************************************/

#ifndef APPLICATION_HPP
# define APPLICATION_HPP

#include "ConnectionManager.hpp"
#include "ft_irc.hpp"
#include "PasswordManager.hpp"
#include "utils_logger.hpp"
#include "utils_signal_manager.hpp"
#include <csignal> // Include for signal handling, it is needed because we need to handle signals like SIGINT (used on the function setUpServer()).
//SIGINT is the signal sent to a process to request its termination. In setUpServer(), we need to ensure that we can gracefully shut down the server when this signal is received.
#include <exception> // Include for standard exceptions
//#include <stdexcept> // Include for standard exceptions
// In reality, both exceptions libraries are not necessary to include separately, as <exception> already includes <stdexcept>.
#include <fcntl.h> // Include for fcntl function
#include <errno.h> // Include for errno macros, for example: errno, EINTR, etc. Here used on the function setUpServer()
#include <sys/poll.h>
#include <vector>

//STRUCTS
// Represents basic socket information (file descriptor and address)
struct SocketInfo {
	int fd;
	struct sockaddr_in address; // this struct holds the address information for a socket, located in <sys/socket.h>, in "ft_irc.hpp".
								// struct sockaddr_in {
								//     sa_family_t    sin_family; // Address family (AF_INET for IPv4)
								//     in_port_t      sin_port;   // Port number (in network byte order)
								//     struct in_addr sin_addr;   // IP address (in network byte order)
								// };
};

// Struct to encapsulate an IRC message to be sent to a specific client
// Contains the file descriptor of the client it is intended FOR, as well as the actual IRC payload (message) to be sent.
struct IncomingIRCMessage {
	int client_fd;
	std::string irc_payload;
};




//CLASSES
class Application {
	private:
		// Port the server is listening on
		int _port;

		// Server and current client socket info
		SocketInfo _serverSocket; // SocketInfo is the struct above: fd and address for the server socket.
		SocketInfo _clientSocket;

		// Max number of simultaneous connections
		const static int MAX_CLIENTS = SOMAXCONN;

		// Track number of active clients
		int _activeConnections;

		// Poll file descriptors for I/O multiplexing
		std::vector<pollfd>* _pollDescriptors;

		// Queue of messages to be sent to clients
		std::vector<IncomingIRCMessage> _pendingMessages;

		// Authentication and server state management
		PasswordManager* _auth;
		ConnectionManager* _state;

		// Server setup & control
		void setUpServer();
		void eventLoop();
		void acceptNewClient();

		// Client communication
		void readFromClients(); //read data from connected clients
		void handleIncomingIrcPayload(int fd); //handle incoming IRC payload
		void receiveCommands(int fd, std::string &messageBuf); //receive commands from clients
		bool messageHasTerminator(std::string &messageBuf); //check if message has terminator
		void extractCommands(int fd, std::string &messageBuf); //extract commands from message buffer
		void processClientInput(int fd, std::string &messageBuf); // process client input such as messages, commands, etc.

		// CommandMessage distribution
		void broadcastPendingMessages(); //send pending messages to all clients
		void cleanUpMessagesFromRemovedClient(int fd); //remove messages from a client that has disconnected

		// Custom exceptions for cleaner control flow
		class ServerShutdownException : public std::exception {}; // Thrown when the server is shutting down
		class NoAvailablePayloadException : public std::exception {}; // Thrown when there is no available payload
		class ClientDisconnectedException : public std::exception {}; // Thrown when a client disconnects

		Application(const Application &src); // Copy constructor
		Application &operator=(const Application &other); // Copy assignment operator

	public:
		Application(int port, std::string password); // Constructor
		virtual ~Application(); // Destructor

		void launchServer(); // Launch the IRC server: set up the server, then enter the event loop
		void sendMessageToClient(int socket, const std::string &message); // Send a message to a specific client: check if the client is connected, then send the message
		void removeClient(int fd); // Remove a client from the server, cleaning up any associated resources
};

#endif
