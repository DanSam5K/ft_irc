#ifndef UTILS_SIGNAL_MANAGER_HPP
#define UTILS_SIGNAL_MANAGER_HPP

#include <csignal>

// ANSI escape sequences for styling terminal output
// Reset formatting
#define ANSI_RESET              "\e[0m"

// Text styles
#define ANSI_BOLD               "\e[1m"
#define ANSI_DIM                "\e[2m"
#define ANSI_ITALIC             "\e[3m"
#define ANSI_UNDERLINE          "\e[4m"

// Standard foreground colors
#define ANSI_FG_BLACK           "\e[30m"
#define ANSI_FG_RED             "\e[31m"
#define ANSI_FG_GREEN           "\e[32m"
#define ANSI_FG_YELLOW          "\e[33m"
#define ANSI_FG_BLUE            "\e[34m"
#define ANSI_FG_MAGENTA         "\e[35m"
#define ANSI_FG_CYAN            "\e[36m"
#define ANSI_FG_WHITE           "\e[37m"

// Bright foreground colors
#define ANSI_FG_BRIGHT_BLACK    "\e[90m"
#define ANSI_FG_BRIGHT_RED      "\e[91m"
#define ANSI_FG_BRIGHT_GREEN    "\e[92m"
#define ANSI_FG_BRIGHT_YELLOW   "\e[93m"
#define ANSI_FG_BRIGHT_BLUE     "\e[94m"
#define ANSI_FG_BRIGHT_MAGENTA  "\e[95m"
#define ANSI_FG_BRIGHT_CYAN     "\e[96m"
#define ANSI_FG_BRIGHT_WHITE    "\e[97m"

// Standard background colors
#define ANSI_BG_BLACK           "\e[40m"
#define ANSI_BG_RED             "\e[41m"
#define ANSI_BG_GREEN           "\e[42m"
#define ANSI_BG_YELLOW          "\e[43m"
#define ANSI_BG_BLUE            "\e[44m"
#define ANSI_BG_MAGENTA         "\e[45m"
#define ANSI_BG_CYAN            "\e[46m"
#define ANSI_BG_WHITE           "\e[47m"

// Bright background colors
#define ANSI_BG_BRIGHT_BLACK    "\e[100m"
#define ANSI_BG_BRIGHT_RED      "\e[101m"
#define ANSI_BG_BRIGHT_GREEN    "\e[102m"
#define ANSI_BG_BRIGHT_YELLOW   "\e[103m"
#define ANSI_BG_BRIGHT_BLUE     "\e[104m"
#define ANSI_BG_BRIGHT_MAGENTA  "\e[105m"
#define ANSI_BG_BRIGHT_CYAN     "\e[106m"
#define ANSI_BG_BRIGHT_WHITE    "\e[107m"

namespace SignalManager
{
    extern bool shouldShutdown;
    
    // Handles system signals (e.g., SIGINT) to trigger server shutdown.
    void signalHandler(int signalNumber);
}

#endif
