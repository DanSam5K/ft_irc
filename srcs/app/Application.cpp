/****************************************************************************#
#- - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - - -#
#- - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - - -#
#****************************************************************************#
#  						        Application.cpp     					     #
#****************************************************************************/
/* - Initializes the server with the given port and password.
   - Sets up the listening socket.
   - Manages client connections and communication.
   - Handles incoming IRC messages and broadcasts them to clients.
   - Cleans up resources on shutdown. */

#include "Application.hpp"

/* Constructor initializes the server with the given port and password.
   - Sets up the server socket.
   - Initializes authentication and connection management. */
Application::Application(int port, std::string password) 
	: _port(port), _activeConnections(0) // Initialize port with the provided value, and sets _activeConnections to 0 (new connection count)
{
	setUpServer(); // Set up the server socket and prepare it for incoming connections
	_auth = new PasswordManager(password); // This line creates an object of PasswordManager with the provided password. This object will manage password-related functionalities.
	_state = new ConnectionManager(*this, *_auth); // This line creates an object of ConnectionManager, this object will manage client connections and their states.
							                       // is used *this, because it refers to the current instance of the Application class.
							                       // and *_auth is used to pass the PasswordManager instance by reference. That means it can be modified by the ConnectionManager.
							                       // In other and simpler words: &(_auth) is the address of the pointer, while *_auth is the actual object.
	// Every instance of Application requires its own instances of these managers, one for authentication and one for connection management.
}

/* Destructor cleans up resources, including closing the server socket and deleting allocated memory. */
Application::~Application()
{
	logActionUtils::info("Shutting down application"); // Log
	close(_serverSocket.fd); // Close server socket (created in setUpServer)
	delete _pollDescriptors; // Delete poll descriptors (created in setUpPoll)
	delete _auth; // Delete authentication manager (created in constructor)
	delete _state; // Delete connection manager (created in constructor)
}

/* Sets up the server socket and prepares it for incoming connections, including creating a non-blocking socket,
   binding to the specified port, listening for incoming connections, and initializing polling structures for client management. */
void Application::setUpServer()
{
	logActionUtils::info("Starting Server Setup"); // Log
	_serverSocket.fd = socket(AF_INET, SOCK_STREAM, 0); // Create socket. Uses AF_INET for IPv4, SOCK_STREAM for TCP, and 0 for the default protocol.

	if (_serverSocket.fd == -1) // If socket creation fails...
	{
		throw std::runtime_error("Failed to create socket!"); // ... throw error.
	}

	logActionUtils::info("Setting non-blocking mode for server socket"); // Log
	int currentFlags = fcntl(_serverSocket.fd, F_GETFL, 0); // Get current socket flags, so it can change it into non-blocking mode.
	fcntl(_serverSocket.fd, F_SETFL, currentFlags | O_NONBLOCK); // Set non-blocking mode.


	logActionUtils::info("Connecting to port", _port); // Log
	_serverSocket.address.sin_family = AF_INET; // Set address family to IPv4, as required by the protocol. AF_INET is a macro located in <netinet/in.h>
	_serverSocket.address.sin_port = htons(_port); // Convert port number to network byte order, because network protocols use big-endian byte order
													// htons is the function used to convert values to network byte order
	_serverSocket.address.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any IP address,
																// htonl is the function used to convert values to network byte order. Is different from htons because
																// htons is used for port numbers (16 bits), while htonl is used for IP addresses (32 bits).
																// INADDR_ANY is a macro that represents any IP address
																// to allow the server to accept connections from any client.



	logActionUtils::info("Binding Socket"); // Log binding (associate a socket with a specific address and port)
											// sockets are basically endpoints for sending and receiving data.
											// In other words, they allow communication between different processes.
											// Sockets are like "phone jacks" used to manage connections between clients and servers.
											// Binding a socket is like plugging in a phone jack to a wall socket.

	if (bind(_serverSocket.fd, (struct sockaddr *)&_serverSocket.address, sizeof(_serverSocket.address)) == -1) // If binding fails...
	// bind(socket, address, address_length). "(struct sockaddr *)&_serverSocket.address" casts the address to the correct type (sockaddr).
	{
		throw(std::runtime_error("Port Binding Failed")); // ... throw error.
	}



	logActionUtils::info("Listening for connections"); // Log
	if (listen(_serverSocket.fd, SOMAXCONN) == -1) // If listening fails...
	// listen(socket, backlog). SOMAXCONN is a constant that represents the maximum number of possible pending connections.
	// backlog/SOMAXCONN then defines the maximum size of this "waiting room".
	{
		throw std::runtime_error("Unable to listen on socket"); // ... throw error.
	}

	_pollDescriptors = new std::vector<pollfd>(MAX_CLIENTS + 1); //std::vector<pollfd>* _pollDescriptors; - pointer to a vector of pollfd structures

	// These are the types of containers: std::pair, std::map, std::vector, std::array, std::list
	// std::list is a doubly linked list that allows for efficient insertion and removal of elements.
	// std::pair is a simple container to store two related values.
	// std::map is related to std::pair in that each element in a std::map is a std::pair, where the first element is the key and the second element is the value.
	// std::array has a fixed size, while std::vector can grow dynamically.

	// the commands used in std::vector are:
	// push_back (allows adding elements to the end of the vector) - ex: vec.push_back(4) - this results in vec = {1, 2, 3, 4}
	// pop_back (removes the last element), - ex: vec.pop_back(); - this results in vec = {1, 2, 3} (removes 4)
	// insert (adds elements at a specific position) - ex: vec.insert(vec.begin() + 1, 5); - this results in vec = {1, 5, 2, 3, 4}
	// erase (removes elements from a specific position)- ex: vec.erase(vec.begin() + 1); - this results in vec = {1, 2, 3, 4} (removes 5)
	// clear (removes all elements), - ex: vec.clear(); - this results in vec = {}
	// size (returns the number of elements), - ex: vec.size(); - this results in 4
	// capacity (returns the size of the allocated storage), - ex: vec.capacity(); - this results in 8 (or more, depending on the implementation)
	// operator[] (accesses elements by index). - ex: vec[0]; - this results in 1

	// the vector has as prototype: std::vector<Type> vec(size); - where Type is the data type of the elements, and size is the initial size of the vector.
	// vec is the name of the vector, but in our use is not explicitly defined, that means we don't have a specific variable name for it, as it is assigned to a pointer.
	// In this case, the vector is initialized with a size of MAX_CLIENTS + 1 to accommodate the server socket and all potential client connections.
	// pollfd is a structure used to monitor multiple file descriptors. It is defined in <poll.h>.
	// It looks like:
	// struct pollfd {
	//		int fd;         // file descriptor to monitor
	//		short events;   // events to monitor (e.g., POLLIN, POLLOUT)
	//		short revents;  // events that occurred
	//};
	// new std::vector is creating a dynamic array that can grow in size to hold all client connections.
	// <pollfd> here is a structure that holds information about the file descriptors being monitored.
	// Each element in the vector is a pollfd structure, which contains the file descriptor and the events to monitor.

	signal(SIGINT, SignalManager::signalHandler);
	// The line above sets up a signal handler for SIGINT (Ctrl+C) to allow graceful shutdown.
	// (tells the program what to do when it receives the Ctrl+C signal).
	// The what to do is defined in the SignalManager::signalHandler function.
	// It changes the shouldShutdown flag to true, allowing the server to shut down gracefully.
	// The next step for the shutdown process, after changing the flag, would be to clean up resources and close connections.
	// This can be seen in the file utils_signal_manager.cpp

	logActionUtils::info("Server setup complete"); // Log
}

void Application::launchServer()
{
	std::vector<pollfd> &client_fds = *_pollDescriptors;
	client_fds[0].fd = _serverSocket.fd;
	client_fds[0].events = POLLIN;

	// _activeConnections = 0; // keep track of number of connected clients

	logActionUtils::info("Entering main loop");

	while (!SignalManager::shouldShutdown)
	{
		try
		{
			eventLoop();
			acceptNewClient();
			broadcastPendingMessages();
			readFromClients();
		}
		catch (Application::ServerShutdownException &e)
		{
			logActionUtils::warn("Server stopping gracefully");
			break;
	   }
	}
	logActionUtils::info("Exited main loop");
}

void Application::readFromClients()
{
	std::vector<pollfd> &client_fds = *_pollDescriptors;
	for (int i = 1; i <= _activeConnections && !SignalManager::shouldShutdown; ++i)
	{
		if (client_fds[i].fd != -1 && client_fds[i].revents &POLLIN)
		{
			handleIncomingIrcPayload(client_fds[i].fd);
		}
	}
}

void Application::eventLoop()
{
	// std::vector<pollfd> &client_fds = *_pollDescriptors;

	int num_ready = poll(_pollDescriptors->data(), _activeConnections + 1, -1);
	if (num_ready == -1 && SignalManager::shouldShutdown)
	{
		logActionUtils::warn("Signal Interrupted");
		throw Application::ServerShutdownException();
	}
	else if (num_ready == -1)
	{
		throw std::runtime_error("Polling failed");
	}
}

void Application::acceptNewClient()
{
	std::vector<pollfd> &client_fds = *_pollDescriptors;

	if (!(client_fds[0].revents &POLLIN))
	{
		return;
	}
	socklen_t clientSize = sizeof(_clientSocket.address);

	// logActionUtils::info("Application: Accepting client call...");
	_clientSocket.fd = accept(_serverSocket.fd, (struct sockaddr *)&_clientSocket.address, &clientSize);
	if (_clientSocket.fd == -1)
	{
		logActionUtils::warn("Client connection attempt Failed");
		return;
	}
	logActionUtils::info("New client connection with fd", _clientSocket.fd);

	// Set the client socket to non-blocking
	int flags = fcntl(_clientSocket.fd, F_GETFL, 0);
	fcntl(_clientSocket.fd, F_SETFL, flags | O_NONBLOCK);

	// add new client to the list of file descriptors to monitor
	if (_activeConnections == MAX_CLIENTS)
	{
		throw std::runtime_error("Maximum client Limit reached");
	}
	client_fds[_activeConnections + 1].fd = _clientSocket.fd;
	client_fds[_activeConnections + 1].events = POLLIN | POLLOUT;

	// Creating new user for client
	_state->registerPendingUser(_clientSocket.fd, _clientSocket.address);
	_activeConnections++;
}

void Application::removeClient(int fd)
{
	logActionUtils::info("Disconnecting client", fd);
	try
	{
		cleanUpMessagesFromRemovedClient(fd);

		_state->disconnectUserBySocket(fd); // Closes the client socket

		std::vector<pollfd>&client_fds = *_pollDescriptors;
		for (int i = 1; i <= _activeConnections; i++)
		{
			if (client_fds[i].fd == fd)
			{
				client_fds.erase(client_fds.begin() + i);  // Remove the client from the vector
				_activeConnections--;
				break;
 		   	}
  		}
	}
	catch (ConnectionManager::UserNotFoundException &e)
	{
		logActionUtils::warn("Application: ConnectionManager:", e.what());
	}
}

void Application::cleanUpMessagesFromRemovedClient(int fd)
{
	if (_pendingMessages.empty()) 
	{
		return ;
	}
	// Remove queued messages for the disconnected client
    std::vector<IncomingIRCMessage>::iterator it = _pendingMessages.begin();
	while (it != _pendingMessages.end())
	{
		if (it->client_fd == fd)
		{
        	it = _pendingMessages.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void Application::handleIncomingIrcPayload(int fd)
{
	static std::map<int, std::string> clientMessageBuffers;

	try
	{
		receiveCommands(fd, clientMessageBuffers[fd]);
	}
	catch (Application::ClientDisconnectedException &e)
	{
		clientMessageBuffers.erase(fd);
		removeClient(fd);
	}
}

void Application::receiveCommands(int fd, std::string &messageBuf)
{
	try
	{
		extractCommands(fd, messageBuf);
		processClientInput(fd, messageBuf);
	}
	catch (Application::NoAvailablePayloadException &e)
	{
		// No data available right now, continue processing
		return;
	}
	catch (ConnectionManager::UserNotFoundException &e)
	{
		logActionUtils::warn("Application: ConnectionManager:", e.what());
		throw ClientDisconnectedException();
	}
}	

bool Application::messageHasTerminator(std::string &messageBuf)
{
	size_t terminator = messageBuf.find("\r\n", 0);
	if (terminator == std::string::npos)
	{
		return (false);
	}
	return (true);
}

void Application::extractCommands(int fd, std::string &messageBuf)
{
	char buf[4096];
	memset(buf, 0, sizeof(buf));
	int bytes_recv = 0;

	bytes_recv = recv(fd, buf, sizeof(buf), 0);
	if (bytes_recv == -1)
	{
		if	(errno == EWOULDBLOCK || errno == EAGAIN)
		{
			throw NoAvailablePayloadException();
		}
		else
		{
			logActionUtils::warn("Error occurs while receiving message from socket", fd);
			throw ClientDisconnectedException();
		}
	}
	if (bytes_recv == 0)
	{
		// logActionUtils::warn("Application: read returned 0, read:", buf);
		throw ClientDisconnectedException();
	}
	messageBuf += std::string(buf, bytes_recv);
}

void Application::processClientInput(int fd, std::string &messageBuf)
{
	size_t terminator = messageBuf.find("\r\n", 0);
	if (terminator == std::string::npos)
	{
		return ;
	}
	size_t pos = 0;

	while (terminator != std::string::npos)
	{
		std::string first_command = messageBuf.substr(pos, terminator + 2 - pos);
		logActionUtils::command(fd, first_command);
		
		try
		{
			_state->processClientCommand(_state->getUserBySocket(fd), first_command);
		}
		catch (ConnectionManager::UserNotFoundException &e)
		{
			logActionUtils::warn("Application: User not found for socket", fd);
			throw ClientDisconnectedException();
		}
		
		pos = terminator + 2;
		terminator = messageBuf.find("\r\n", pos);
	}
	messageBuf = messageBuf.substr(pos);
}

void Application::sendMessageToClient(int socket, const std::string&message)
{
	IncomingIRCMessage newMessage;
	newMessage.client_fd = socket;
	newMessage.irc_payload = message;
	_pendingMessages.push_back(newMessage);
}

void Application::broadcastPendingMessages()
{
	std::vector<pollfd> &client_fds = *_pollDescriptors;

	for (std::vector<IncomingIRCMessage>::const_iterator it = _pendingMessages.begin(); it != _pendingMessages.end(); ++it)
	{
		const IncomingIRCMessage&message = *it;
		int index = -1;
		for (int i = 1; i <= _activeConnections; i++)
		{
			if (client_fds[i].fd == message.client_fd)
			{
				index = i;
				break;
			}
		}
		if (index != -1 && client_fds[index].revents &POLLOUT)
		{
			ssize_t bytes_sent = send(message.client_fd, message.irc_payload.c_str(), message.irc_payload.length(), 0);
			if (bytes_sent == -1)
			{
				logActionUtils::warn("CommandMessage to socket fail", message.client_fd);
				removeClient(message.client_fd);
			}
		}
	}
	_pendingMessages.clear();
}
