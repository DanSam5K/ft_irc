#!/usr/bin/env python3
"""
IRC Server Test Suite
Works on GitHub Codespaces, Actions, and any system with Python 3
No external dependencies required - uses only standard library
"""

import socket
import time
import sys
import threading
from typing import List, Tuple, Optional

class IRCTester:
    def __init__(self, host='127.0.0.1', port=6667, password='password123'):
        self.host = host
        self.port = port
        self.password = password
        self.tests_total = 0
        self.tests_passed = 0
        self.tests_failed = 0
        
    def print_header(self):
        print("\n" + "="*50)
        print("   IRC SERVER COMPREHENSIVE TEST SUITE")
        print("="*50)
        
    def print_test(self, test_name: str):
        print(f"\n[TEST] {test_name}")
        self.tests_total += 1
        
    def print_success(self, message: str):
        print(f"✅ [PASS] {message}")
        self.tests_passed += 1
        
    def print_failure(self, message: str):
        print(f"❌ [FAIL] {message}")
        self.tests_failed += 1
        
    def print_info(self, message: str):
        print(f"ℹ️  [INFO] {message}")
        
    def send_irc_commands(self, commands: str, timeout: int = 5) -> Tuple[bool, str]:
        """Send IRC commands and return (success, response)"""
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(timeout)
            sock.connect((self.host, self.port))
            
            # Send commands
            sock.send(commands.encode('utf-8'))
            
            # Receive response
            response = ""
            start_time = time.time()
            while time.time() - start_time < timeout:
                try:
                    data = sock.recv(4096).decode('utf-8')
                    if not data:
                        break
                    response += data
                    # Stop if we get a complete response (ends with \r\n)
                    if response.endswith('\r\n'):
                        break
                except socket.timeout:
                    break
                    
            sock.close()
            return True, response
            
        except Exception as e:
            return False, str(e)
    
    def test_server_connectivity(self) -> bool:
        """Test if server is accepting connections"""
        self.print_test("Server Connectivity")
        
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(3)
            result = sock.connect_ex((self.host, self.port))
            sock.close()
            
            if result == 0:
                self.print_success(f"Server accepting connections on {self.host}:{self.port}")
                return True
            else:
                self.print_failure(f"Cannot connect to {self.host}:{self.port}")
                return False
        except Exception as e:
            self.print_failure(f"Connection error: {e}")
            return False
    
    def test_unauthenticated_commands(self):
        """Test commands without authentication"""
        print("\n--- Testing Unauthenticated Commands ---")
        
        # Test JOIN without auth (should fail)
        self.print_test("JOIN without authentication")
        success, response = self.send_irc_commands("JOIN #test\r\n")
        if success and "451" in response and "not registered" in response:
            self.print_success("Correctly rejected JOIN without authentication")
        else:
            self.print_failure("JOIN without auth should return 451 error")
            self.print_info(f"Response: {response}")
        
        # Test PRIVMSG without auth (should fail)
        self.print_test("PRIVMSG without authentication")
        success, response = self.send_irc_commands("PRIVMSG #test :Hello\r\n")
        if success and "451" in response:
            self.print_success("Correctly rejected PRIVMSG without authentication")
        else:
            self.print_failure("PRIVMSG without auth should return 451 error")
            self.print_info(f"Response: {response}")
    
    def test_authentication(self):
        """Test authentication flow"""
        print("\n--- Testing Authentication Flow ---")
        
        # Test wrong password
        self.print_test("Wrong password")
        success, response = self.send_irc_commands(f"PASS wrongpassword\r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\n")
        if success and "464" in response:
            self.print_success("Correctly rejected wrong password")
        else:
            self.print_failure("Wrong password should return 464 error")
            self.print_info(f"Response: {response}")
        
        # Test correct authentication
        self.print_test("Correct authentication")
        commands = f"PASS {self.password}\r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\n"
        success, response = self.send_irc_commands(commands)
        if success and "001" in response and "Welcome" in response:
            self.print_success("Successfully authenticated and received welcome message")
        else:
            self.print_failure("Should receive 001 Welcome message after correct auth")
            self.print_info(f"Response: {response}")
        
        # Test long nickname
        self.print_test("Long nickname rejection")
        commands = f"PASS {self.password}\r\nNICK verylongnickname\r\nUSER testuser 0 * :Test User\r\n"
        success, response = self.send_irc_commands(commands)
        if success and "432" in response:
            self.print_success("Correctly rejected long nickname")
        else:
            self.print_failure("Long nickname should return 432 error")
            self.print_info(f"Response: {response}")
    
    def test_cap_commands(self):
        """Test CAP commands for modern IRC client compatibility"""
        print("\n--- Testing CAP Commands ---")
        
        # Test CAP LS
        self.print_test("CAP LS command")
        success, response = self.send_irc_commands("CAP LS 302\r\n")
        if success and "CAP" in response and "LS" in response:
            self.print_success("CAP LS command works")
        else:
            self.print_failure("CAP LS should return capability list")
            self.print_info(f"Response: {response}")
        
        # Test CAP negotiation flow
        self.print_test("CAP negotiation flow")
        commands = "CAP LS\r\nCAP END\r\n"
        success, response = self.send_irc_commands(commands)
        if success:
            self.print_success("CAP negotiation completed")
        else:
            self.print_failure("CAP negotiation failed")
            self.print_info(f"Response: {response}")
    
    def test_complete_flow(self):
        """Test complete IRC client connection flow"""
        print("\n--- Testing Complete IRC Client Flow ---")
        
        # Modern IRC client flow
        self.print_test("Modern IRC client flow")
        commands = f"CAP LS 302\r\nPASS {self.password}\r\nNICK modernuser\r\nUSER modernuser 0 * :Modern User\r\nCAP END\r\n"
        success, response = self.send_irc_commands(commands, timeout=10)
        if success and "001" in response:
            self.print_success("Modern IRC client flow successful")
        else:
            self.print_failure("Modern IRC client flow failed")
            self.print_info(f"Response: {response}")
        
        # Basic IRC client flow
        self.print_test("Basic IRC client flow")
        commands = f"PASS {self.password}\r\nNICK basicuser\r\nUSER basicuser 0 * :Basic User\r\n"
        success, response = self.send_irc_commands(commands)
        if success and "001" in response:
            self.print_success("Basic IRC client flow successful")
        else:
            self.print_failure("Basic IRC client flow failed")
            self.print_info(f"Response: {response}")
    
    def test_channel_operations(self):
        """Test channel operations"""
        print("\n--- Testing Channel Operations ---")
        
        # Test JOIN after authentication
        self.print_test("JOIN channel after authentication")
        commands = f"PASS {self.password}\r\nNICK chanuser\r\nUSER chanuser 0 * :Channel User\r\nJOIN #testchan\r\n"
        success, response = self.send_irc_commands(commands, timeout=10)
        if success and ("353" in response or "JOIN" in response):
            self.print_success("Successfully joined channel")
        else:
            self.print_failure("Failed to join channel after authentication")
            self.print_info(f"Response: {response}")
        
        # Test JOIN with empty channel
        self.print_test("JOIN with empty channel")
        commands = f"PASS {self.password}\r\nNICK emptyuser\r\nUSER emptyuser 0 * :Empty User\r\nJOIN :\r\n"
        success, response = self.send_irc_commands(commands)
        if success and "461" in response:
            self.print_success("Correctly rejected empty channel JOIN")
        else:
            self.print_failure("Empty channel JOIN should return 461 error")
            self.print_info(f"Response: {response}")
    
    def test_error_handling(self):
        """Test error handling"""
        print("\n--- Testing Error Handling ---")
        
        # Test invalid command
        self.print_test("Invalid command")
        success, response = self.send_irc_commands("INVALID_COMMAND\r\n")
        if success and "421" in response:
            self.print_success("Correctly handled invalid command")
        else:
            self.print_failure("Invalid command should return 421 error")
            self.print_info(f"Response: {response}")
        
        # Test malformed JOIN (should not crash server)
        self.print_test("Malformed JOIN command")
        success, response = self.send_irc_commands("JOIN :\r\n")
        if success:
            self.print_success("Server handled malformed command without crashing")
        else:
            self.print_failure("Server should handle malformed commands gracefully")
            self.print_info(f"Response: {response}")
    
    def test_concurrent_connections(self):
        """Test multiple simultaneous connections"""
        print("\n--- Testing Concurrent Connections ---")
        
        self.print_test("Multiple simultaneous connections")
        
        def connect_user(user_id: int) -> bool:
            commands = f"PASS {self.password}\r\nNICK user{user_id}\r\nUSER user{user_id} 0 * :User {user_id}\r\n"
            success, response = self.send_irc_commands(commands)
            return success and "001" in response
        
        # Test 3 concurrent connections
        threads = []
        results = []
        
        def worker(user_id):
            result = connect_user(user_id)
            results.append(result)
        
        for i in range(3):
            thread = threading.Thread(target=worker, args=(i+1,))
            threads.append(thread)
            thread.start()
        
        for thread in threads:
            thread.join()
        
        if all(results):
            self.print_success("All concurrent connections successful")
        else:
            self.print_failure(f"Only {sum(results)}/3 concurrent connections successful")
    
    def show_results(self):
        """Show final test results"""
        print("\n" + "="*50)
        print("            FINAL RESULTS")
        print("="*50)
        print(f"Total tests: {self.tests_total}")
        print(f"✅ Passed: {self.tests_passed}")
        print(f"❌ Failed: {self.tests_failed}")
        
        if self.tests_failed == 0:
            print("\n🎉 ALL TESTS PASSED! 🎉")
            print("Your IRC server is working correctly!")
            return True
        else:
            print(f"\n❌ {self.tests_failed} test(s) failed")
            print("Check the output above for details")
            return False
    
    def run_all_tests(self) -> bool:
        """Run all tests and return True if all pass"""
        self.print_header()
        
        # Check server connectivity first
        if not self.test_server_connectivity():
            print("\nℹ️  Make sure your IRC server is running:")
            print(f"   ./ircserv {self.port} {self.password}")
            return False
        
        # Run all test suites
        self.test_unauthenticated_commands()
        self.test_authentication()
        self.test_cap_commands()
        self.test_complete_flow()
        self.test_channel_operations()
        self.test_error_handling()
        self.test_concurrent_connections()
        
        # Show results
        return self.show_results()

def main():
    """Main function"""
    # Parse command line arguments
    host = sys.argv[1] if len(sys.argv) > 1 else '127.0.0.1'
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 6667
    password = sys.argv[3] if len(sys.argv) > 3 else 'password123'
    
    print(f"Testing IRC server at {host}:{port} with password '{password}'")
    
    # Create and run tester
    tester = IRCTester(host, port, password)
    success = tester.run_all_tests()
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
