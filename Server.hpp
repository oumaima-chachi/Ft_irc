#pragma once

#include "musthave.hpp"
#include "Client.hpp"
#include "codes.hpp"
#include "Channel.hpp"

class Server {

	public:
		Server(int port, const std::string &password);
		~Server();
		void	run();

	private:
		int					Port_;
		std::string			Pass_;
		int					Server_fd;
		struct sockaddr_in	Server_addr_;
		
		std::vector<pollfd> _pollDescriptors;
		std::map<int, Client*>	_connectedClients;
		LoungeManager		_channelManager;
		bool				_serverActive;
		
		struct Challenge 
		{
			std::string	target;
			std::string	channel;
		};
		
		void	socketcreating();
		void	Addrsetuping();
		void	bindsocket();
		void	startlistening();

		void	handlingnewconnection();
		void	receiveclientdata(int fd);
		void	handleClientDisconnect(int fd);
		void	disconnectClient(int clientFd);
		
		void	executecommand(Client* client, const std::string& command);
		void	authenticatingclient(Client* client);
		void	welcomingmsg(Client* client) const;
		bool	NickValid(const std::string& nickname);
		void	dispatchCommand(Client* cl, const std::string& cmd, const std::vector<std::string>& args);

		bool						validatePassword(const std::string& input) const;
		Client*						findClientByNickname(const std::string& nickname);
		std::vector<std::string>	split(const std::string& str, char delimiter);
		void						sendReplay(int fd, const std::string& response) const;

		Client*	searchForUser(std::string nickname);

		void						handleModeK(Client* client, Channel* channel, const std::string& channelName, char sign, const std::vector<std::string>& params);
		void						handleModeO(Client* client, Channel* channel, const std::string& channelName, char sign, const std::vector<std::string>& params);
		void						handleModeL(Client* client, Channel* channel, const std::string& channelName, char sign, const std::vector<std::string>& params);
		void						verifyPassword(Client* client, const std::vector<std::string>& params);
		void						registerUser(Client* client, const std::vector<std::string>& params);
		bool						validateUser(Client* client, const std::vector<std::string>& params);
		std::vector<std::string>	parsePasswords(const std::vector<std::string>& params);
		void						joinChannel(Client* client, const std::string& channelName, const std::string& password);
		void						manageMode(Client* client, const std::vector<std::string>& params);
		void						handleModeI(Client* client, Channel* channel, const std::string& channelName, char sign, const std::vector<std::string>& params);
		void						handleModeT(Client* client, Channel* channel, const std::string& channelName, char sign, const std::vector<std::string>& params);
		void						kickClient(Client* client, const std::vector<std::string>& params);
		void						processKick(Client* client, Channel* channel, const std::string& channelName, const std::string& nickname, const std::string& reason);
		void						inviteClient(Client* client, const std::vector<std::string>& params);
		bool						validateInvite(Client* client, const std::vector<std::string>& params);
		void						processQuitChannels(Client* client, const std::string& quitMessage);
		void						sendMessage(Client* client, const std::vector<std::string>& params);
		void						processPrivmsg(Client* client, const std::string& target, const std::string& msgToSend);
		void						joinChannel(Client* client, const std::vector<std::string>& params);
		void						manageTopic(Client* client, const std::vector<std::string>& params);
		void						processTopic(Client* client, Channel* channel, const std::string& channelName, const std::vector<std::string>& params);
		bool						checkPassErrors(Client *client, const std::vector<std::string> &params);
		void						changeNickname(Client* client, const std::vector<std::string>& params);
		bool						validateNick(Client* client, const std::vector<std::string>& params);
		void						disconnectClient(Client* client, const std::vector<std::string>& params);

        void notice(int fd, const std::string &nick, const std::string &msg);
        std::string getBotNick() const;
        void handleBotMessage(Client *client, const std::string &msg);
        void handleDccCommand(Client *client, const std::vector<std::string> &args);

};