#!/usr/bin/env python3
import socket
import time

def test_invite_parsing():
    """Test INVITE command parsing step by step"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 6667))
    
    # Register
    sock.send(b"PASS testpass\r\n")
    time.sleep(0.1)
    sock.send(b"NICK user1\r\n")
    time.sleep(0.1)
    sock.send(b"USER user1 0 * :User One\r\n")
    time.sleep(0.1)
    
    # Join channel
    sock.send(b"JOIN #test\r\n")
    time.sleep(0.1)
    
    # Test INVITE with different formats
    print("Testing INVITE user2 #test")
    sock.send(b"INVITE user2 #test\r\n")
    time.sleep(0.1)
    
    print("Testing INVITE user2:#test")
    sock.send(b"INVITE user2:#test\r\n")
    time.sleep(0.1)
    
    # Close connection
    sock.close()
    print("INVITE parsing test completed!")

if __name__ == "__main__":
    test_invite_parsing()
