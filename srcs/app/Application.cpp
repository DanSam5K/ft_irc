
// #include "Application.hpp"
// #include "Context.hpp"
// #include "ft_irc.hpp"
// #include "Password.hpp"
// #include "log_event.hpp"
// #include "signal.hpp"

// #include <csignal>
// #include <exception>
// #include <stdexcept>
// #include <fcntl.h>
// #include <errno.h>
// #include <sys/poll.h>
// #include <cstring>

// #define IRC_MSG_TERMINATOR "\r\n"
// #define BUFFER_SIZE 4096

// Application::Application(int port, std::string password) : _port(port), _activeConnections(0) {
//     setUpServer();
//     _auth = new Password(password);
//     _state = new Context(*this, *_auth);
// }

// Application::~Application() {
//     log_event::info("Application: Terminating application");
//     close(_serverSocket.fd);
//     delete _pollDescriptors;
//     delete _auth;
//     delete _state;
// }

// // Set up the server socket and prepare for listening
// void Application::setUpServer() {
//     log_event::info("Application: Initializing server...");
//     _serverSocket.fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (_serverSocket.fd == -1)
//         throw std::runtime_error("Application: Can't create a socket!");

//     int flags = fcntl(_serverSocket.fd, F_GETFL, 0);
//     fcntl(_serverSocket.fd, F_SETFL, flags | O_NONBLOCK);

//     if (_port < 6660 || _port > 7000)
//         throw std::runtime_error("Application: Invalid port: must be between 6660 and 7000");

//     _serverSocket.address.sin_family = AF_INET;
//     _serverSocket.address.sin_port = htons(_port);
//     _serverSocket.address.sin_addr.s_addr = htonl(INADDR_ANY);

//     if (bind(_serverSocket.fd, (struct sockaddr*)&_serverSocket.address, sizeof(_serverSocket.address)) == -1)
//         throw std::runtime_error("Application: Can't bind port: may be in use.");

//     if (listen(_serverSocket.fd, SOMAXCONN) == -1)
//         throw std::runtime_error("Application: Can't listen!");

//     _pollDescriptors = new std::vector<pollfd>(MAX_CLIENTS + 1);
//     signal(SIGINT, sig::signalHandler);
//     log_event::info("Application: Server initialized.");
// }

// void Application::launchServer() {
//     std::vector<pollfd>& fds = *_pollDescriptors;
//     fds[0].fd = _serverSocket.fd;
//     fds[0].events = POLLIN;

//     log_event::info("Application: Launching server...");

//     while (!sig::stopServer) {
//         try {
//             eventLoop();
//             acceptNewClient();
//             broadcastPendingMessages();
//             readFromClients();
//         } catch (Application::ServerShutdownException&) {
//             log_event::warn("Application: Server shutdown");
//             break;
//         }
//     }
//     log_event::info("Application: Server loop terminated");
// }

// // Handles input from connected clients
// void Application::readFromClients() {
//     std::vector<pollfd>& fds = *_pollDescriptors;
//     for (int i = 1; i <= _activeConnections && !sig::stopServer; i++) {
//         if (fds[i].fd != -1 && (fds[i].revents & POLLIN)) {
//             handleIncomingIrcPayload(fds[i].fd);
//         }
//     }
// }

// // Waits for activity using poll
// void Application::eventLoop() {
//     std::vector<pollfd>& fds = *_pollDescriptors;
//     int num_ready = poll(fds.data(), _activeConnections + 1, -1);

//     if (num_ready == -1) {
//         if (sig::stopServer)
//             throw Application::ServerShutdownException();
//         throw std::runtime_error("Application: Poll error");
//     }
// }

// // Accept new clients and register them
// void Application::acceptNewClient() {
//     std::vector<pollfd>& fds = *_pollDescriptors;
//     if (!(fds[0].revents & POLLIN)) return;

//     socklen_t size = sizeof(_clientSocket.address);
//     _clientSocket.fd = accept(_serverSocket.fd, (struct sockaddr*)&_clientSocket.address, &size);

//     if (_clientSocket.fd == -1) {
//         log_event::warn("Application: Accept failed");
//         return;
//     }

//     int flags = fcntl(_clientSocket.fd, F_GETFL, 0);
//     fcntl(_clientSocket.fd, F_SETFL, flags | O_NONBLOCK);

//     if (_activeConnections >= MAX_CLIENTS)
//         throw std::runtime_error("Application: Too many clients");

//     fds[_activeConnections + 1].fd = _clientSocket.fd;
//     fds[_activeConnections + 1].events = POLLIN | POLLOUT;
//     _state->create_unregistered_user(_clientSocket.fd);
//     _activeConnections++;
// }

// // Remove a client from the server
// void Application::removeClient(int fd) {
//     log_event::info("Application: Removing client", fd);
//     cleanUpMessagesFromRemovedClient(fd);
//     _clientBuffers.erase(fd);

//     try {
//         _state->remove_user(fd);
//         std::vector<pollfd>& fds = *_pollDescriptors;
//         for (int i = 1; i <= _activeConnections; i++) {
//             if (fds[i].fd == fd) {
//                 fds.erase(fds.begin() + i);
//                 _activeConnections--;
//                 break;
//             }
//         }
//     } catch (Context::CouldNotFindUserException& e) {
//         log_event::warn("Application:", e.what());
//     }
// }

// // Delete any queued messages for a disconnected client
// void Application::cleanUpMessagesFromRemovedClient(int fd) {
//     _pendingMessages.erase(
//         std::remove_if(
//             _pendingMessages.begin(), _pendingMessages.end(),
//             [fd](const IncomingIRCMessage& msg) { return msg.client_fd == fd; }),
//         _pendingMessages.end());
// }

// // Process incoming IRC messages from a single client
// void Application::handleIncomingIrcPayload(int fd) {
//     try {
//         receiveCommands(fd, _clientBuffers[fd]);
//     } catch (Application::ClientDisconnectedException&) {
//         removeClient(fd);
//     }
// }

// // Reads a complete command or part of one, appending it to the buffer
// void Application::receiveCommands(int fd, std::string& buffer) {
//     while (!messageHasTerminator(buffer) && !sig::stopServer) {
//         try {
//             extractCommands(fd, buffer);
//             processClientInput(fd, buffer);
//         } catch (Application::NoAvailablePayloadException&) {
//             break;
//         } catch (Context::CouldNotFindUserException& e) {
//             log_event::warn("Application:", e.what());
//         }
//     }
// }

// // Check if a full command has been received (terminated by \r\n)
// bool Application::messageHasTerminator(const std::string& buffer) {
//     return buffer.find(IRC_MSG_TERMINATOR) != std::string::npos;
// }

// // Read raw data from client and append to buffer
// void Application::extractCommands(int fd, std::string& buffer) {
//     char buf[BUFFER_SIZE];
//     memset(buf, 0, sizeof(buf));

//     int bytes = recv(fd, buf, sizeof(buf), 0);
//     if (bytes == -1) {
//         if (errno == EWOULDBLOCK || errno == EAGAIN)
//             throw NoAvailablePayloadException();
//         throw ClientDisconnectedException();
//     }
//     if (bytes == 0)
//         throw ClientDisconnectedException();

//     buffer += std::string(buf, bytes);
// }

// // Process each command found in the buffer
// void Application::processClientInput(int fd, std::string& buffer) {
//     size_t pos = 0, term;
//     while ((term = buffer.find(IRC_MSG_TERMINATOR, pos)) != std::string::npos) {
//         std::string cmd = buffer.substr(pos, term - pos + 2);
//         log_event::command(fd, cmd);
//         _state->handle_message(_state->get_user_by_socket(fd), cmd);
//         pos = term + 2;
//     }
//     buffer = buffer.substr(pos); // Retain any incomplete message
// }

// // Queue a message for delivery
// void Application::sendMessageToClient(int socket, const std::string& message) {
//     _pendingMessages.push_back({ socket, message });
// }

// // Deliver queued messages to clients when ready
// void Application::broadcastPendingMessages() {
//     std::vector<pollfd>& fds = *_pollDescriptors;

//     for (const IncomingIRCMessage& msg : _pendingMessages) {
//         for (int i = 1; i <= _activeConnections; i++) {
//             if (fds[i].fd == msg.client_fd && (fds[i].revents & POLLOUT)) {
//                 if (send(msg.client_fd, msg.irc_payload.c_str(), msg.irc_payload.size(), 0) == -1) {
//                     log_event::warn("Application: Send failed", msg.client_fd);
//                     removeClient(msg.client_fd);
//                 }
//                 break;
//             }
//         }
//     }
//     _pendingMessages.clear();
// }


#include "Application.hpp"
#include "Context.hpp"
#include "ft_irc.hpp"
#include "Password.hpp"
#include "log_event.hpp"
#include "signal.hpp"
#include <csignal>
#include <exception>
#include <stdexcept>
#include <fcntl.h> // Include for fcntl function
#include <errno.h>
#include <sys/poll.h>
#include <vector>

Application::Application( int port, std::string password ) : _port( port )
{
	setUpServer();
	_auth = new Password( password );
	_state = new Context( *this, *_auth );
}

Application::~Application()
{
	log_event::info( "Application: Terminating application" );
	close( _serverSocket.fd );
	delete _pollDescriptors;
	delete _auth;
	delete _state;
}

void Application::setUpServer()
{
	log_event::info( "Application: Initializing server..." );
	log_event::info( "Application: Creating server socket..." );
	_serverSocket.fd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( _serverSocket.fd == -1 )
	{
		throw std::runtime_error( "Application: Can't create a socket!" );
	}
	log_event::info( "Application: Setting the server socket to non-blocking..." );
	int flags = fcntl( _serverSocket.fd, F_GETFL, 0 );
	fcntl( _serverSocket.fd, F_SETFL, flags | O_NONBLOCK );

	log_event::info( "Application: Connecting to port", _port );
	_serverSocket.address.sin_family = AF_INET;
	if ( _port < 6660 || _port > 7000 )
	{
		throw( std::runtime_error( "Application: Invalid port: port must be between 6660 and 7000" ) );
	}
	_serverSocket.address.sin_port = htons( _port );
	_serverSocket.address.sin_addr.s_addr = htonl( INADDR_ANY );

	log_event::info( "Application: Binding socket..." );
	if ( bind( _serverSocket.fd, ( struct sockaddr * )&_serverSocket.address, sizeof( _serverSocket.address ) ) == -1 )
	{
		throw(std::runtime_error("Application: Can't bind port: Port might be in use."));
	}
	log_event::info( "Application: Mark the socket for listening..." );
	if ( listen(_serverSocket.fd, SOMAXCONN ) == -1 )
	{
		throw std::runtime_error( "Application: Can't listen !" );
	}
	_pollDescriptors = new std::vector<pollfd>( MAX_CLIENTS + 1 );
	signal( SIGINT, sig::signalHandler );
	log_event::info( "Application: Server initialized." );
}

void Application::launchServer()
{
	std::vector<pollfd> &client_fds = *_pollDescriptors;
	client_fds[0].fd = _serverSocket.fd;
	client_fds[0].events = POLLIN;

	_activeConnections = 0; // keep track of number of connected clients
	log_event::info( "Application: Launching server..." );

	while ( sig::stopServer == false )
	{
		try
		{
			eventLoop();
			acceptNewClient();
			broadcastPendingMessages();
			readFromClients();
		}
		catch ( Application::ServerShutdownException &e )
		{
			log_event::warn( "Application: Stopping server" );
			break;
	   }
	}
	log_event::info( "Application: Terminating server loop" );
}

void Application::readFromClients()
{
	std::vector<pollfd> &client_fds = *_pollDescriptors;
	for ( int i = 1; i <= _activeConnections && sig::stopServer == false; i++ )
	{
		if ( client_fds[i].fd != -1 && client_fds[i].revents & POLLIN )
		{
			handleIncomingIrcPayload(client_fds[i].fd);
		}
	}
}

void Application::eventLoop()
{
	std::vector<pollfd> &client_fds = *_pollDescriptors;

	int num_ready = poll( client_fds.data(), _activeConnections + 1, -1 );
	if ( num_ready == -1 && sig::stopServer == true )
	{
		log_event::warn( "Application: Poll error because of intercepted signal" );
		throw Application::ServerShutdownException();
	}
	else if ( num_ready == -1 && sig::stopServer == true )
	{
		throw std::runtime_error( "Application: Poll error" );
	}
}

void Application::acceptNewClient()
{
	std::vector<pollfd> &client_fds = *_pollDescriptors;

	if ( ! ( client_fds[0].revents & POLLIN ) )
	{
		return;
	}
	socklen_t clientSize = sizeof( _clientSocket.address );

	log_event::info( "Application: Accepting client call..." );
	_clientSocket.fd = accept( _serverSocket.fd, ( struct sockaddr * )&_clientSocket.address, &clientSize );
	if ( _clientSocket.fd == -1 )
	{
		log_event::warn( "Application: Client cannot connect!" );
		return;
	}
	log_event::info( "Application: Client is assigned socket", _clientSocket.fd );

	// Set the client socket to non-blocking
	int flags = fcntl( _clientSocket.fd, F_GETFL, 0 );
	fcntl( _clientSocket.fd, F_SETFL, flags | O_NONBLOCK );

	// add new client to the list of file descriptors to monitor
	if ( _activeConnections == MAX_CLIENTS )
	{
		throw std::runtime_error( "Application: Too many clients!" );
	}
	client_fds[_activeConnections + 1].fd = _clientSocket.fd;
	client_fds[_activeConnections + 1].events = POLLIN | POLLOUT;

	// Creating new user for client
	_state->create_unregistered_user( _clientSocket.fd );
	_activeConnections++;
}

void Application::removeClient( int fd )
{
	log_event::info( "Application: Client disconnected from socket", fd );
	try
	{
		cleanUpMessagesFromRemovedClient( fd );

		_state->remove_user( fd ); // Closes the client socket

		std::vector<pollfd>& client_fds = *_pollDescriptors;
		for ( int i = 1; i <= _activeConnections; i++ )
		{
			if ( client_fds[i].fd == fd )
			{
				client_fds.erase( client_fds.begin() + i );  // Remove the client from the vector
				_activeConnections--;
				break;
 		   	}
  		}
	}
	catch ( Context::CouldNotFindUserException & e )
	{
		log_event::warn( "Application: Context:", e.what() );
	}
}

void Application::cleanUpMessagesFromRemovedClient( int fd )
{
	if ( _pendingMessages.empty() ) 
	{
		return ;
	}
	// Remove queued messages for the disconnected client
    std::vector<IncomingIRCMessage>::iterator it = _pendingMessages.begin();
	while ( it != _pendingMessages.end() )
	{
		if ( it->client_fd == fd )
		{
        	it = _pendingMessages.erase( it );
		}
		else
		{
			it++;
		}
	}
}

void Application::handleIncomingIrcPayload( int fd )
{
	static std::string message_buffer;

	try
	{
		receiveCommands( fd, message_buffer );
	}
	catch ( Application::ClientDisconnectedException & e )
	{
		message_buffer = "";
		removeClient( fd );
	}
}

void Application::receiveCommands( int fd, std::string & message_buffer )
{
	while ( ! messageHasTerminator( message_buffer ) && ! sig::stopServer )
	{
		try
		{
			extractCommands( fd, message_buffer );
			processClientInput( fd, message_buffer );
		}
		catch ( Application::NoAvailablePayloadException & e )
		{
			break ;
		}
		catch ( Context::CouldNotFindUserException & e )
		{
			log_event::warn( "Application: Context:", e.what() );
		}
	}
}	

bool Application::messageHasTerminator( std::string & message_buffer )
{
	size_t terminator = message_buffer.find( "\r\n", 0 );
	if ( terminator == std::string::npos )
	{
		return ( false );
	}
	return ( true );
}

void Application::extractCommands( int fd, std::string & message_buffer )
{
	char buf[4096];
	memset( buf, 0, sizeof( buf ) );
	int bytes_recv = 0;

	bytes_recv = recv( fd, buf, sizeof( buf ), 0 );
	if ( bytes_recv == -1 )
	{
		if	( errno == EWOULDBLOCK || errno == EAGAIN )
		{
			throw NoAvailablePayloadException();
		}
		else
		{
			log_event::warn( "Application: Connection issue while receiving message from socket", fd );
			throw ClientDisconnectedException();
		}
	}
	if ( bytes_recv == 0 )
	{
		log_event::warn( "Application: read returned 0, read:", buf );
		throw ClientDisconnectedException();
	}
	message_buffer += std::string( buf );
}

void Application::processClientInput( int fd, std::string & message_buffer )
{
	size_t terminator = message_buffer.find( "\r\n", 0 );
	if ( terminator == std::string::npos )
	{
		return ;
	}
	size_t pos = 0;

	while ( terminator != std::string::npos )
	{
		std::string first_command = message_buffer.substr( pos, terminator + 2 - pos );
		log_event::command( fd, first_command );
		_state->handle_message( _state->get_user_by_socket( fd ), first_command );
		pos = terminator + 2;
		terminator = message_buffer.find( "\r\n", pos );
	}
	message_buffer = "";
}

void Application::sendMessageToClient( int socket, const std::string& message )
{
	IncomingIRCMessage newMessage;
	newMessage.client_fd = socket;
	newMessage.irc_payload = message;
	_pendingMessages.push_back( newMessage );
}

void Application::broadcastPendingMessages()
{
	std::vector<pollfd> &client_fds = *_pollDescriptors;

	for ( std::vector<IncomingIRCMessage>::const_iterator it = _pendingMessages.begin(); it != _pendingMessages.end(); ++it )
	{
		const IncomingIRCMessage& message = *it;
		int index = -1;
		for ( int i = 1; i <= _activeConnections; i++ )
		{
			if ( client_fds[i].fd == message.client_fd )
			{
				index = i;
				break;
			}
		}
		if ( index != -1 && client_fds[index].revents & POLLOUT )
		{
			ssize_t bytes_sent = send( message.client_fd, message.irc_payload.c_str(), message.irc_payload.length(), 0 );
			if ( bytes_sent == -1 )
			{
				log_event::warn( "Application: Error while sending message to socket", message.client_fd );
				removeClient( message.client_fd );
			}
		}
	}
	_pendingMessages.clear();
}
