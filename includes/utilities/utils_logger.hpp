/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  						         utils_logger.hpp 	 					     #
#****************************************************************************/

#ifndef UTILS_LOGGER_HPP
# define UTILS_LOGGER_HPP

// Event logging levels (it allows filtering of log messages)
#define EVENT_LOGGER_OFF 0 // No logging
#define EVENT_LOGGER_ERROR 1 // Error messages
#define EVENT_LOGGER_WARNING 2 // Warning messages
#define EVENT_LOGGER_INFO 3 // Informational messages

#define EVENT_LOGGER_LEVEL 3 // Current logging level

#include "utils_signal_manager.hpp"
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>

namespace logActionUtils // Namespace for logging actions
{
	void info(std::string msg); // Log informational messages
	void warn(std::string msg); // Log warning messages
	void warn(std::string msg, std::string command); // Log warning messages with command
	void error(std::string msg); // Log error messages
	void reply(int socket, std::string msg); // Log reply messages
	void command(int socket, std::string command); // Log command messages

	std::string getFormattedCommand(std::string command); // Get formatted command string

	template <typename T> // info: Log informational messages with detail
	void info(std::string msg, T detail)
	{
		if (EVENT_LOGGER_LEVEL < EVENT_LOGGER_INFO)
		{
			return;
		}
		std::stringstream ss;

		ss << ANSI_RESET "[STATUS] " << msg << " " << detail << ANSI_RESET;
		std::cerr << ss.str() << std::endl;
	}

	template <typename T> // warn: Log warning messages with detail
	void warn(std::string msg, T detail)
	{
		if (EVENT_LOGGER_LEVEL < EVENT_LOGGER_WARNING)
		{
			return;
		}
		std::stringstream ss;

		ss << ANSI_FG_YELLOW "[WARN] " << msg << " " << detail << ANSI_RESET;
		std::cerr << ss.str() << std::endl;
	}
}
#endif
