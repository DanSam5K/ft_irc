#ifndef UTILS_SIGNAL_MANAGER_HPP
#define UTILS_SIGNAL_MANAGER_HPP

#include <csignal>

// ANSI escape sequences for styling terminal output
// Reset formatting
#define ANSI_RESET              "\e[0m"

// Standard foreground colors
#define ANSI_FG_RED             "\e[31m"
#define ANSI_FG_YELLOW          "\e[33m"
#define ANSI_FG_MAGENTA         "\e[35m"
#define ANSI_FG_CYAN            "\e[36m"

namespace SignalManager
{
    extern bool shouldShutdown; // this is a extern variable, that means that it is defined in another file.
                                // The reason why it is not defined here is to allow for shared access across multiple files.
                                // This allows different parts of the program to check if a shutdown has been requested.
    // Handles system signals (e.g., SIGINT) to trigger server shutdown.
    void signalHandler(int signalNumber);
}

#endif
