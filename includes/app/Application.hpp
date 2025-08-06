#ifndef APPLICATION_H
#define APPLICATION_H

#include "ConnectionManager.hpp"
#include "ft_irc.hpp"
#include "PasswordManager.hpp"
#include "log_event.hpp"
#include "signal.hpp"
#include <csignal>
#include <exception>
#include <stdexcept>
#include <fcntl.h> // Include for fcntl function
#include <errno.h>
#include <sys/poll.h>
#include <vector>

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
		void readFromClients();
		void handleIncomingIrcPayload(int fd);
		void receiveCommands(int fd, std::string& messageBuf);
		bool messageHasTerminator(std::string& messageBuf);
		void extractCommands(int fd, std::string& messageBuf);
		void processClientInput(int fd, std::string& messageBuf);

		// Message distribution
		void broadcastPendingMessages();
		void cleanUpMessagesFromRemovedClient(int fd);

		// Custom exceptions for cleaner control flow
		class ServerShutdownException : public std::exception {};
		class NoAvailablePayloadException : public std::exception {};
		class ClientDisconnectedException : public std::exception {};

		Application(const Application& src);
		Application &operator=(const Application& other);

	public:
		Application(int port, const std::string &password);
		~Application();

		void launchServer();
		void sendMessageToClient(int socket, const std::string &message);
		void removeClient(int fd);
};

#endif
