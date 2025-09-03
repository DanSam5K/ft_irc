#!/bin/bash
echo "Testing IRC connection with explicit line endings..."
(
    echo -e "PASS password123\r"
    sleep 1
    echo -e "NICK testuser\r" 
    sleep 1
    echo -e "USER testuser 0 * :Test User\r"
    sleep 1
    echo -e "QUIT\r"
) | nc 127.0.0.1 6667
echo "Connection test complete"
