#ifndef PASSWORD_MANAGER_HPP
#define PASSWORD_MANAGER_HPP

#include "ft_irc.hpp"

// Default SHA-256 hash for the connection password "password"
#define DEFAULT_CONN_PASSWORD_HPPASH "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8"

class PasswordManager {
private:
	std::string hashedConnectionPassword;

	// Internal: Generate SHA-256 hash from plain text
	std::string computeSHA256(std::string passwordText);

public:
	// Constructor that stores hashed version of connection password
	PasswordManager(std::string connectionPassword);
	virtual ~PasswordManager();

	// Return SHA-256 hash of provided plain text
	std::string generateHash(std::string plainText);

	// Validate incoming connection password against stored one
	void verifyConnectionPassword(std::string password);

	// Generic password check: compare hash with plain password
	void verifyPassword(std::string storedHash, std::string password);

	// Exception thrown on invalid password attempts
	class InvalidPasswordException : public std::exception {
	public:
		virtual const char* what() const throw();
	};
};

#endif



// #ifndef PASSWORD_HPP
// #define PASSWORD_HPP

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
