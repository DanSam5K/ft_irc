#include "ft_irc.hpp"
#include "utils_signal_manager.hpp"
#include <iostream>

// Static variable to indicate if the server should shut down
bool SignalManager::shouldShutdown = false;

// Handles incoming signals (e.g., SIGINT) and sets shutdown flag
void SignalManager::signalHandler(int signalNumber)
{
	/* std::cout << "[Sig Handler] Caught signal " << signalNumber << std::endl; */
	if (signalNumber == SIGINT)
	{
		SignalManager::shouldShutdown = true;
	}
}

// Checks if a character is present in a given string
bool confirmPresence(char c, std::string str)
{
	if (str.find(c) != std::string::npos)
	{
		return (true);
	}
	return (false);
}

// Verifies if a string is a valid channel name (starts with # or &)
bool confirmChannel(std::string name)
{
	const std::string channel_types = "#&";
	if (name.size() <= 0)
	{
		return (false);
	}
	if (confirmPresence(name[0], channel_types))
	{
		return (true);
	}
	return (false);
}

// Converts a string to lowercase
std::string stringToLowercase(std::string string)
{
	std::string lowercase = string;
	std::string::iterator it = lowercase.begin();
	for (; it != lowercase.end(); it++)
	{
		*it = std::tolower(*it);
	}
	return (lowercase);
}
