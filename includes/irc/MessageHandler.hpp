#ifndef MESSAGE_HPPANDLER_HPP
# define MESSAGE_HPPANDLER_HPP

# include "ft_irc.hpp"
# include "CommandMessage.hpp"
# include "reply_message.hpp"
# include "ModeHandler.hpp"

class ConnectionManager;

// Handles parsing and execution of IRC commands received from clients
class MessageHandler
{
    private:
        // Type alias for member function pointers used as command handlers
        typedef void (MessageHandler::*handler)(CommandMessage &message);
        typedef std::pair<std::string, handler> handlerPair;

        ConnectionManager &context;           // Reference to the server's connection manager
        std::map<std::string, handler> handle;// Maps command names to their handler functions

        // Registers all supported IRC command handlers
        void configureMessageHandlers();

        // Individual command handler functions for supported IRC commands
        void adminCommandHandler(CommandMessage &message);
        void capRequestHandler(CommandMessage &message);
        void infoCommandHandler(CommandMessage &message);
        void joinCommandHandler(CommandMessage &message);
        void kickCommandHandler(CommandMessage &message);
        void listCommandHandler(CommandMessage &message);
        void modeCommandHandler(CommandMessage &message);
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

        // Checks if a message from a user is allowed to be processed
        bool checkMessageEligibility(ClientUser &sender, CommandMessage &message);

        // Builds a CommandMessage object from raw input
        CommandMessage *buildCommandMessage(ClientUser &sender, std::string rawMessage);

        // Validates the structure and content of a command message
        void checkMessageValidity(ClientUser &sender, CommandMessage &message);

        // Sends welcome messages and initial info to a newly registered user
        void greetNewUser(ClientUser &user);

    public:
        // Constructs a MessageHandler for the given server context
        MessageHandler(ConnectionManager &context);
        virtual ~MessageHandler();

        // Processes a raw IRC command received from a client
        void processClientCommand(ClientUser &sender, const std::string &rawMessage);
};

#endif



// class MessageHandler
// {
// 	private:
// 		typedef void (MessageHandler::*handler)(CommandMessage &message);
// 		typedef std::pair<std::string, handler> handlerPair;

// 		ConnectionManager &context;
// 		std::map<std::string, handler> handle;

// 		void configureMessageHandlers();

// 		void adminCommandHandler(CommandMessage &message);
// 		void capRequestHandler(CommandMessage &message);
// 		void infoCommandHandler(CommandMessage &message);
// 		void joinCommandHandler(CommandMessage &message);
// 		void kickCommandHandler(CommandMessage &message);
// 		void listCommandHandler(CommandMessage &message);
// 		void modeCommandHandler( CommandMessage &message);
// 		void namesCommandHandler(CommandMessage &message);
// 		void nickChangeHandler(CommandMessage &message);
// 		// void operatorCommandHandler(CommandMessage &message);
// 		void partCommandHandler(CommandMessage &message);
// 		void passCommandHandler(CommandMessage &message);
// 		void privateMessageHandler(CommandMessage &message);
// 		void quitCommandHandler(CommandMessage &message);
// 		void summonCommandHandler(CommandMessage &message);
// 		void userCommandHandler(CommandMessage &message);
// 		void usersCommandHandler(CommandMessage &message);
// 		void versionRequestHandler(CommandMessage &message);
// 		// void whoCommandHandler(CommandMessage &message);
// 		void inviteCommandHandler(CommandMessage &message);
// 		void topicCommandHandler(CommandMessage &message);
// 		void pingCommandHandler(CommandMessage &message);
// 		void pongCommandHandler(CommandMessage &message);

// 		bool checkMessageEligibility(ClientUser &sender, CommandMessage &message);
// 		CommandMessage *buildCommandMessage(ClientUser &sender, std::string rawMessage);
// 		void checkMessageValidity(ClientUser &sender, CommandMessage &message);
// 		void greetNewUser(ClientUser &user);
// 		// bool checkIsValidUsername(std::string username);

// 	public:
// 		MessageHandler(ConnectionManager &context);
// 		virtual ~MessageHandler();

// 		void processClientCommand(ClientUser &sender, const std::string &rawMessage);
// };