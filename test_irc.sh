#!/bin/bash
echo "Testing IRC connection..."
{
    echo "PASS password123"
    sleep 1
    echo "NICK testuser"
    sleep 1 
    echo "USER testuser 0 * :Test User"
    sleep 2
    echo "QUIT"
} | nc 127.0.0.1 6667
echo "Connection test complete"
