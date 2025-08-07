#ifndef MESSAGE_HANDLER_H
# define MESSAGE_HANDLER_H

# include "ft_irc.hpp"
# include "CommandMessage.hpp"
# include "reply.hpp"
# include "ModeHandler.hpp"

class ConnectionManager;

class MessageHandler
{
	private:
		typedef void (MessageHandler::*handler)(CommandMessage &message);
		typedef std::pair<std::string, handler> handlerPair;

		ConnectionManager &context;
		std::map<std::string, handler> handle;

		void configureMessageHandlers();

		void adminCommandHandler(CommandMessage &message);
		void capRequestHandler(CommandMessage &message);
		void infoCommandHandler(CommandMessage &message);
		void joinCommandHandler(CommandMessage &message);
		void kickCommandHandler(CommandMessage &message);
		void listCommandHandler(CommandMessage &message);
		void modeCommandHandler( CommandMessage &message);
		void namesCommandHandler(CommandMessage &message);
		void nickChangeHandler(CommandMessage &message);
		// void operatorCommandHandler(CommandMessage &message);
		void partCommandHandler(CommandMessage &message);
		void passCommandHandler(CommandMessage &message);
		void privateMessageHandler(CommandMessage &message);
		void quitCommandHandler(CommandMessage &message);
		void summonCommandHandler(CommandMessage &message);
		void userCommandHandler(CommandMessage &message);
		void usersCommandHandler(CommandMessage &message);
		void versionRequestHandler(CommandMessage &message);
		// void whoCommandHandler(CommandMessage &message);
		void inviteCommandHandler(CommandMessage &message);
		void topicCommandHandler(CommandMessage &message);
		void pingCommandHandler(CommandMessage &message);
		void pongCommandHandler(CommandMessage &message);

		bool checkMessageEligibility(User &sender, CommandMessage &message);
		CommandMessage *buildCommandMessage(User &sender, std::string rawMessage);
		void checkMessageValidity(User &sender, CommandMessage &message);
		void greetNewUser(User &user);
		bool checkIsValidUsername(std::string username);

	public:
		MessageHandler(ConnectionManager &context);
		virtual ~MessageHandler();

		void processClientCommand(User &sender, const std::string &rawMessage);
};

#endif
