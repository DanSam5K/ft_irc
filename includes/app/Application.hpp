#ifndef APPLICATION_H
#define APPLICATION_H

#include "Password.hpp"
#include "ft_irc.hpp"
#include "Context.hpp"

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
		Password* _auth;
		Context* _state;

		// Server setup & control
		void setUpServer();
		void eventLoop();
		void acceptNewClient();

		// Client communication
		void readFromClients();
		void handleIncomingIrcPayload(int fd);
		void receiveCommands(int fd, std::string& message_buffer);
		bool messageHasTerminator(std::string& message_buffer);
		void extractCommands(int fd, std::string& message_buffer);
		void processClientInput(int fd, std::string& message_buffer);

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
		Application(int port, std::string password);
		~Application();

		void launchServer();
		void sendMessageToClient(int socket, const std::string& message);
		void removeClient(int fd);
};

#endif



// Version 1
// #ifndef APPLICATION_H
// #define APPLICATION_H

// #include "Password.hpp"
// #include "ft_irc.hpp"

// #include "Context.hpp"

// struct SocketInfo
// {
// 	int fd;
// 	struct sockaddr_in address;
// };

// struct IncomingIRCMessage {
//   int client_fd;
//   std::string irc_payload;
// };

// class Application
// {
// 	private:
// 		int	_port;
// 		SocketInfo _serverSocket;
// 		SocketInfo _clientSocket;

// 		std::list<pthread_t> _clientThreads;
// 		const static int MAX_CLIENTS = SOMAXCONN;

// 		int _activeConnections;
		
// 		std::vector<pollfd> * _pollDescriptors;
// 		std::vector<IncomingIRCMessage> _pendingMessages;

// 		Password * _auth;
// 		Context * _state;

// 		void setUpServer();
// 		void eventLoop();
// 		void acceptNewClient();
// 		void readFromClients();
// 		void handleIncomingIrcPayload( int fd );
// 		void receiveCommands( int fd, std::string & message_buffer );
// 		bool messageHasTerminator( std::string & message_buffer );
// 		void extractCommands( int fd, std::string & message_buffer );
// 		void processClientInput( int fd, std::string & message_buffer );
// 		void broadcastPendingMessages();
// 		void cleanUpMessagesFromRemovedClient( int fd );

// 		class SeverShutdownException : public std::exception {};
// 		class NoAvailablePayloadException : public std::exception {};
// 		class ClientDisconnectedException : public std::exception {};

// 	public:
// 		Application( int port, std::string password );
// 		Application( const Application & src );
// 		Application & operator=( const Application & other );
// 		virtual ~Application();

// 		// void checkClientTimeout(); // Separate class
// 		void launchServer();
// 		void sendMessageToClient(int socket, const std::string& message);
// 		void removeClient( int fd );

// };

// #endif /* APPLICATION_H */
