# ft_irc Manual Testing Guide

This comprehensive testing guide ensures your IRC server meets all evaluation requirements and passes the mandatory part perfectly.

## Prerequisites

- **Reference IRC Client**: irssi
- **Testing Tools**: `nc` (netcat), multiple terminal windows
- **Server**: Compiled `ircserv` executable
- **Test Port**: Use port 6667 (standard IRC port)
- **Test Password**: Use "password123" for consistency

## Test Setup

### 1. Compile and Start Server
```bash
# Compile the project
make

# Start the server (use port 6667 and password "password123")
./ircserv 6667 password123
```

**Expected Result**: Server starts without errors and displays listening message.

---

## PART 1: BASIC CHECKS (Critical - Must Pass All)

### Test 1.1: Compilation and Makefile
```bash
# Test Makefile targets
make clean
make all
make fclean
make re

# Verify executable name
ls -la | grep ircserv
```

**Pass Criteria**: 
- All Makefile targets work
- Executable is named `ircserv`
- No compilation errors

### Test 1.2: Code Review - poll() Usage
```bash
# Search for poll() calls in code
grep -r "poll(" srcs/ includes/
```

**Pass Criteria**: 
- Only ONE poll() call found
- poll() called before each accept, read/recv, write/send
- No errno usage after these calls

### Test 1.3: Code Review - fcntl() Usage
```bash
# Search for fcntl() calls
grep -r "fcntl(" srcs/ includes/
```

**Pass Criteria**: 
- fcntl() only used as: `fcntl(fd, F_SETFL, O_NONBLOCK)`
- No other fcntl() flags used

---

## PART 2: NETWORKING TESTS

### Test 2.1: Server Startup and Listening
```bash
# Terminal 1: Start server
./ircserv 6667 password123

# Terminal 2: Check if server is listening
netstat -an | grep 6667
# OR
lsof -i :6667
```

**Pass Criteria**: Server listens on all interfaces (0.0.0.0:6667)

### Test 2.2: Basic nc Connection
```bash
# Terminal 2: Connect with nc
nc 127.0.0.1 6667

# Type commands:
PASS password123
NICK testuser
USER testuser 0 * :Test User
```

**Pass Criteria**: 
- Connection established
- Server responds to commands
- No connection errors

### Test 2.3: IRC Client Connection (irssi)
```bash
# Start irssi
irssi

# In irssi, connect to server:
/connect 127.0.0.1 6667 password123

# Set nickname:
/nick testuser
```

**Pass Criteria**: 
- Successful connection
- No error messages
- Can authenticate properly

### Test 2.4: Multiple Simultaneous Connections
```bash
# Terminal 2: nc connection 1
nc 127.0.0.1 6667

# Terminal 3: nc connection 2  
nc 127.0.0.1 6667

# Terminal 4: irssi connection
irssi
/connect 127.0.0.1 6667 password123
/nick user3

# In each connection, authenticate:
PASS password123
NICK user1  # (use different nicks: user1, user2, user3)
USER user1 0 * :User One
```

**Pass Criteria**: 
- All connections work simultaneously
- Server doesn't hang or block
- Each client can send commands independently

### Test 2.5: Channel Communication
```bash
# In each connected client:
JOIN #testchannel

# From user1:
PRIVMSG #testchannel :Hello from user1

# Check: message appears in user2 and user3
```

**Pass Criteria**: Messages sent to channel are forwarded to all channel members

---

## PART 3: NETWORKING SPECIALS (Stress Tests)

### Test 3.1: Partial Commands (Critical Test)
```bash
# Terminal 2: Connect with nc
nc -C 127.0.0.1 6667

# Send partial command using Ctrl+D:
# Type "com" then Ctrl+D
# Type "man" then Ctrl+D  
# Type "d" then Enter
```

**Pass Criteria**: 
- Server handles partial commands correctly
- Other connections remain functional
- Server reconstructs complete command

### Test 3.2: Unexpected Client Kill
```bash
# Terminal 2: Connect and authenticate
nc 127.0.0.1 6667
PASS password123
NICK killtest
USER killtest 0 * :Kill Test

# Terminal 3: Another client
nc 127.0.0.1 6667
PASS password123
NICK survivor
USER survivor 0 * :Survivor

# Kill Terminal 2 process (Ctrl+C or kill -9)
# Test Terminal 3 still works:
JOIN #test
PRIVMSG #test :Still alive
```

**Pass Criteria**: 
- Server remains operational after client disconnect
- Other connections unaffected
- New clients can still connect

### Test 3.3: Half-Command Kill
```bash
# Terminal 2: Connect and start typing command
nc 127.0.0.1 6667
PASS password123
NICK halfcmd
USER halfcmd 0 * :Half Command
PRIVMSG #test :This is only half a comm

# Kill nc process before completing command (Ctrl+C)
# Test server with new connection:
nc 127.0.0.1 6667
PASS password123
NICK newuser
USER newuser 0 * :New User
```

**Pass Criteria**: 
- Server not in odd state
- Server not blocked
- New connections work normally

### Test 3.4: Client Suspension Test
```bash
# Terminal 2: Connect to channel
nc 127.0.0.1 6667
PASS password123
NICK suspended
USER suspended 0 * :Suspended User
JOIN #flood

# Terminal 3: Flood sender
nc 127.0.0.1 6667  
PASS password123
NICK flooder
USER flooder 0 * :Flooder
JOIN #flood

# Suspend Terminal 2 (Ctrl+Z)
# In Terminal 3, flood the channel:
for i in {1..50}; do echo "PRIVMSG #flood :Flood message $i"; done

# Resume Terminal 2 (fg command)
fg
```

**Pass Criteria**: 
- Server doesn't hang during flood
- Suspended client receives all messages when resumed
- No memory leaks (monitor with `top` or `htop`)

---

## PART 4: CLIENT COMMANDS BASIC

### Test 4.1: Authentication Flow
```bash
# Test with nc:
nc 127.0.0.1 6667

# Correct sequence:
PASS password123
NICK testuser
USER testuser 0 * :Test User

# Test with irssi:
irssi
/connect 127.0.0.1 6667 password123
/nick testuser
```

**Pass Criteria**: 
- Authentication works with both nc and irssi
- Proper welcome messages received
- No authentication errors

### Test 4.2: Private Messages (PRIVMSG)
```bash
# Setup two clients (user1 and user2)

# From user1:
PRIVMSG user2 :Hello user2
PRIVMSG user2 :How are you?

# From user2:  
PRIVMSG user1 :Hi there!

# Test channel messages:
JOIN #test
PRIVMSG #test :Channel message

# Test invalid targets:
PRIVMSG nonexistent :This should fail
PRIVMSG #nonexistent :This should also fail
```

**Pass Criteria**: 
- Direct messages work between users
- Channel messages work
- Proper error responses for invalid targets
- Messages have correct format

---

## PART 5: CHANNEL OPERATOR COMMANDS

### Test 5.1: Operator Privilege Setup
```bash
# Client 1 (will be operator):
nc 127.0.0.1 6667
PASS password123
NICK operator
USER operator 0 * :Channel Operator
JOIN #opchannel

# Client 2 (regular user):
nc 127.0.0.1 6667
PASS password123
NICK regular
USER regular 0 * :Regular User
JOIN #opchannel
```

### Test 5.2: KICK Command
```bash
# As regular user (should fail):
KICK #opchannel operator :You're out

# As operator (should work):
KICK #opchannel regular :Goodbye

# Verify regular user was removed from channel
```

**Pass Criteria**: 
- Regular users cannot kick
- Operators can kick users
- Kicked user receives proper notification
- User removed from channel

### Test 5.3: INVITE Command
```bash
# Setup: operator in #private, regular user outside

# Client 3 (to be invited):
nc 127.0.0.1 6667
PASS password123
NICK invited
USER invited 0 * :Invited User

# As regular user (should fail):
INVITE invited #private

# As operator (should work):
INVITE invited #private

# invited user should be able to join:
JOIN #private
```

**Pass Criteria**: 
- Regular users cannot invite
- Operators can invite users
- Invited user can join channel
- Proper invite notifications

### Test 5.4: TOPIC Command
```bash
# As regular user (should fail if +t mode):
TOPIC #opchannel :New topic by regular user

# As operator (should work):
TOPIC #opchannel :New topic by operator

# View current topic:
TOPIC #opchannel
```

**Pass Criteria**: 
- Topic restrictions work based on channel mode
- Operators can always set topic
- Topic changes are broadcast to channel

### Test 5.5: MODE Commands

#### Test 5.5.1: Invite-only mode (+i)
```bash
# As operator:
MODE #opchannel +i

# Test: regular user tries to join new channel
# Should fail without invite

# Remove mode:
MODE #opchannel -i
```

#### Test 5.5.2: Topic restriction (+t)
```bash
# As operator:
MODE #opchannel +t

# Test: regular user tries to change topic
# Should fail

# Remove mode:
MODE #opchannel -t
```

#### Test 5.5.3: Channel key (+k)
```bash
# As operator:
MODE #opchannel +k secretkey

# Test: user tries to join without key
JOIN #opchannel
# Should fail

# Test: user joins with key
JOIN #opchannel secretkey
# Should work

# Remove key:
MODE #opchannel -k
```

#### Test 5.5.4: Operator privilege (+o)
```bash
# As operator:
MODE #opchannel +o regular

# Test: regular user now has operator privileges
# Should be able to kick, invite, etc.

# Remove operator:
MODE #opchannel -o regular
```

#### Test 5.5.5: User limit (+l)
```bash
# As operator:
MODE #opchannel +l 3

# Test: try to have more than 3 users join
# 4th user should be rejected

# Remove limit:
MODE #opchannel -l
```

**Pass Criteria for all MODE tests**: 
- Only operators can change modes
- Mode changes work as expected
- Proper error messages for unauthorized attempts
- Mode effects are enforced correctly

---

## PART 6: ADDITIONAL REQUIRED COMMANDS

### Test 6.1: WHO Command
```bash
WHO #opchannel
WHO operator
```

### Test 6.2: LIST Command
```bash
LIST
LIST #opchannel
```

### Test 6.3: QUIT Command
```bash
QUIT :Goodbye everyone
```

**Pass Criteria**: 
- Commands return proper responses
- Information is accurate and formatted correctly

---

## EVALUATION SCORING

### Mandatory Part Checklist

**Basic Checks (Pass/Fail - Must be Perfect)**
- [ ] Makefile works correctly
- [ ] Only one poll() in code
- [ ] poll() called before accept/read/write
- [ ] fcntl() only used with O_NONBLOCK
- [ ] No errno usage after system calls

**Networking (0-5 points)**
- [ ] Server starts and listens correctly
- [ ] nc connection works
- [ ] IRC client connection works  
- [ ] Multiple simultaneous connections
- [ ] Channel message forwarding

**Networking Specials (0-5 points)**
- [ ] Partial command handling
- [ ] Unexpected client kill handling
- [ ] Half-command kill handling
- [ ] Client suspension handling

**Client Commands Basic (0-5 points)**
- [ ] Authentication works
- [ ] PRIVMSG fully functional

**Channel Operator Commands (0-5 points)**
- [ ] Regular users cannot use operator commands
- [ ] KICK command works
- [ ] INVITE command works
- [ ] TOPIC command works
- [ ] MODE +i works
- [ ] MODE +t works
- [ ] MODE +k works
- [ ] MODE +o works
- [ ] MODE +l works

### Final Verification

Run through this entire guide systematically. Each test must pass for the project to meet evaluation standards. Document any failures and fix them before submission.

**Total possible score**: 20/25 points (Basic checks must be perfect, others rated 0-5)

---

## Quick Test Script

For rapid testing, you can use the existing test files in your project:
- `test_server.py`
- `test_multiple_clients.py`
- `test_invite_command.py`
- `test_kick_command.py`
- `test_topic_command.py`

These complement the manual tests above and can help verify functionality programmatically.
