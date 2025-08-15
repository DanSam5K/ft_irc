#ifndef UTILS_LOGGER_HPP
#define UTILS_LOGGER_HPP

#define EVENT_LOGGER_OFF 0
#define EVENT_LOGGER_ERROR 1
#define EVENT_LOGGER_WARNING 2
#define EVENT_LOGGER_INFO 3

#define EVENT_LOGGER_LEVEL 3

#include "utils_signal_manager.hpp"
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>

namespace logActionUtils
{
	void info(std::string msg);
	void warn(std::string msg);
	void warn(std::string msg, std::string command);
	void error(std::string msg);
	void reply(int socket, std::string msg);
	void command(int socket, std::string command);
	std::string get_formatted_command(std::string command);

	template <typename T>
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

	template <typename T>
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
