#pragma once

#include "musthave.hpp"
#include "Client.hpp"

class Client;

class Channel
{
	private:
		std::string             channel_name;
		std::string             channel_key;
		std::vector<Client*>    joined_users;
		std::vector<Client*>    channel_operators;
		std::vector<char>       active_modes;
		std::string             channel_topic;
		int                     isInviteONly;
		int                     user_limit;
		bool                    is_topic_restricted;

	public:
		std::vector<Client*>    _invitedClients;

		Channel(std::string new_name);
		std::string             getRoomName();
		std::string             getRoomSecret();
		void                    set_pass_ok(std::string password);
		std::string             getRoomTitle() const;
		void                    setRoomTitle(const std::string &newTopic);
		int                     getMaxBuddies();
		void                    setMaxBuddies(int limit);

		void                    addBadge(char mode);
		std::vector<char>       getBadges();
		void                    removeBadge(char mode);
		
		void                    setKnockOnly(int flag);
		int                     getKnockOnly();
		bool                    hasPass(std::string nickName);
		void                    welcomeGuest(Client* invitedClient);

		void                    lockTitle(bool state);
		bool                    isTitleLocked();

		void                    cuddleIn(Client* client);
		void                    waveGoodbye(Client* client);
		void                    removeUuser(std::string nickname, std::string msg);
		bool                    Is_Member(Client* client);
		bool                    checkForClient(std::string client_name);
		Client*                 findBuddy(std::string nickname);
		Client*                 firstMember();
		bool                    isLonely();
		size_t                  countBuddies();
		std::vector<std::string> getBuddyNames();

		void                    promoteToCaptain(Client* client);
		void                    addCaptain(std::string newOp);
		void                    demoteCaptain(std::string newOp);
		bool                    isCaptain(Client* client);
		void                    appointFirstCaptain();
		bool                    hasNoCaptains();
		bool                    isCaptainListEmpty();
		size_t                  countCaptains();

		void                    echoToAll(const std::string &msg, std::string senderNick);
		void                    echoSystemMessage(const std::string &msg);
};

class LoungeManager
{
	public:
		std::vector<Channel*>   allLounges;

		~LoungeManager();
		Channel*                openLounge(std::string channel_name);
		Channel*                findLounge(std::string channel_name);
		void                    closeLounge(const std::string& channelName);
};

