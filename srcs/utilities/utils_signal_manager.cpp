#include "ft_irc.hpp"
#include "utils_signal_manager.hpp"
#include <iostream>

bool SignalManager::shouldShutdown = false;

void SignalManager::signalHandler(int signalNumber)
{
	/* std::cout << "[Sig Handler] Caught signal " << signalNumber << std::endl; */
	if (signalNumber == SIGINT)
	{
		SignalManager::shouldShutdown = true;
	}
}


bool confirmPresence(char c, std::string str)
{
	if (str.find(c) != std::string::npos)
	{
		return (true);
	}
	return (false);
}

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

std::string string_to_lowercase(std::string string)
{
	std::string lowercase = string;
	std::string::iterator it = lowercase.begin();
	for (; it != lowercase.end(); it++)
	{
		*it = std::tolower(*it);
	}
	return (lowercase);
}
