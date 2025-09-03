#!/bin/bash
echo "Testing connection without QUIT (should stay connected)..."
echo -e "PASS password123\r" | nc -w 3 127.0.0.1 6667 &
echo "PID: $!"
sleep 5
echo "Test complete - check server logs"
