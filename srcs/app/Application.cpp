#include "Application.hpp"


/*Initializes the server with the given port and password.

setUpServer() sets up the listening socket.

Password and ConnectionManager objects are dynamically allocated.

Note: Consider using smart pointers (like std::auto_ptr in C++98 or at least clear deallocation) to avoid memory leaks. */

Application::Application( int port, const std::string &password ) 
	: _port( port ), _activeConnections(0)
{
	setUpServer();
	_auth = new Password( password );
	_state = new ConnectionManager( *this, *_auth );
}


/* Cleanly shuts down the server, closes the socket, and deallocates memory.

Risk: delete _pollDescriptors; assumes it's allocated. If not properly initialized or reassigned elsewhere, this could cause undefined behavior.
Suggestion: Set these pointers to NULL after deletion to avoid dangling pointers. */

Application::~Application()
{
	log_event::info( "Shutting down application" );
	close( _serverSocket.fd );
	delete _pollDescriptors;
	delete _auth;
	delete _state;
}


/*
Sets socket to non-blocking mode, as required.
- Correct use of fcntl() with O_NONBLOCK on macOS per subject.

Ensures valid port range.
- Subject Violation: The subject does not require this restriction. In fact, it says your program should accept any port passed as argument. This may cause your server to fail evaluation if the evaluator uses port 6667 (standard for IRC).
- Fix: Remove port range restriction and allow any valid port number.
*/

void Application::setUpServer()
{
	// log_event::info( "Application: Initializing server..." );
	log_event::info( "Starting Server Setup" );

	_serverSocket.fd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( _serverSocket.fd == -1 )
	{
		throw std::runtime_error( "Failed to create socket!" );
	}
	log_event::info( "Setting non-blocking mode for server socket" );
	int currentFlags = fcntl( _serverSocket.fd, F_GETFL, 0 );

	fcntl( _serverSocket.fd, F_SETFL, currentFlags | O_NONBLOCK );

	log_event::info( "Connecting to port", _port );
	_serverSocket.address.sin_family = AF_INET;
	// if ( _port < 6660 || _port > 7000 )
	// {
	// 	throw( std::runtime_error( "Application: Invalid port: port must be between 6660 and 7000" ) );
	// }
	_serverSocket.address.sin_port = htons( _port );
	_serverSocket.address.sin_addr.s_addr = htonl( INADDR_ANY );

	log_event::info( "Binding Socket" );
	if ( bind( _serverSocket.fd, ( struct sockaddr * )&_serverSocket.address, sizeof( _serverSocket.address ) ) == -1 )
	{
		throw(std::runtime_error("Port Binding Failed"));
	}
	log_event::info( "Listening for connections" );
	if ( listen(_serverSocket.fd, SOMAXCONN ) == -1 )
	{
		throw std::runtime_error( "Unable to listen on socket" );
	}
	_pollDescriptors = new std::vector<pollfd>( MAX_CLIENTS + 1 );
	signal( SIGINT, sig::signalHandler );
	log_event::info( "Server setup complete" );
}

void Application::launchServer()
{
	std::vector<pollfd> &client_fds = *_pollDescriptors;
	client_fds[0].fd = _serverSocket.fd;
	client_fds[0].events = POLLIN;

	// _activeConnections = 0; // keep track of number of connected clients

	log_event::info( "Entering main loop" );

	while ( !sig::stopServer )
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
			log_event::warn( "Server stopping gracefully" );
			break;
	   }
	}
	log_event::info( "Exited main loop" );
}

void Application::readFromClients()
{
	std::vector<pollfd> &client_fds = *_pollDescriptors;
	for ( int i = 1; i <= _activeConnections && !sig::stopServer; ++i )
	{
		if ( client_fds[i].fd != -1 && client_fds[i].revents & POLLIN )
		{
			handleIncomingIrcPayload(client_fds[i].fd);
		}
	}
}

void Application::eventLoop()
{
	// std::vector<pollfd> &client_fds = *_pollDescriptors;

	int num_ready = poll( _pollDescriptors->data(), _activeConnections + 1, -1 );
	if ( num_ready == -1 && sig::stopServer )
	{
		log_event::warn( "Signal Interrupted" );
		throw Application::ServerShutdownException();
	}
	else if ( num_ready == -1)
	{
		throw std::runtime_error( "Polling failed" );
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

	// log_event::info( "Application: Accepting client call..." );
	_clientSocket.fd = accept( _serverSocket.fd, ( struct sockaddr * )&_clientSocket.address, &clientSize );
	if ( _clientSocket.fd == -1 )
	{
		log_event::warn( "Client connection attempt Failed" );
		return;
	}
	log_event::info( "New client connection with fd", _clientSocket.fd );

	// Set the client socket to non-blocking
	int flags = fcntl( _clientSocket.fd, F_GETFL, 0 );
	fcntl( _clientSocket.fd, F_SETFL, flags | O_NONBLOCK );

	// add new client to the list of file descriptors to monitor
	if ( _activeConnections == MAX_CLIENTS )
	{
		throw std::runtime_error( "Maximum client Limit reached" );
	}
	client_fds[_activeConnections + 1].fd = _clientSocket.fd;
	client_fds[_activeConnections + 1].events = POLLIN | POLLOUT;

	// Creating new user for client
	_state->registerPendingUser( _clientSocket.fd );
	_activeConnections++;
}

void Application::removeClient( int fd )
{
	log_event::info( "Disconnecting client", fd );
	try
	{
		cleanUpMessagesFromRemovedClient( fd );

		_state->disconnectUserBySocket( fd ); // Closes the client socket

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
	catch ( ConnectionManager::UserNotFoundException & e )
	{
		log_event::warn( "Application: ConnectionManager:", e.what() );
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
	static std::string messageBuf;

	try
	{
		receiveCommands( fd, messageBuf );
	}
	catch ( Application::ClientDisconnectedException & e )
	{
		messageBuf = "";
		removeClient( fd );
	}
}

void Application::receiveCommands( int fd, std::string & messageBuf )
{
	while ( ! messageHasTerminator( messageBuf ) && ! sig::stopServer )
	{
		try
		{
			extractCommands( fd, messageBuf );
			processClientInput( fd, messageBuf );
		}
		catch ( Application::NoAvailablePayloadException & e )
		{
			break ;
		}
		catch ( ConnectionManager::UserNotFoundException & e )
		{
			log_event::warn( "Application: ConnectionManager:", e.what() );
		}
	}
}	

bool Application::messageHasTerminator( std::string & messageBuf )
{
	size_t terminator = messageBuf.find( "\r\n", 0 );
	if ( terminator == std::string::npos )
	{
		return ( false );
	}
	return ( true );
}

void Application::extractCommands( int fd, std::string & messageBuf )
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
			log_event::warn( "Error occurs while receiving message from socket", fd );
			throw ClientDisconnectedException();
		}
	}
	if ( bytes_recv == 0 )
	{
		// log_event::warn( "Application: read returned 0, read:", buf );
		throw ClientDisconnectedException();
	}
	messageBuf += std::string( buf );
}

void Application::processClientInput( int fd, std::string & messageBuf )
{
	size_t terminator = messageBuf.find( "\r\n", 0 );
	if ( terminator == std::string::npos )
	{
		return ;
	}
	size_t pos = 0;

	while ( terminator != std::string::npos )
	{
		std::string first_command = messageBuf.substr( pos, terminator + 2 - pos );
		log_event::command( fd, first_command );
		_state->processClientCommand( _state->getUserBySocket( fd ), first_command );
		pos = terminator + 2;
		terminator = messageBuf.find( "\r\n", pos );
	}
	messageBuf = "";
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
				log_event::warn( "Message to socket fail", message.client_fd );
				removeClient( message.client_fd );
			}
		}
	}
	_pendingMessages.clear();
}
