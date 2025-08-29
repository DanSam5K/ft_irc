# ft_irc

## Overview

**ft_irc** is a C++ IRC server implementation designed to comply with the 42 Network subject requirements. It supports multiple simultaneous clients, channel management, operator commands, and works with both standard IRC clients (such as HexChat, irssi) and simple tools like `nc` or `telnet`.

---

## Features

- **Non-blocking TCP/IP server** using poll() (or equivalent)
- **Authentication** with password, nickname, and username
- **Channel management**: join, part, list, topic
- **Private messaging**: PRIVMSG, NOTICE
- **Operator commands**: KICK, INVITE, TOPIC, MODE
  - `+i/-i`: Invite-only channel
  - `+t/-t`: Restrict topic changes to operators
  - `+k/-k`: Channel key (password)
  - `+o/-o`: Operator privilege
  - `+l/-l`: User limit
- **Multiple clients**: Handles many connections without blocking
- **Partial command aggregation**: Handles fragmented input as per RFC
- **Graceful disconnects**: Unexpected disconnects do not affect other clients
- **Compatible with reference IRC clients** (HexChat, irssi, etc.)

---

## Usage

### Build

```sh
make
```

### Run

```sh
./ircserv <port> <password>
```
- `<port>`: Port to listen on (e.g., 6667)
- `<password>`: Password required for clients to connect

### Connect

#### Using nc/telnet

```sh
nc -C localhost 6667
```
Then type:
```
PASS <password>
NICK <nickname>
USER <username> 0 * :<realname>
JOIN #channel
```

#### Using irssi/HexChat

- Add a new server: `localhost` port `6667`, password as specified.
- Connect and use standard IRC commands.

---

## Operator Commands

- **KICK**: Remove a user from a channel
- **INVITE**: Invite a user to a channel
- **TOPIC**: Change/view channel topic
- **MODE**: Change channel modes (`+i`, `+t`, `+k`, `+o`, `+l`)
- Only operators can use these commands; regular users will receive error replies if they attempt them.

---

## Project Structure

```
srcs/
  app/                # Server setup and connection management
  irc/                # IRC protocol logic (channels, users, commands, parsing)
  reply/              # Reply message formatting
  utilities/          # Logging and signal handling
includes/             # Header files
Makefile              # Build instructions
```

---

## Requirements

- Only one poll()/select()/epoll()/kqueue() for all I/O
- All sockets are non-blocking (`fcntl(fd, F_SETFL, O_NONBLOCK)`)
- No forking
- No IRC client implementation
- No server-to-server communication
- Must aggregate partial commands before processing
- Must handle multiple clients without blocking
- Must work with a reference IRC client (HexChat, irssi, etc.)
- Must implement: PASS, NICK, USER, JOIN, PART, LIST, TOPIC, PRIVMSG, NOTICE, KICK, INVITE, MODE, QUIT, WHO

---

## Testing

- Connect with multiple clients (nc, irssi, HexChat)
- Authenticate, set nickname/username, join channels
- Send/receive messages, test operator commands
- Try sending partial commands (with Ctrl+D in nc)
- Disconnect clients unexpectedly and verify server stability
- Flood channels and resume clients to test aggregation and memory management

---

## References

- [RFC 1459](https://www.rfc-editor.org/rfc/rfc1459.html)
- [Modern IRC Docs](https://modern.ircdocs.horse/)

---

## Authors

- Your Name(s) Here

---

## License

MIT (or your chosen license)