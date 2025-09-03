/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  					     	  utils_signal_manager.hpp 	 	    		     #
#****************************************************************************/

#ifndef UTILS_SIGNAL_MANAGER_HPP
# define UTILS_SIGNAL_MANAGER_HPP

#include <csignal> // For signal handling, used in server shutdown

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
    extern bool shouldShutdown; // Indicates if the server should shut down

    // Handles system signals (e.g., SIGINT) to trigger server shutdown.
    void signalHandler(int signalNumber);
}

#endif
