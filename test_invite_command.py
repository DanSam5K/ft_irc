#!/usr/bin/env python3
import socket
import time
import threading

def client1():
    """First client: channel operator who will invite user2"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 6667))
    
    # Register
    sock.send(b"PASS testpass\r\n")
    time.sleep(0.1)
    sock.send(b"NICK user1\r\n")
    time.sleep(0.1)
    sock.send(b"USER user1 0 * :User One\r\n")
    time.sleep(0.1)
    
    # Join channel (will become operator since channel is new)
    sock.send(b"JOIN #test\r\n")
    time.sleep(0.1)
    
    # Wait for user2 to register
    time.sleep(2)
    
    # Invite user2 to the channel
    sock.send(b"INVITE user2 #test\r\n")
    time.sleep(0.1)
    
    # Wait for invite to be processed
    time.sleep(2)
    
    # Close
    sock.close()
    print("Client1 (operator) finished")

def client2():
    """Second client: will be invited to channel"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 6667))
    
    # Register
    sock.send(b"PASS testpass\r\n")
    time.sleep(0.1)
    sock.send(b"NICK user2\r\n")
    time.sleep(0.1)
    sock.send(b"USER user2 0 * :User Two\r\n")
    time.sleep(0.1)
    
    # Wait for invite
    time.sleep(3)
    
    # Try to receive data
    received_data = b""
    for i in range(5):
        try:
            sock.settimeout(0.5)
            data = sock.recv(1024)
            if data:
                received_data += data
                print(f"Client2 received chunk {i+1}: {data}")
        except socket.timeout:
            print(f"Client2 timeout on attempt {i+1}")
        except Exception as e:
            print(f"Client2 error on attempt {i+1}: {e}")
    
    if received_data:
        print(f"Client2 total received: {received_data}")
    else:
        print("Client2 received no data")
    
    # Close
    sock.close()
    print("Client2 finished")

def main():
    print("Starting INVITE command test...")
    
    # Start clients in separate threads
    t1 = threading.Thread(target=client1)
    t2 = threading.Thread(target=client2)
    
    t1.start()
    time.sleep(0.5)  # Start client2 slightly after client1
    t2.start()
    
    # Wait for both to finish
    t1.join()
    t2.join()
    
    print("INVITE command test completed!")

if __name__ == "__main__":
    main()
