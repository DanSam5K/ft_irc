#!/bin/bash
echo "Testing JOIN command that causes crash..."
printf "JOIN :\r\n" | nc 127.0.0.1 6667
echo "Test complete"
