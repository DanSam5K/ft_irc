#include "PasswordManager.hpp"
#include "ft_irc.hpp"
#include "utils_logger.hpp"

PasswordManager::PasswordManager(std::string connectionPassword) {
	if (connectionPassword.empty())
	{
		logActionUtils::info("PasswordManager: No password provided, using default password");
		hashedConnectionPassword = DEFAULT_CONN_PASSWORD_HPPASH;
		return ;
	}
	hashedConnectionPassword = computeSHA256(connectionPassword);
}

PasswordManager::~PasswordManager() {}

std::string PasswordManager::computeSHA256(std::string password)
{
	unsigned char hash[SHA256_DIGEST_LENGTH];
	unsigned const char *plainText = reinterpret_cast<unsigned const char *>(password.c_str());
	SHA256(plainText, password.size(), hash);

	std::stringstream ss;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
	{
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
	}
	logActionUtils::info("PasswordManager: Created hash", ss.str());
	return (ss.str());
}

std::string PasswordManager::generateHash(std::string plainText)
{
	return (computeSHA256(plainText));
}

void PasswordManager::verifyConnectionPassword(std::string password)
{
	verifyPassword(hashedConnectionPassword, password);
}

void PasswordManager::verifyPassword(std::string hash, std::string plainText)
{
	std::string password_hash = computeSHA256(plainText);

	if (password_hash != hash)
	{
		logActionUtils::info("PasswordManager: Incorrect password...");
		throw InvalidPasswordException();
	}
	logActionUtils::info("PasswordManager: Passwords match!");
}

const char* PasswordManager::InvalidPasswordException::what() const throw()
{
	return ("invalid password");
}
