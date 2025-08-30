#ifndef FT_IRC_HPP
 define FT_IRC_HPP

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


# define SERVER_PREFIX ":ircserv.42.de"
# define SERVER_NAME "ircserv"
# define SERVER_CREATION "1/08/2025"
# define SERVER_VERSION "0.01"

# define ADMIN_INFO_1 "Wolfsburg, Germany"
# define ADMIN_INFO_2 "The best 42 Germany IRC server !"
# define ADMIN_EMAIL "dsamuel@student.42wolfsburg.de & demrodri@student.42wolfsburg.de"

// Checks if a character exists in a string
bool confirmPresence(char c, std::string str);

// Checks if a string is a valid IRC channel name
bool confirmChannel(std::string name);

// Converts a string to lowercase
std::string stringToLowercase(std::string string);

// Returns true if elem is found in the list
template <typename T>
bool is_in_list(std::list<T> list, T elem)
{
    for (typename std::list<T>::iterator it = list.begin(); it != list.end(); ++it)
    {
        if (*it == elem)
            return true;
    }
    return false;
}

// Deletes all values in a map and clears it
template <typename T, typename U>
void delete_map(std::map<T, U> &map)
{
    for (typename std::map<T, U>::iterator it = map.begin(); it != map.end(); ++it)
    {
        delete it->second;
    }
    map.clear();
}

// Checks if value exists in a static array
template<typename T>
bool is_in_array(T value, T array[], unsigned int size_array)
{
    for (unsigned int i = 0; i < size_array; ++i)
    {
        if (value == array[i])
            return true;
    }
    return false;
}

// Returns the index of value in array, or -1 if not found
template<typename T>
unsigned int get_array_index(T value, T array[], unsigned int size_array)
{
    for (unsigned int i = 0; i < size_array; ++i)
    {
        if (value == array[i])
            return i;
    }
    return static_cast<unsigned int>(-1);
}

#endif 
