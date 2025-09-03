/****************************************************************************#
#  - - - - >  42 WOLFSBURG  < - - - - - - - - - - - > ft_ircserv  < - - - -  #
#  - - - - >  By: dsamuel & demrodri < - - - - - - - >  08/2025   < - - - -  #
#****************************************************************************#
#  						     utils_logger.cpp 	    					     #
#****************************************************************************/

#include "utils_logger.hpp"

// Logs informational messages if the logger level allows it
void logActionUtils::info(std::string msg)
{
	if (EVENT_LOGGER_LEVEL < EVENT_LOGGER_INFO)
	{
		return;
	}
	std::stringstream ss;

	ss << ANSI_RESET "[STATUS] " << msg << ANSI_RESET;
	std::cerr << ss.str() << std::endl;
}

// Logs warning messages if the logger level allows it
void logActionUtils::warn(std::string msg)
{
	if (EVENT_LOGGER_LEVEL < EVENT_LOGGER_WARNING)
	{
		return ;
	}
	std::stringstream ss;

	ss << ANSI_FG_YELLOW "[WARN] " << msg << ANSI_RESET;
	std::cerr << ss.str() << std::endl;
}

// Logs warning messages with an associated command string
void logActionUtils::warn(std::string msg, std::string command)
{
	if (EVENT_LOGGER_LEVEL < EVENT_LOGGER_WARNING)
	{
		return ;
	}
	std::string formatted_command = getFormattedCommand(command);

	std::stringstream ss;
	ss << ANSI_FG_YELLOW "[WARN] " << msg << ": [" << formatted_command << "]" << ANSI_RESET;
	std::cerr << ss.str() << std::endl;
}

// Logs error messages if the logger level allows it
void logActionUtils::error(std::string msg)
{
	if (EVENT_LOGGER_LEVEL < EVENT_LOGGER_ERROR)
	{
		return ;
	}
	std::stringstream ss;

	ss << ANSI_FG_RED "[ERROR] " << msg << ANSI_RESET;
	std::cerr << ss.str() << std::endl;
}

// Logs a reply message sent to a specific socket
void logActionUtils::reply(int socket, std::string reply)
{
	std::stringstream ss;
	std::string formatted_reply = getFormattedCommand(reply);


	ss << ANSI_FG_CYAN "[REPLY][Socket " << socket << "]: [" << formatted_reply << "]" << ANSI_RESET;
	std::cerr << ss.str() << std::endl;
}

// Logs a command message received from a specific socket
void logActionUtils::command(int socket, std::string command)
{
	std::string formatted_command = getFormattedCommand(command);
	std::stringstream ss;
	ss << ANSI_FG_MAGENTA "[COMMAND][Socket " << socket << "]: [" << formatted_command << "]" << ANSI_RESET;
	std::cerr << ss.str() << std::endl;
}

// Formats a command string by escaping special characters for logging
std::string logActionUtils::getFormattedCommand(std::string command)
{
	std::stringstream ss;

	for(size_t i = 0; i < command.length(); i++)
	{
		switch (command[i])
		{
			case '\n':
				ss << "\\n";
				break ;
			case '\r':
				ss << "\\r";
				break ;
			case '\b':
				ss << "\\b";
				break ;
			case '\t':
				ss << "\\t";
				break ;
			case '\a':
				ss << "\\a";
				break ;
			case '\v':
				ss << "\\v";
				break ;
			case '\0':
				ss << "\\0";
				break ;
			default:
				ss << command[i];
		}
	}
	return (ss.str());
}
