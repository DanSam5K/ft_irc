/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  						        Application.hpp     					     #
#****************************************************************************/

#ifndef APPLICATION_HPP
# define APPLICATION_HPP

#include "ConnectionManager.hpp"
#include "ft_irc.hpp"
#include "PasswordManager.hpp"
#include <csignal>
#include <errno.h>
#include <sys/poll.h>

// Represents basic socket information (file descriptor and address)
struct SocketInfo {
	int fd;
	struct sockaddr_in address;
};

// Struct to encapsulate an IRC message to be sent to a specific client
struct IncomingIRCMessage {
	int client_fd;
	std::string irc_payload;
};

class Application {
	private:
		// Port the server is listening on
		int _port;

		// Server and current client socket info
		SocketInfo _serverSocket;
		SocketInfo _clientSocket;

		// Max number of simultaneous connections
		const static int MAX_CLIENTS = SOMAXCONN;

		// Track number of active clients
		int _activeConnections;

		// Poll file descriptors for I/O multiplexing (vector that holds the state of each connection)
		std::vector<pollfd>* _pollDescriptors;

		// Queue of messages to be sent to clients (vector that holds the messages for each client)
		std::vector<IncomingIRCMessage> _pendingMessages;

		// Authentication and server state management (pointer to the respective managers)
		PasswordManager* _auth; // handles user authentication (validate user credentials, manage user sessions)
		ConnectionManager* _state; // handles client connections (track active connections, handle connection lifecycle)

		// Server setup &control
		void setUpServer(); // create socket, bind, listen, accept, poll (for allowing I/O multiplexing)
		void eventLoop(); // wait for events, handle events, repeat
		void acceptNewClient(); // create new socket, accept connection, add to poll descriptors

		// Client communication
		void readFromClients(); // handle incoming messages (read from socket, store in buffer, process buffer)
		void handleIncomingIrcPayload(int fd); // process incoming IRC payloads (validate, parse, route)
		void receiveCommands(int fd, std::string &messageBuf); // extract commands from message buffer (read, validate, execute)
		bool messageHasTerminator(std::string &messageBuf); // check if message buffer has terminator (find terminator, validate message)
		void extractCommands(int fd, std::string &messageBuf); // extract commands from message buffer (identify command boundaries, store commands)
		void processClientInput(int fd, std::string &messageBuf); // process client input (validate, parse, execute commands)

		// CommandMessage distribution
		void broadcastPendingMessages(); // send pending messages to all clients (iterate over clients, send messages)
		void cleanUpMessagesFromRemovedClient(int fd); // remove messages from a client that has disconnected (identify client, remove messages from queue)

		// Custom exceptions for cleaner control flow
		class ServerShutdownException : public std::exception {}; // thrown when the server is shutting down (close sockets, clean up resources)
		class NoAvailablePayloadException : public std::exception {}; // thrown when there is no payload available for processing (check queue, validate message)
		class ClientDisconnectedException : public std::exception {}; // thrown when a client disconnects unexpectedly (identify client, clean up resources)

		Application(const Application &src);
		Application &operator=(const Application &other);

	public:
		Application(int port, std::string password);
		virtual ~Application();

		void launchServer(); // start the server (set up server, enter event loop)
		void sendMessageToClient(int socket, const std::string &message); // send a message to a specific client (find client, send message)
		void removeClient(int fd); // remove a client from the server (find client, close socket, clean up resources)
};

#endif
