#include "../Server.hpp"

void Server::processTopic(Client* client, Channel* channel, const std::string& channelName, const std::vector<std::string>& params)
{
    if (params.size() == 1)
    {
        std::string topic = channel->getRoomTitle();
        sendReplay(client->get_ffd(), RPL_TOPIC(topic, client->get_nickname(), channelName));
    }
    else
    {
        if (!channel->isCaptain(client) && channel->isTitleLocked())
        {
            sendReplay(client->get_ffd(), ERR_CHANOPRIVSNEEDED(channelName));
            return;
        }
        std::string newTopic = (params[1].empty()) ? "" : params[1];
        channel->setRoomTitle(newTopic);

        if (newTopic[0] == ':')
            newTopic = newTopic.substr(1);
        std::string topicMsg = ":" + client->get_nickname() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
        channel->echoToAll(topicMsg, "");
    }
}

void Server::manageTopic(Client* client, const std::vector<std::string>& params)
{
    if (!client->is_registered())
    {
        sendReplay(client->get_ffd(), ERR_NOTREGISTERED(std::string("*")));
        return;
    }
    if (params.empty())
    {
        sendReplay(client->get_ffd(), ERR_NEEDMOREPARAMS(std::string("TOPIC")));
        return;
    }
    std::string channelName = params[0];
    Channel* channel = _channelManager.findLounge(channelName);

    if (!channel)
    {
        sendReplay(client->get_ffd(), ERR_NOSUCHCHANNEL(client->get_nickname(), channelName));
        return;
    }
    if (!channel->Is_Member(client))
    {
        sendReplay(client->get_ffd(), ERR_NOTONCHANNEL(channelName));
        return;
    }
    processTopic(client, channel, channelName, params);
}
