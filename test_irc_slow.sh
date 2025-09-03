#!/bin/bash
echo "Testing IRC connection with delays..."
{
    echo "PASS password123"
    sleep 3
    echo "NICK testuser" 
    sleep 3
    echo "USER testuser 0 * :Test User"
    sleep 3
    echo "QUIT"
} | nc 127.0.0.1 6667
echo "Connection test complete"
