#!/usr/bin/env python3
import socket
import time

def test_irc_server():
    # Connect to the server
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 6667))
    
    # Test PASS command
    print("Testing PASS command...")
    sock.send(b"PASS testpass\r\n")
    time.sleep(0.1)
    
    # Test NICK command
    print("Testing NICK command...")
    sock.send(b"NICK testuser\r\n")
    time.sleep(0.1)
    
    # Test USER command
    print("Testing USER command...")
    sock.send(b"USER testuser 0 * :Test User\r\n")
    time.sleep(0.1)
    
    # Test JOIN command
    print("Testing JOIN command...")
    sock.send(b"JOIN #test\r\n")
    time.sleep(0.1)
    
    # Test PRIVMSG command
    print("Testing PRIVMSG command...")
    sock.send(b"PRIVMSG #test :Hello everyone!\r\n")
    time.sleep(0.1)
    
    # Test another PRIVMSG
    print("Testing another PRIVMSG...")
    sock.send(b"PRIVMSG #test :This is a test message\r\n")
    time.sleep(0.1)
    
    # Test PING command
    print("Testing PING command...")
    sock.send(b"PING :test\r\n")
    time.sleep(0.1)
    
    # Test LIST command
    print("Testing LIST command...")
    sock.send(b"LIST\r\n")
    time.sleep(0.1)
    
    # Test NAMES command
    print("Testing NAMES command...")
    sock.send(b"NAMES #test\r\n")
    time.sleep(0.1)
    
    # Test PART command
    print("Testing PART command...")
    sock.send(b"PART #test :Leaving channel\r\n")
    time.sleep(0.1)
    
    # Test QUIT command
    print("Testing QUIT command...")
    sock.send(b"QUIT :Goodbye!\r\n")
    time.sleep(0.1)
    
    # Close connection
    sock.close()
    print("Test completed!")

if __name__ == "__main__":
    test_irc_server()
