#!/bin/bash

# IRC Server Test Script
# Tests various IRC commands and scenarios

SERVER="127.0.0.1"
PORT="6667"
PASSWORD="password123"

echo "🧪 IRC Server Test Suite"
echo "========================"

# Test 1: Basic Connection and Authentication
echo -e "\n📝 Test 1: Basic Authentication"
echo "Sending: PASS, NICK, USER commands..."
{
    echo "PASS $PASSWORD"
    echo "NICK testuser"
    echo "USER testuser 0 * :Test User"
    echo "QUIT :Goodbye"
} | nc $SERVER $PORT
echo "✅ Test 1 completed"

# Test 2: Long Nickname Test
echo -e "\n📝 Test 2: Long Nickname (should be rejected)"
{
    echo "PASS $PASSWORD"
    echo "NICK verylongnicknamethatshouldberejected"
    echo "USER test 0 * :Test"
    echo "QUIT"
} | nc $SERVER $PORT
echo "✅ Test 2 completed"

# Test 3: CAP Command Test
echo -e "\n📝 Test 3: CAP Commands (modern IRC client)"
{
    echo "CAP LS 302"
    echo "PASS $PASSWORD"
    echo "NICK modern"
    echo "USER modern 0 * :Modern Client"
    echo "CAP END"
    echo "QUIT"
} | nc $SERVER $PORT
echo "✅ Test 3 completed"

# Test 4: JOIN Channel Test
echo -e "\n📝 Test 4: JOIN Channel Commands"
{
    echo "PASS $PASSWORD"
    echo "NICK joiner"
    echo "USER joiner 0 * :Channel Joiner"
    echo "JOIN #test"
    echo "PRIVMSG #test :Hello everyone!"
    echo "PART #test :Leaving now"
    echo "QUIT"
} | nc $SERVER $PORT
echo "✅ Test 4 completed"

# Test 5: Invalid Commands Test
echo -e "\n📝 Test 5: Invalid Commands"
{
    echo "PASS $PASSWORD"
    echo "NICK tester"
    echo "USER tester 0 * :Tester"
    echo "INVALIDCOMMAND"
    echo "ANOTHERBADCOMMAND arg1 arg2"
    echo "QUIT"
} | nc $SERVER $PORT
echo "✅ Test 5 completed"

# Test 6: Edge Cases
echo -e "\n📝 Test 6: Edge Cases"
{
    echo "PASS $PASSWORD"
    echo "NICK edge"
    echo "USER edge 0 * :Edge Case Tester"
    echo "JOIN :"
    echo "JOIN"
    echo "PRIVMSG"
    echo "QUIT"
} | nc $SERVER $PORT
echo "✅ Test 6 completed"

echo -e "\n🎉 All tests completed!"
echo "Check the server logs for detailed responses."
