#!/usr/bin/env python3
import socket
import time
import threading

def client1():
    """First client: joins channel and sends messages"""
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
    
    # Wait for user2 to join
    time.sleep(2)
    
    # Send a message
    sock.send(b"PRIVMSG #test :Hello from user1!\r\n")
    time.sleep(0.1)
    
    # Wait for message to be processed
    time.sleep(2)
    
    # Close
    sock.close()
    print("Client1 finished")

def client2():
    """Second client: joins channel and receives messages"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 6667))
    
    # Register
    sock.send(b"PASS testpass\r\n")
    time.sleep(0.1)
    sock.send(b"NICK user2\r\n")
    time.sleep(0.1)
    sock.send(b"USER user2 0 * :User Two\r\n")
    time.sleep(0.1)
    
    # Join channel
    sock.send(b"JOIN #test\r\n")
    time.sleep(0.1)
    
    # Wait for messages and try to receive
    time.sleep(3)
    
    # Try to receive data multiple times
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
    print("Starting multiple client test...")
    
    # Start clients in separate threads
    t1 = threading.Thread(target=client1)
    t2 = threading.Thread(target=client2)
    
    t1.start()
    time.sleep(0.5)  # Start client2 slightly after client1
    t2.start()
    
    # Wait for both to finish
    t1.join()
    t2.join()
    
    print("Multiple client test completed!")

if __name__ == "__main__":
    main()
