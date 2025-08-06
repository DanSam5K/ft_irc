#ifndef PASSWORD_MANAGER_HPP
#define PASSWORD_MANAGER_HPP

#include "ft_irc.hpp"

// Default SHA-256 hash for the connection password "password"
#define DEFAULT_CONN_PASSWORD_HASH "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8"

class PasswordManager {
private:
	std::string hashedConnectionPassword;

	// Internal: Generate SHA-256 hash from plain text
	std::string computeSHA256(const std::string &password);

public:
	// Constructor that stores hashed version of connection password
	PasswordManager(const std::string &connectionPassword);
	virtual ~PasswordManager();

	// Return SHA-256 hash of provided plain text
	std::string generateHash(const std::string &plainText);

	// Validate incoming connection password against stored one
	void verifyConnectionPassword(const std::string &password);

	// Generic password check: compare hash with plain password
	void verifyPassword(const std::string &storedHash, const std::string &password);

	// Exception thrown on invalid password attempts
	class InvalidPasswordException : public std::exception {
	public:
		virtual const char* what() const throw();
	};
};

#endif



// #ifndef PASSWORD_H
// #define PASSWORD_H

// #include "ft_irc.hpp"

// /* The default connection password is 'password', the sha256 hash is: */
// #define DEFAULT_CONNECTION_PASSWORD "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8"

// class Password
// {
// 	private:
// 		std::string connection_password_hash;
// 		std::string create_sha256_hash(std::string plain_text);

// 	public:
// 		Password(std::string connection_password);
// 		virtual ~Password();

// 		std::string get_hash(std::string plain_text);
// 		void validate_connection_password(std::string password);
// 		void validate_password(std::string hash, std::string password);

// 		class InvalidPasswordException : public std::exception
// 		{
// 			public:
// 				virtual const char* what() const throw();
// 		};
// };

// #endif
