#!/bin/bash

# Comprehensive IRC Server Test Script
# Works on GitHub Codespaces, Actions, and any Unix environment
# Uses only basic tools: echo, printf, sleep, timeout, bash

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test configuration
SERVER_HOST="127.0.0.1"
SERVER_PORT="6667"
SERVER_PASSWORD="password123"
TEST_TIMEOUT=5

# Counters
TESTS_TOTAL=0
TESTS_PASSED=0
TESTS_FAILED=0

print_header() {
    echo -e "\n${BLUE}================================${NC}"
    echo -e "${BLUE}  IRC SERVER COMPREHENSIVE TEST${NC}"
    echo -e "${BLUE}================================${NC}\n"
}

print_test() {
    local test_name="$1"
    echo -e "${YELLOW}[TEST]${NC} $test_name"
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
}

print_success() {
    local message="$1"
    echo -e "${GREEN}[PASS]${NC} $message"
    TESTS_PASSED=$((TESTS_PASSED + 1))
}

print_failure() {
    local message="$1"
    echo -e "${RED}[FAIL]${NC} $message"
    TESTS_FAILED=$((TESTS_FAILED + 1))
}

print_info() {
    local message="$1"
    echo -e "${BLUE}[INFO]${NC} $message"
}

# Function to send commands to server and capture response
send_irc_command() {
    local commands="$1"
    local expected_pattern="$2"
    local test_name="$3"
    
    print_test "$test_name"
    
    # Create temporary files for communication
    local temp_input=$(mktemp)
    local temp_output=$(mktemp)
    
    # Write commands to temp file
    echo -e "$commands" > "$temp_input"
    
    # Send commands and capture output with timeout
    if timeout $TEST_TIMEOUT bash -c "cat '$temp_input' | nc '$SERVER_HOST' '$SERVER_PORT'" > "$temp_output" 2>&1; then
        local output=$(cat "$temp_output")
        
        if [[ -n "$expected_pattern" ]] && echo "$output" | grep -q "$expected_pattern"; then
            print_success "$test_name - Found expected pattern: $expected_pattern"
        elif [[ -z "$expected_pattern" ]]; then
            print_success "$test_name - Command executed successfully"
        else
            print_failure "$test_name - Expected pattern '$expected_pattern' not found"
            echo "  Actual output: $output"
        fi
        
        # Print server response for debugging
        if [[ -n "$output" ]]; then
            echo "  Server response: $output"
        fi
    else
        print_failure "$test_name - Connection failed or timed out"
    fi
    
    # Cleanup
    rm -f "$temp_input" "$temp_output"
    
    # Small delay between tests
    sleep 1
}

# Function to check if server is running
check_server() {
    print_test "Server Connectivity"
    
    if timeout 3 bash -c "</dev/tcp/$SERVER_HOST/$SERVER_PORT" 2>/dev/null; then
        print_success "Server is accepting connections on $SERVER_HOST:$SERVER_PORT"
        return 0
    else
        print_failure "Cannot connect to server on $SERVER_HOST:$SERVER_PORT"
        return 1
    fi
}

# Function to test server without authentication
test_unauthenticated_commands() {
    echo -e "\n${BLUE}--- Testing Unauthenticated Commands ---${NC}"
    
    # Test JOIN without authentication (should fail)
    send_irc_command "JOIN #test\r\n" "451.*not registered" "JOIN without registration"
    
    # Test PRIVMSG without authentication (should fail)
    send_irc_command "PRIVMSG #test :Hello\r\n" "451.*not registered" "PRIVMSG without registration"
    
    # Test NICK without password (should work)
    send_irc_command "NICK testuser\r\n" "" "NICK without password"
}

# Function to test authentication flow
test_authentication() {
    echo -e "\n${BLUE}--- Testing Authentication Flow ---${NC}"
    
    # Test wrong password
    send_irc_command "PASS wrongpassword\r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\n" "464.*Password incorrect" "Wrong password"
    
    # Test correct password with short nickname
    send_irc_command "PASS $SERVER_PASSWORD\r\nNICK shortnic\r\nUSER testuser 0 * :Test User\r\n" "001.*Welcome" "Correct authentication with short nickname"
    
    # Test correct password with long nickname (should fail)
    send_irc_command "PASS $SERVER_PASSWORD\r\nNICK verylongnickname\r\nUSER testuser 0 * :Test User\r\n" "432.*too long" "Long nickname rejection"
}

# Function to test CAP commands (modern IRC client compatibility)
test_cap_commands() {
    echo -e "\n${BLUE}--- Testing CAP Commands ---${NC}"
    
    # Test CAP LS
    send_irc_command "CAP LS 302\r\n" "CAP.*LS" "CAP LS command"
    
    # Test CAP END
    send_irc_command "CAP LS\r\nCAP END\r\n" "" "CAP negotiation"
}

# Function to test complete IRC client flow
test_complete_flow() {
    echo -e "\n${BLUE}--- Testing Complete IRC Client Flow ---${NC}"
    
    # Modern IRC client flow with CAP
    local modern_flow="CAP LS 302\r\nPASS $SERVER_PASSWORD\r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\nCAP END\r\n"
    send_irc_command "$modern_flow" "001.*Welcome" "Modern IRC client flow"
    
    # Basic IRC client flow
    local basic_flow="PASS $SERVER_PASSWORD\r\nNICK basicuser\r\nUSER basicuser 0 * :Basic User\r\n"
    send_irc_command "$basic_flow" "001.*Welcome" "Basic IRC client flow"
}

# Function to test channel operations
test_channel_operations() {
    echo -e "\n${BLUE}--- Testing Channel Operations ---${NC}"
    
    # Test JOIN with authenticated user
    local auth_join="PASS $SERVER_PASSWORD\r\nNICK chanuser\r\nUSER chanuser 0 * :Channel User\r\nJOIN #testchan\r\n"
    send_irc_command "$auth_join" "353.*chanuser" "JOIN channel after authentication"
    
    # Test JOIN without channel name (should fail)
    local auth_join_empty="PASS $SERVER_PASSWORD\r\nNICK emptyuser\r\nUSER emptyuser 0 * :Empty User\r\nJOIN :\r\n"
    send_irc_command "$auth_join_empty" "461.*Not enough parameters" "JOIN with empty channel"
}

# Function to test error handling
test_error_handling() {
    echo -e "\n${BLUE}--- Testing Error Handling ---${NC}"
    
    # Test invalid command
    send_irc_command "INVALID_COMMAND\r\n" "421.*Unknown command" "Invalid command"
    
    # Test command with no parameters
    send_irc_command "NICK\r\n" "431.*No nickname given" "NICK with no parameters"
    
    # Test malformed commands (this should not crash the server)
    send_irc_command "JOIN :\r\n" "" "Malformed JOIN command"
}

# Function to test concurrent connections
test_concurrent_connections() {
    echo -e "\n${BLUE}--- Testing Concurrent Connections ---${NC}"
    
    print_test "Multiple simultaneous connections"
    
    # Start multiple connections in background
    local pids=()
    for i in {1..3}; do
        (
            echo -e "PASS $SERVER_PASSWORD\r\nNICK user$i\r\nUSER user$i 0 * :User $i\r\n" | \
            timeout 3 nc "$SERVER_HOST" "$SERVER_PORT" > /dev/null 2>&1
        ) &
        pids+=($!)
    done
    
    # Wait for all background processes
    local all_success=true
    for pid in "${pids[@]}"; do
        if ! wait "$pid"; then
            all_success=false
        fi
    done
    
    if $all_success; then
        print_success "Multiple concurrent connections handled"
    else
        print_failure "Some concurrent connections failed"
    fi
}

# Function to show final results
show_results() {
    echo -e "\n${BLUE}================================${NC}"
    echo -e "${BLUE}         TEST RESULTS${NC}"
    echo -e "${BLUE}================================${NC}"
    echo -e "Total tests: $TESTS_TOTAL"
    echo -e "${GREEN}Passed: $TESTS_PASSED${NC}"
    echo -e "${RED}Failed: $TESTS_FAILED${NC}"
    
    if [ $TESTS_FAILED -eq 0 ]; then
        echo -e "\n${GREEN}🎉 ALL TESTS PASSED! 🎉${NC}"
        echo -e "${GREEN}Your IRC server is working correctly!${NC}"
        exit 0
    else
        echo -e "\n${RED}❌ Some tests failed${NC}"
        echo -e "${YELLOW}Check the output above for details${NC}"
        exit 1
    fi
}

# Main execution
main() {
    print_header
    
    # Check if nc (netcat) is available
    if ! command -v nc &> /dev/null; then
        print_failure "netcat (nc) is not available. Cannot run tests."
        echo "Please install netcat or use a different testing method."
        exit 1
    fi
    
    # Check if server is running
    if ! check_server; then
        echo -e "\n${YELLOW}Make sure your IRC server is running:${NC}"
        echo "  ./ircserv $SERVER_PORT $SERVER_PASSWORD"
        exit 1
    fi
    
    # Run all tests
    test_unauthenticated_commands
    test_authentication
    test_cap_commands
    test_complete_flow
    test_channel_operations
    test_error_handling
    test_concurrent_connections
    
    # Show final results
    show_results
}

# Run main function
main "$@"
