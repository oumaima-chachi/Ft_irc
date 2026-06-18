#include "../Channel.hpp"
#include "../musthave.hpp"
#include "../Server.hpp"
#include "../Client.hpp"


Channel::Channel(std::string new_name) : channel_name(new_name), channel_key(""), channel_topic("No topic is set"), isInviteONly(0), user_limit(0) {}

LoungeManager::~LoungeManager()
{
	for (size_t i = 0; i < allLounges.size(); ++i)
    {
        delete allLounges[i];
    }
    allLounges.clear();
}

bool Channel::hasNoCaptains()
{
	if (channel_operators.size() < 2)
		return true;
	else return false;
}

void Channel::appointFirstCaptain()
{
	std::string name;
	if (channel_operators.empty() && !joined_users.empty())
    {
		channel_operators.push_back(joined_users[0]);
		name = "@" + channel_operators[0]->get_nickname();
		channel_operators[0]->set_nickname(name);
	}
}

void Channel::addCaptain(std::string newOp)
{
	std::string name;
	Client* client = findBuddy(newOp);
	name = client->get_nickname();
	client->set_nickname(name);
	channel_operators.push_back(client);
}


Client* Channel::findBuddy(std::string nickname)
{
	for (size_t i = 0; i < joined_users.size(); ++i)
    {
        if (nickname == joined_users[i]->get_nickname())
        {
            return joined_users[i];
        }
    }
    return NULL;
}

void Channel::setKnockOnly(int flag)
{
	isInviteONly = flag;
}

int Channel::getKnockOnly()
{
    return isInviteONly;
}

void Channel::setMaxBuddies(int limit)
{
	this->user_limit = limit;
}

int Channel::getMaxBuddies()
{
    return user_limit;
}

bool Channel::hasPass(std::string nickName)
{
	for(size_t i = 0; i < _invitedClients.size(); i++)
	{
		if (_invitedClients[i]->get_nickname() == nickName)
			return true;
	}
	return false;
}

void	Channel::welcomeGuest(Client* invitedClient)
{
	for (size_t i = 0; i < _invitedClients.size(); i++)
	{
		if (_invitedClients[i] == invitedClient)
			return ;
	}
	_invitedClients.push_back(invitedClient);
}

void Channel::waveGoodbye(Client* client)
{
    bool wasOp = isCaptain(client);

    joined_users.erase(std::remove(joined_users.begin(), joined_users.end(), client), joined_users.end());
   channel_operators.erase(std::remove(channel_operators.begin(), channel_operators.end(), client), channel_operators.end());

    if (wasOp && channel_operators.empty() && !joined_users.empty()) {
        Client* newOp = joined_users.front();
        promoteToCaptain(newOp);

        std::string modeMsg = ":FT_irc MODE " + getRoomName() + " +o " + newOp->get_nickname() + "\r\n";
        echoToAll(modeMsg, "");
    }
}
size_t Channel::countBuddies() 
{
    return joined_users.size();
}

size_t Channel::countCaptains() 
{
    return channel_operators.size();
}

void Channel::setRoomTitle(const std::string &newTopic)
{
       channel_topic = newTopic;
}


std::string Channel::getRoomTitle() const
{
    return channel_topic;
}

std::string Channel::getRoomName()
{
    return channel_name;
}

std::string Channel::getRoomSecret()
{
    return channel_key;
}

void Channel::set_pass_ok(std::string pass)
{
    channel_key = pass;
}

void Channel::addBadge(char mode)
{
    for (std::vector<char>::iterator it = active_modes.begin(); it != active_modes.end(); it++) {
        if (*it == mode)
            return;
    }
	active_modes.push_back(mode);
}

std::vector<char> Channel::getBadges()
{
	return active_modes;
}

void Channel::removeBadge(char mode)
{
	for (std::vector<char>::iterator it = active_modes.begin(); it != active_modes.end(); ++it) {
    	if ((*it) == mode) {
            active_modes.erase(it);
            return;
		}
    }
}

void Channel::cuddleIn(Client* client) 
{
    if (std::find(joined_users.begin(), joined_users.end(), client) == joined_users.end()) {
        joined_users.push_back(client);
    }
}

void Channel::promoteToCaptain(Client* client) 
{
    if (!isCaptain(client)) {
        channel_operators.push_back(client);
    }
}

bool Channel::isCaptain(Client* client) 
{
    return std::find(channel_operators.begin(), channel_operators.end(), client) != channel_operators.end();
}

bool Channel::Is_Member(Client* client) 
{
    return std::find(joined_users.begin(), joined_users.end(), client) != joined_users.end();
}

bool Channel::checkForClient(std::string client_name)
{
    for (size_t i = 0; i <joined_users.size(); ++i) 
    {
        if (client_name == joined_users[i]->get_nickname())
            return true;
    }
    return false;
}

bool Channel::isLonely()
{
	if (!joined_users.size())
		return true;
	return false;
}

void Channel::lockTitle(bool state) 
{
    is_topic_restricted = state;
}
bool Channel::isTitleLocked() 
{
    return is_topic_restricted;
}

void Channel::removeUuser(std::string nickname, std::string msg)
{
    for (std::vector<Client*>::iterator it = joined_users.begin(); it != joined_users.end(); ++it) {
        if ((*it)->get_nickname() == nickname)
		{
			send((*it)->get_ffd(), msg.c_str(), msg.size(), 0);
            joined_users.erase(it);
            break;
        }
    }
    for (std::vector<Client*>::iterator it = channel_operators.begin(); it != channel_operators.end(); ++it) {
        if ((*it)->get_nickname() == nickname)
		{
			// send((*it)->get_ffd(), msg.c_str(), msg.size(), 0);
            channel_operators.erase(it);
            break;
        }
    }
}

bool Channel::isCaptainListEmpty() {
    if (channel_operators.empty())
        return true;
    return false;
}

Client * Channel::firstMember() {
    return joined_users.front();
}

void Channel::demoteCaptain(std::string nickname)
{
    for (std::vector<Client*>::iterator it = channel_operators.begin(); it != channel_operators.end(); ++it) {
        if ((*it)->get_nickname() == nickname) {
            channel_operators.erase(it); return;
		}
    }
}

void Channel::echoToAll(const std::string &msg, std::string senderNick){
    for (size_t i = 0; i < joined_users.size(); ++i) {
        if (joined_users[i]->get_nickname() != senderNick) {
            send(joined_users[i]->get_ffd(), msg.c_str(), msg.size(), 0);
        }
    }
}

void Channel::echoSystemMessage(const std::string &msg){
    for (size_t i = 0; i < joined_users.size(); ++i) {
        send(joined_users[i]->get_ffd(), msg.c_str(), msg.size(), 0);
    }
}

std::vector<std::string> Channel::getBuddyNames() {
    std::vector<std::string> names;
    for (size_t i = 0; i < joined_users.size(); ++i) {
        std::string name = joined_users[i]->get_nickname();
        for (size_t j = 0; j < channel_operators.size(); j++) {
            if (channel_operators[j]->get_nickname() == joined_users[i]->get_nickname()) {
                name = "@" + name;  // Prepend @ for operators
                break;
            }
        }
        names.push_back(name);
    }
    return names;
}

Channel* LoungeManager::findLounge(std::string channel_name) {
    for (size_t i = 0; i < allLounges.size(); ++i) {
        if (channel_name == allLounges[i]->getRoomName()) {
            return allLounges[i];
        }
    }
    return NULL;
}

Channel* LoungeManager::openLounge(std::string channel_name) {
    Channel* exist_channel = findLounge(channel_name);
    if (!exist_channel) {
        Channel* newChannel = new Channel(channel_name);
        allLounges.push_back(newChannel);
        return newChannel;
    }
    return exist_channel;
}

void LoungeManager::closeLounge(const std::string& channelName)
{
    for (std::vector<Channel*>::iterator it = allLounges.begin(); it != allLounges.end(); ++it) {
        if ((*it)->getRoomName() == channelName) {
            delete *it;
            allLounges.erase(it);
            return;
        }
    }
}
