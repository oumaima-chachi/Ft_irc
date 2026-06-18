#include "../Server.hpp"

std::vector<std::string> Server::split(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end;

    while ((end = str.find(delimiter, start)) != std::string::npos)
	{
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
	}
    tokens.push_back(str.substr(start));
    return tokens;
}

std::vector<std::string> Server::parsePasswords(const std::vector<std::string>& params)
{
    std::vector<std::string> passwords;
    if (params.size() > 1)
    {
        std::string passParam = params[1];
        if (!passParam.empty() && passParam[0] == ':')
            passParam = passParam.substr(1);
        std::istringstream iss(passParam);
        std::string token;
        while (std::getline(iss, token, ' '))
        {
            if (!token.empty())
            {
                std::istringstream tokenStream(token);
                std::string subToken;
                while (std::getline(tokenStream, subToken, ','))
                {
                    if (!subToken.empty())
                        passwords.push_back(subToken);
                }
            }
        }
    }
    return passwords;
}

void Server::joinChannel(Client* client, const std::string& channelName, const std::string& password)
{
    if (channelName == "#")
    {
        sendReplay(client->get_ffd(), ERR_NEEDMOREPARAMS(std::string("JOIN")));
        return;
    }
    if (channelName[0] != '#')
    {
        sendReplay(client->get_ffd(), ERR_NOSUCHCHANNEL(client->get_nickname(), channelName));
        return;
    }
    Channel* channel = _channelManager.findLounge(channelName);
    if (channel && !channel->getRoomSecret().empty())
    {
        if (channel->getRoomSecret() != password)
        {
            sendReplay(client->get_ffd(), ERR_BADCHANNELKEY(client->get_nickname(), channelName));
            return;
        }
    }
    if (channel && channel->getMaxBuddies() != 0)
    {
        if (channel->countBuddies() == (size_t)channel->getMaxBuddies())
        {
            sendReplay(client->get_ffd(), ERR_CHANNELISFULL(client->get_nickname(), channelName));
            return;
        }
    }
    if (channel && channel->getKnockOnly() == 1)
    {
        if (!channel->hasPass(client->get_nickname()))
        {
            sendReplay(client->get_ffd(), ERR_INVITEONLYCHAN(client->get_nickname(), channelName));
            return;
        }
    }
    if (channel && channel->Is_Member(client))
    {
        sendReplay(client->get_ffd(), ERR_USERONCHANNEL(client->get_nickname(), client->get_nickname(), channelName));
        return;
    }
    if (!channel)
    {
        channel = _channelManager.openLounge(channelName);
        channel->promoteToCaptain(client);
    }
    channel->cuddleIn(client);

    sendReplay(client->get_ffd(), RPL_JOIN(client->get_nickname(), client->get_username(), channelName, client->get_ip()));
    channel->echoToAll(RPL_JOIN(client->get_nickname(), client->get_username(), channelName, client->get_ip()), client->get_nickname());

    std::vector<std::string> names = channel->getBuddyNames();
    std::string namesStr;
    for (size_t j = 0; j < names.size(); j++)
    {
        if (!namesStr.empty()) namesStr += " ";
        namesStr += names[j];
    }
    std::string topic = channel->getRoomTitle();
    if (topic[0] == ':')
        topic = topic.substr(1);
    sendReplay(client->get_ffd(), RPL_TOPIC(topic, client->get_nickname(), channelName));
    sendReplay(client->get_ffd(), RPL_NAMREPLY(client->get_nickname(), channelName, namesStr));
    sendReplay(client->get_ffd(), RPL_ENDOFNAMES(client->get_nickname(), channelName));
}

void Server::joinChannel(Client* client, const std::vector<std::string>& params)
{
    if (!client->is_registered())
    {
        sendReplay(client->get_ffd(), ERR_NOTREGISTERED(std::string("*")));
        return;
    }
    if (params.empty())
    {
        sendReplay(client->get_ffd(), ERR_NEEDMOREPARAMS(std::string("JOIN")));
        return;
    }
    std::vector<std::string> channels = split(params[0], ',');
    std::vector<std::string> passwords = parsePasswords(params);

    for (size_t i = 0; i < channels.size(); i++)
    {
        std::string password = (i < passwords.size()) ? passwords[i] : "";
        joinChannel(client, channels[i], password);
    }
}
