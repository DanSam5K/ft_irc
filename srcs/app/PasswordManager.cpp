#include "PasswordManager.hpp"
#include "ft_irc.hpp"
#include "utils_logger.hpp"

PasswordManager::PasswordManager(const std::string &connectionPassword) {
	if (connectionPassword.empty())
	{
		log_action_utils::info("PasswordManager: No password provided, using default password");
		hashedConnectionPassword = DEFAULT_CONN_PASSWORD_HASH;
		return ;
	}
	hashedConnectionPassword = computeSHA256(connectionPassword);
}

PasswordManager::~PasswordManager() {}

std::string PasswordManager::computeSHA256(const std::string &password)
{
	unsigned char hash[SHA256_DIGEST_LENGTH];
	unsigned const char *plainText = reinterpret_cast<unsigned const char *>(password.c_str());
	SHA256(plainText, password.size(), hash);

	std::stringstream ss;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
	{
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
	}
	log_action_utils::info("PasswordManager: Created hash", ss.str());
	return (ss.str());
}

std::string PasswordManager::generateHash(const std::string &plainText)
{
	return (computeSHA256(plainText));
}

void PasswordManager::verifyConnectionPassword(const std::string &password)
{
	verifyPassword(hashedConnectionPassword, password);
}

void PasswordManager::verifyPassword(const std::string &hash, const std::string &plainText)
{
	std::string password_hash = computeSHA256(plainText);

	if (password_hash != hash)
	{
		log_action_utils::info("PasswordManager: Incorrect password...");
		throw InvalidPasswordException();
	}
	log_action_utils::info("PasswordManager: Passwords match!");
}

const char* PasswordManager::InvalidPasswordException::what() const throw()
{
	return ("invalid password");
}
