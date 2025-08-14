# ft_irc

# Project Program Subject

Chapter IV
Mandatory Part
Program name ircserv
Turn in files Makefile, *.{h, hpp}, *.cpp, *.tpp, *.ipp,
an optional configuration file
Makefile NAME, all, clean, fclean, re
Arguments port: The listening port
password: The connection password
External functs. Everything in C++ 98.
socket, close, setsockopt, getsockname,
getprotobyname, gethostbyname, getaddrinfo,
freeaddrinfo, bind, connect, listen, accept, htons,
htonl, ntohs, ntohl, inet_addr, inet_ntoa, send,
recv, signal, sigaction, lseek, fstat, fcntl, poll
(or equivalent)
Libft authorized n/a
Description An IRC server in C++ 98
You are required to develop an IRC server using the C++ 98 standard.
You must not develop an IRC client.
You must not implement server-to-server communication.
Your executable will be run as follows:
./ircserv <port> <password>
• port: The port number on which your IRC server will be listening for incoming
IRC connections.
• password: The connection password. It will be needed by any IRC client that tries
to connect to your server.
Even though poll() is mentioned in the subject and the evaluation
scale, you may use any equivalent such as select(), kqueue(), or
epoll().
6
ft_irc Internet Relay Chat
IV.1 Requirements
• The server must be capable of handling multiple clients simultaneously without
hanging.
• Forking is prohibited. All I/O operations must be non-blocking.
• Only 1 poll() (or equivalent) can be used for handling all these operations (read,
write, but also listen, and so forth).
Because you have to use non-blocking file descriptors, it is
possible to use read/recv or write/send functions with no poll()
(or equivalent), and your server wouldn’t be blocking.
However, it would consume more system resources.
Therefore, if you attempt to read/recv or write/send in any file
descriptor without using poll() (or equivalent), your grade will be
0.
• Several IRC clients exist. You have to choose one of them as a reference. Your
reference client will be used during the evaluation process.
• Your reference client must be able to connect to your server without encountering
any error.
• Communication between client and server has to be done via TCP/IP (v4 or v6).
• Using your reference client with your server must be similar to using it with any
official IRC server. However, you only have to implement the following features:
◦ You must be able to authenticate, set a nickname, a username, join a channel,
send and receive private messages using your reference client.
◦ All the messages sent from one client to a channel have to be forwarded to
every other client that joined the channel.
◦ You must have operators and regular users.
◦ Then, you have to implement the commands that are specific to channel
operators:
∗ KICK - Eject a client from the channel
∗ INVITE - Invite a client to a channel
∗ TOPIC - Change or view the channel topic
∗ MODE - Change the channel’s mode:
· i: Set/remove Invite-only channel
· t: Set/remove the restrictions of the TOPIC command to channel
operators
· k: Set/remove the channel key (password)
7
ft_irc Internet Relay Chat
· o: Give/take channel operator privilege
· l: Set/remove the user limit to channel
• Of course, you are expected to write a clean code.
IV.2 For MacOS only
Since MacOS does not implement write() in the same way as other Unix
OSes, you are permitted to use fcntl().
You must use file descriptors in non-blocking mode in order to get a
behavior similar to the one of other Unix OSes.
However, you are allowed to use fcntl() only as follows:
fcntl(fd, F_SETFL, O_NONBLOCK);
Any other flag is forbidden.
IV.3 Test example
Verify every possible error and issue, such as receiving partial data, low bandwidth, etc.
To ensure that your server correctly processes all data sent to it, the following simple
test using nc can be performed:
\$> nc -C 127.0.0.1 6667
com^Dman^Dd
\$>
Use ctrl+D to send the command in several parts: ’com’, then ’man’, then ’d\n’.
In order to process a command, you have to first aggregate the received packets in
order to rebuild it.
8
Chapter V

Bonus part
Here are additional features you may add to your IRC server to make it resemble an
actual IRC server more closely:
• Handle file transfer.
• A bot.
The bonus part will only be assessed if the mandatory part is
PERFECT. Perfect means the mandatory part has been integrally done
and works without malfunctioning. If you have not passed ALL the
mandatory requirements, your bonus part will not be evaluated at all.

# Evaluations Guidelines to confirm if the project works expected

Mandatory Part
Basic checks

There is a Makefile, the project compiles correctly with the required options, is written in C++, and the executable is called as expected.
Ask and check how many poll() (or equivalent) are present in the code. There must be only one.
Verify that the poll() (or equivalent) is called every time before each accept, read/recv, write/send. After these calls, errno should not be used to trigger specific action (e.g. like reading again after errno == EAGAIN).
Verify that each call to fcntl() is done as follows: fcntl(fd, F_SETFL, O_NONBLOCK); Any other use of fcntl() is forbidden.
If any of these points is wrong, the evaluation ends now and the final mark is 0.
Networking

Check the following requirements:

The server starts, and listens on all network interfaces on the port given from the command line.
Using the 'nc' tool, you can connect to the server, send commands, and the server answers you back.
Ask the team what is their reference IRC client.
Using this IRC client, you can connect to the server.
The server can handle multiple connections at the same time. The server should not block. It should be able to answer all demands. Do some test with the IRC client and nc at the same time.
Join a channel thanks to the appropriate command. Ensure that all messages from one client on that channel are sent to all other clients that joined the channel.
Networking specials

Network communications can be disturbed by many strange situations.

Just like in the subject, using nc, try to send partial commands. Check that the server answers correctly. With a partial command sent, ensure that other connections still run fine.
Unexpectedly kill a client. Then check that the server is still operational for the other connections and for any new incoming client.
Unexpectedly kill a nc with just half of a command sent. Check again that the server is not in an odd state or blocked.
Stop a client (^-Z) connected on a channel. Then flood the channel using another client. The server should not hang. When the client is live again, all stored commands should be processed normally. Also, check for memory leaks during this operation.
Client Commands basic

With both nc and the reference IRC client, check that you can authenticate, set a nickname, a username, join a channel. This should be fine (you should have already done this previously).
Verify that private messages (PRIVMSG) are fully functional with different parameters.
Client Commands channel operator

With both nc and the reference IRC client, check that a regular user does not have privileges to do channel operator actions. Then test with an operator. All the channel operation commands should be tested (remove one point for each feature that is not working).
Rate it from 0 (failed) through 5 (excellent)

Bonus part
Evaluate the bonus part if, and only if, the mandatory part has been entirely and perfectly done, and the error management handles unexpected or bad usage. In case all the mandatory points were not passed during the defense, bonus points must be totally ignored.

File transfer

File transfer works with the reference IRC client.

A small bot

There's an IRC bot.

