#!/bin/bash
echo "Testing IRC sequence with proper terminators..."
{
    printf "PASS password123\r\n"
    sleep 1
    printf "NICK alice\r\n"
    sleep 1
    printf "USER alice 0 * :Alice User\r\n"
    sleep 1
    printf "JOIN #general\r\n"
    sleep 1
    printf "PRIVMSG #general :Hello everyone!\r\n"
    sleep 1
    printf "QUIT\r\n"
} | nc 127.0.0.1 6667
echo "Test complete"
