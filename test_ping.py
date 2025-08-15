#!/usr/bin/env python3
import socket
import time

def test_ping_command():
    # Connect to the server
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 6667))
    
    # Basic registration
    sock.send(b"PASS testpass\r\n")
    time.sleep(0.1)
    sock.send(b"NICK testuser\r\n")
    time.sleep(0.1)
    sock.send(b"USER testuser 0 * :Test User\r\n")
    time.sleep(0.1)
    
    # Test PING with different formats
    print("Testing PING :test")
    sock.send(b"PING :test\r\n")
    time.sleep(0.1)
    
    print("Testing PING test")
    sock.send(b"PING test\r\n")
    time.sleep(0.1)
    
    # Close connection
    sock.close()
    print("PING test completed!")

if __name__ == "__main__":
    test_ping_command()
