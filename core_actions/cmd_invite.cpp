#include "../Server.hpp"

bool Server::validateInvite(Client* client, const std::vector<std::string>& params)
{
    if (!client->is_registered())
    {
        sendReplay(client->get_ffd(), ERR_NOTREGISTERED(std::string("*")));
        return false;
    }
    if (params.empty() || params.size() < 2)
    {
        sendReplay(client->get_ffd(), ERR_NEEDMOREPARAMS(std::string("INVITE")));
        return false;
    }
    return true;
}

void Server::inviteClient(Client* client, const std::vector<std::string>& params)
{
    if (!validateInvite(client, params))
        return;

    std::string nickName = params[0];
    std::string channelName = params[1];
    std::string reason = "";

    if (params.size() > 2 && params[2][0] == ':')
        reason = params[2];

    Client* invitedClient = searchForUser(nickName);
    if (!invitedClient)
    {
        sendReplay(client->get_ffd(), ERR_NOSUCHNICK(nickName));
        return;
    }
    Channel* channel = _channelManager.findLounge(channelName);
    if (!channel)
    {
        sendReplay(client->get_ffd(), ERR_NOSUCHCHANNEL(client->get_nickname(), channelName));
        return;
    }
    if (!channel->isCaptain(client))
    {
        sendReplay(client->get_ffd(), ERR_CHANOPRIVSNEEDED(channelName));
        return;
    }
    if (channel->Is_Member(invitedClient))
    {
        sendReplay(client->get_ffd(), ERR_USERONCHANNEL(client->get_nickname(), invitedClient->get_nickname(), channelName));
        return;
    }
    sendReplay(client->get_ffd(), RPL_INVITING(client->get_nickname(), nickName, channelName));
    sendReplay(invitedClient->get_ffd(), RPL_INVITED(client->get_nickname(), nickName, channelName, client->get_info()));
    channel->welcomeGuest(invitedClient);
}
