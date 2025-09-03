#!/bin/bash
echo "Testing IRC connection with unique nickname..."
UNIQUE_NICK="user$(date +%s)"
(
    echo -e "PASS password123\r"
    sleep 1
    echo -e "NICK $UNIQUE_NICK\r" 
    sleep 1
    echo -e "USER $UNIQUE_NICK 0 * :Test User\r"
    sleep 2
    echo -e "QUIT\r"
) | nc 127.0.0.1 6667
echo "Connection test complete"
