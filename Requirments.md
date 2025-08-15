1

2

3

4

5

6

7

8

9

10

11

12

13

Functions Used? Times used Reason
socket Yes 1 _serverSocketFd = socket(AF_INET, SOCK_STREAM, 0); // -> create the server socket
close Yes 5+
setsockopt Yes 1 setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0
getsockname No
getprotobyname No
gethostbyname No
getaddrinfo No
freeaddrinfo No
bind Yes 1 bind(_serverSocketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))
connect No
listen Yes 1 listen(_serverSocketFd, SOMAXCONN)
accept Yes 1 accept(_serverSocketFd, (sockaddr *)&(clientAddr), &clientLen)
htons Yes 1 serverAddr.sin_port = htons(this->_port); // Convert the port to network byte order (big endian)
htonl No
ntohs No
ntohl No
inet_addr No
inet_ntoa Yes 1 client.setIpAdd(inet_ntoa((clientAddr.sin_addr)));
send Yes 5+
recv Yes 1 ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // receive the data
signal Yes 4
sigaction No
lseek No
fstat No
fcntl Yes 3
poll (or equivalen Yes 1 if ((poll(&_pollFds[0], _pollFds.size(), -1) == -1) && Server::Signal == false)
_serverSocketFd = socket(AF_INET, SOCK_STREAM, 0); // -> create the server socket
setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0
bind(_serverSocketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))
accept(_serverSocketFd, (sockaddr *)&(clientAddr), &clientLen)
serverAddr.sin_port = htons(this->_port); // Convert the port to network byte order (big endian)
client.setIpAdd(inet_ntoa((clientAddr.sin_addr)));
ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // receive the data
if ((poll(&_pollFds[0], _pollFds.size(), -1) == -1) && Server::Signal == false)
Requirement
You must not develop an IRC client
You must not implement server-to-server communication.
The server must be capable of handling multiple clients simultaneously without hanging.
Forking is prohibited. All I/O operations must be non-blocking.
Communication between client and server has to be done via TCP/IP (v4 or v6).
authenticate
set a nickname
set a username
join a channel
send and receive private messages
KICK
INVITE
TOPIC
MODE
QUIT
WHO
LIST
you are allowed to use fcntl() only as follows: fcntl(fd, F_SETFL, O_NONBLOCK); Any other flag is forbidden.
In order to process a command, you have to first aggregate the received packets in order to rebuild it.
Only 1 poll() (or equivalent) can be used for handling all these operations (read, write, but also listen, 
and so forth).
Several IRC clients exist. You have to choose one of them as a reference. Your reference client will 
be used during the evaluation process.
Your reference client must be able to connect to your server without encountering
any error.
Using your reference client with 
your server must be similar to 
using it with any
official IRC server. However, you 
only have to implement the 
following features:
All the messages sent from one client to a 
channel have to be forwarded to
every other client that joined the channel.
You must have operators and regular 
users.
Implement the commands that are specific 
to channel
operators:
There is a Makefile, the project compiles correctly, is written in C++, and the executable is called "ircserv".
Ask and check how many poll() (or equivalent) are present in the code. There must be only one.
Verify that the poll() (or equivalent) is called every time before each accept, read/recv, write/send. After these 
calls, errno should not be used to trigger specific action (e.g. like reading again after errno == EAGAIN).
Verify that each call to fcntl() is done as follows: fcntl(fd, F_SETFL, O_NONBLOCK); Any other use of fcntl() 
is forbidden.
The server starts, and listens on all network interfaces on the port given from the command line
Using the 'nc' tool, you can connect to the server, send commands, and the server answers you back.
Ask the team what is their reference IRC client.
Using this IRC client, you can connect to the server.
The server can handle multiple connections at the same time. The server should not block. It should be able 
to answer all demands. Do some test with the IRC client and nc at the same time.
Join a channel thanks to the appropriate command. Ensure that all messages from one client on that channel 
are sent to all other clients that joined the channel.
Just like in the subject, using nc, try to send partial commands. Check that the server answers correctly. With 
a partial command sent, ensure that other connections still run fine.
Unexpectedly kill a client. Then check that the server is still operational for the other connections and for any 
new incoming client.
Unexpectedly kill a nc with just half of a command sent. Check again that the server is not in an odd state or 
blocked.
Stop a client (^-Z) connected on a channel. Then flood the channel using another client. The server should 
not hang. When the client is live again, all stored commands should be processed normally. Also, check for 
memory leaks during this operation.
With both nc and a regular IRC client, check that you can authenticate, set a nickname, a username, join a 
channel. This should be ok (you should have already done this previously).
Verify that private messages (PRIVMSG) and notices (NOTICE) are fully functional with different parameters.
Check that a regular user does not have privileges to do operator actions. Then test with an operator. All the 
channel operation commands should be tested (remove one point for each feature that is not working).
With both nc and the reference IRC client, check that a regular user does not have privileges to do channel 
operator actions. Then test with an operator. All the channel operation commands should be tested (remove 
one point for each feature that is not working).
Requirement CategoryCorrect? Comment
You must not develop an IRC client Subject Yes
You must not implement server-to-server communication. Subject Yes
The server must be capable of handling multiple clients simultaneously without hanging. Subject Yes Paulo: I created a tester in which I test that. I tested even wiht 200 and all of them were connected fine.
Forking is prohibited. All I/O operations must be non-blocking. Subject Yes No fork() function
Subject Yes
Subject Yes Hexchat
Subject Yes
Communication between client and server has to be done via TCP/IP (v4 or v6). Subject Yes We are using IPv4
Subject Yes
Subject Yes
Subject Yes
Subject Yes
Subject Yes
Subject Yes
Subject Yes
KICK Subject
INVITE Subject
TOPIC Subject
i Subject Yes
t Subject Yes
k Subject
o Subject Yes
l Subject
you are allowed to use fcntl() only as follows: fcntl(fd, F_SETFL, O_NONBLOCK); Any other flag is forbidden. Subject
In order to process a command, you have to first aggregate the received packets in order to rebuild it. Subject
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes Changed recently. I was using one to give to flag wrongly before
Only 1 poll() (or equivalent) can be used for handling all these operations (read, write, but also listen, 
Several IRC clients exist. You have to choose one of them as a reference. Your reference client will 
Your reference client must be able to connect to your server without encountering
There is a Makefile, the project compiles correctly, is written in C++, and the executable is called "ircserv".
Ask and check how many poll() (or equivalent) are present in the code. There must be only one.
Verify that the poll() (or equivalent) is called every time before each accept, read/recv, write/send. After these 
calls, errno should not be used to trigger specific action (e.g. like reading again after errno == EAGAIN).
Verify that each call to fcntl() is done as follows: fcntl(fd, F_SETFL, O_NONBLOCK); Any other use of fcntl() 
The server starts, and listens on all network interfaces on the port given from the command line Eval sheet Yes
Using the 'nc' tool, you can connect to the server, send commands, and the server answers you back. Eval sheet Yes
Ask the team what is their reference IRC client. Eval sheet Yes HexChat
Using this IRC client, you can connect to the server. Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
https://www.rfc-editor.org/rfc/rfc1459.htmlhttps://modern.ircdocs.horse/
The server can handle multiple connections at the same time. The server should not block. It should be able 
Join a channel thanks to the appropriate command. Ensure that all messages from one client on that channel 
Just like in the subject, using nc, try to send partial commands. Check that the server answers correctly. With 
Unexpectedly kill a client. Then check that the server is still operational for the other connections and for any 
Unexpectedly kill a nc with just half of a command sent. Check again that the server is not in an odd state or 
Stop a client (^-Z) connected on a channel. Then flood the channel using another client. The server should 
not hang. When the client is live again, all stored commands should be processed normally. Also, check for 
With both nc and a regular IRC client, check that you can authenticate, set a nickname, a username, join a 
Verify that private messages (PRIVMSG) and notices (NOTICE) are fully functional with different parameters.
Check that a regular user does not have privileges to do operator actions. Then test with an operator. All the 
channel operation commands should be tested (remove one point for each feature that is not working).
With both nc and the reference IRC client, check that a regular user does not have privileges to do channel 
operator actions. Then test with an operator. All the channel operation commands should be tested (remove 
Paulo: I created a tester in which I test that. I tested even wiht 200 and all of them were connected fine.
Changed recently. I was using one to give to flag wrongly before
Requirement
You must not develop an IRC client
You must not implement server-to-server communication.
The server must be capable of handling multiple clients simultaneously without hanging.
Forking is prohibited. All I/O operations must be non-blocking.
Communication between client and server has to be done via TCP/IP (v4 or v6).
authenticate
set a nickname
set a username
join a channel
send and receive private messages
KICK
INVITE
TOPIC
MODE
QUIT
WHO
LIST
you are allowed to use fcntl() only as follows: fcntl(fd, F_SETFL, O_NONBLOCK); Any other flag is forbidden.
In order to process a command, you have to first aggregate the received packets in order to rebuild it.
The server starts, and listens on all network interfaces on the port given from the command line
Only 1 poll() (or equivalent) can be used for handling all these operations (read, write, but also listen, 
and so forth).
Several IRC clients exist. You have to choose one of them as a reference. Your reference client will 
be used during the evaluation process.
Your reference client must be able to connect to your server without encountering
any error.
Using your reference client with 
your server must be similar to 
using it with any
official IRC server. However, you 
only have to implement the 
following features:
All the messages sent from one client to a 
channel have to be forwarded to
every other client that joined the channel.
You must have operators and regular 
users.
Implement the commands that are specific 
to channel
operators:
There is a Makefile, the project compiles correctly, is written in C++, and the executable is called "ircserv".
Ask and check how many poll() (or equivalent) are present in the code. There must be only one.
Verify that the poll() (or equivalent) is called every time before each accept, read/recv, write/send. After these 
calls, errno should not be used to trigger specific action (e.g. like reading again after errno == EAGAIN).
Verify that each call to fcntl() is done as follows: fcntl(fd, F_SETFL, O_NONBLOCK); Any other use of fcntl() 
is forbidden.
Using the 'nc' tool, you can connect to the server, send commands, and the server answers you back.
Ask the team what is their reference IRC client.
Using this IRC client, you can connect to the server.
The server can handle multiple connections at the same time. The server should not block. It should be able 
to answer all demands. Do some test with the IRC client and nc at the same time.
Join a channel thanks to the appropriate command. Ensure that all messages from one client on that channel 
are sent to all other clients that joined the channel.
Just like in the subject, using nc, try to send partial commands. Check that the server answers correctly. With 
a partial command sent, ensure that other connections still run fine.
Unexpectedly kill a client. Then check that the server is still operational for the other connections and for any 
new incoming client.
Unexpectedly kill a nc with just half of a command sent. Check again that the server is not in an odd state or 
blocked.
Stop a client (^-Z) connected on a channel. Then flood the channel using another client. The server should 
not hang. When the client is live again, all stored commands should be processed normally. Also, check for 
memory leaks during this operation.
With both nc and a regular IRC client, check that you can authenticate, set a nickname, a username, join a 
channel. This should be ok (you should have already done this previously).
Verify that private messages (PRIVMSG) and notices (NOTICE) are fully functional with different parameters.
Check that a regular user does not have privileges to do operator actions. Then test with an operator. All the 
channel operation commands should be tested (remove one point for each feature that is not working).
With both nc and the reference IRC client, check that a regular user does not have privileges to do channel 
operator actions. Then test with an operator. All the channel operation commands should be tested (remove 
one point for each feature that is not working).
Requirement CategoryCorrect? Comment
You must not develop an IRC client Subject Yes
You must not implement server-to-server communication. Subject Yes
The server must be capable of handling multiple clients simultaneously without hanging. Subject Yes Paulo: I created a tester in which I test that. I tested even wiht 200 and all of them were connected fine.
Forking is prohibited. All I/O operations must be non-blocking. Subject Yes No fork() function
Subject Yes
Subject Yes Hexchat
Subject Yes
Communication between client and server has to be done via TCP/IP (v4 or v6). Subject Yes We are using IPv4
Subject Yes Using Pass
Subject Yes Nick is still mi
Subject Yes Still things to do it - Paulo
Subject Yes
Subject Yes
Subject Yes
Subject Yes
KICK Subject Test Test without being operator
INVITE Subject Yes
TOPIC Subject Yes
i Subject Yes
t Subject Yes Test without being operator
k Subject Test without being operator
o Subject Yes Test without being operator
l Subject Test without being operator
Yes
Yes
Yes
you are allowed to use fcntl() only as follows: fcntl(fd, F_SETFL, O_NONBLOCK); Any other flag is forbidden. Subject Yes
In order to process a command, you have to first aggregate the received packets in order to rebuild it. Subject Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes Changed recently. I was using one to give to flag wrongly before
The server starts, and listens on all network interfaces on the port given from the command line Eval sheet Yes
Only 1 poll() (or equivalent) can be used for handling all these operations (read, write, but also listen, 
Several IRC clients exist. You have to choose one of them as a reference. Your reference client will 
Your reference client must be able to connect to your server without encountering
There is a Makefile, the project compiles correctly, is written in C++, and the executable is called "ircserv".
Ask and check how many poll() (or equivalent) are present in the code. There must be only one.
Verify that the poll() (or equivalent) is called every time before each accept, read/recv, write/send. After these 
calls, errno should not be used to trigger specific action (e.g. like reading again after errno == EAGAIN).
Verify that each call to fcntl() is done as follows: fcntl(fd, F_SETFL, O_NONBLOCK); Any other use of fcntl() 
Using the 'nc' tool, you can connect to the server, send commands, and the server answers you back. Eval sheet Yes
Ask the team what is their reference IRC client. Eval sheet Yes HexChat
Using this IRC client, you can connect to the server. Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes Change message when server is disconnected -Paulo
Eval sheet Yes
Eval sheet Yes
Eval sheet Yes
Eval sheet Need to implement NOTICE
Eval sheet Yes
Eval sheet Yes
https://modern.ircdocs.horse/
The server can handle multiple connections at the same time. The server should not block. It should be able 
Join a channel thanks to the appropriate command. Ensure that all messages from one client on that channel 
Just like in the subject, using nc, try to send partial commands. Check that the server answers correctly. With 
Unexpectedly kill a client. Then check that the server is still operational for the other connections and for any 
Unexpectedly kill a nc with just half of a command sent. Check again that the server is not in an odd state or 
Stop a client (^-Z) connected on a channel. Then flood the channel using another client. The server should 
not hang. When the client is live again, all stored commands should be processed normally. Also, check for 
With both nc and a regular IRC client, check that you can authenticate, set a nickname, a username, join a 
Verify that private messages (PRIVMSG) and notices (NOTICE) are fully functional with different parameters.
Check that a regular user does not have privileges to do operator actions. Then test with an operator. All the 
channel operation commands should be tested (remove one point for each feature that is not working).
With both nc and the reference IRC client, check that a regular user does not have privileges to do channel 
operator actions. Then test with an operator. All the channel operation commands should be tested (remove 
Paulo: I created a tester in which I test that. I tested even wiht 200 and all of them were connected fine.
Changed recently. I was using one to give to flag wrongly before
Change message when server is disconnected -Paulo
https://modern.ircdocs.horse/