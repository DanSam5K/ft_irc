#!/bin/bash
echo "Testing QUIT command..."
echo -e "QUIT\r" | nc 127.0.0.1 6667
echo "Test complete"
