#ifndef FT_IRC_HPP
#define FT_IRC_HPP

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <exception>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <netinet/in.h>
#include <netdb.h>
#include <new>
#include <openssl/sha.h>
#include <poll.h>
#include <signal.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>
#include <openssl/sha.h>
#include "utils_logger.hpp"
#include "utils_signal_manager.hpp"

// # define ANSI_RESET	"\e[0m"

// # define BOLD	"\e[1m"
// # define DIM	"\e[2m"
// # define ITAL	"\e[3m"
// # define ULINE	"\e[4m"

// # define BLACK	"\e[30m"
// # define ANSI_FG_RED	"\e[31m"
// # define ANSI_FG_GREEN	"\e[32m"
// # define ANSI_FG_YELLOW	"\e[33m"
// # define BLUE	"\e[34m"
// # define ANSI_FG_MAGENTA	"\e[35m"
// # define ANSI_FG_CYAN	"\e[36m"
// # define WHITE	"\e[37m"

// # define BRIGHT_BLACK	"\e[90m"
// # define BRIGHT_RED		"\e[91m"
// # define BRIGHT_GREEN	"\e[92m"
// # define BRIGHT_YELLOW	"\e[93m"
// # define BRIGHT_BLUE	"\e[94m"
// # define BRIGHT_PURPLE	"\e[95m"
// # define BRIGHT_CYAN	"\e[96m"
// # define BRIGHT_WHITE	"\e[97m"

// # define BG_BLACK	"\e[40m"
// # define BG_RED		"\e[41m"
// # define BG_GREEN	"\e[42m"
// # define BG_YELLOW	"\e[43m"
// # define BG_BLUE	"\e[44m"
// # define BG_PURPLE	"\e[45m"
// # define BG_CYAN	"\e[46m"
// # define BG_WHITE	"\e[47m"

// # define BG_BRIGHT_BLACK	"\e[100m"
// # define BG_BRIGHT_RED		"\e[101m"
// # define BG_BRIGHT_GREEN	"\e[102m"
// # define BG_BRIGHT_YELLOW	"\e[103m"
// # define BG_BRIGHT_BLUE		"\e[104m"
// # define BG_BRIGHT_PURPLE	"\e[105m"
// # define BG_BRIGHT_CYAN		"\e[106m"
// # define BG_BRIGHT_WHITE	"\e[107m"

# define SERVER_PREFIX ":ircserv.42.de"
# define SERVER_NAME "ircserv"
# define SERVER_CREATION "1/08/2025"
# define SERVER_VERSION "0.01"

# define ADMIN_INFO_1 "Wolfsburg, Germany"
# define ADMIN_INFO_2 "The best 42 Germany IRC server !"
# define ADMIN_EMAIL "dsamuel@student.42wolfsburg.de "

bool confirmPresence(char c, std::string str);

template <typename T>
bool is_in_list(std::list<T> list, T elem)
{
	typename std::list<T>::iterator it = list.begin();
	for (; it != list.end(); it++)
	{
		if (*it == elem)
		{
			return (true);
		}
	}
	return (false);
}

bool confirmChannel(std::string name);

std::string stringToLowercase(std::string string);

template <typename T, typename U>
void delete_map(std::map<T, U> &map)
{
	typename std::map<T, U>::iterator it = map.begin();
	for (; it != map.end(); it++)
	{
		delete (it->second);
	}
	map.clear();
}

template<typename T>
bool is_in_array(T value, T array[], unsigned int size_array)
{
	for (unsigned int i = 0; i < size_array; i++)
	{
		if (value == array[i])
		{
			return (true);
		}
	}
	return (false);
}

template<typename T>
unsigned int get_array_index(T value, T array[], unsigned int size_array)
{
	for (unsigned int i = 0; i < size_array; i++)
	{
		if (value == array[i])
		{
			return (i);
		}
	}
	return (-1);
}

// void examineString(std::string s);
// void examineChar(char c);
// void examineCharStar(char *cs);

#endif /* FT_IRC_HPP */
