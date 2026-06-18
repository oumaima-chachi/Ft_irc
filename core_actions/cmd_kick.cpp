#include "../Server.hpp"

void Server::processKick(Client* client, Channel* channel, const std::string& channelName, const std::string& nickname, const std::string& reason)
{
    std::string cleanReason = reason;
    if (!cleanReason.empty() && cleanReason[0] == ':')
    {
        cleanReason = cleanReason.substr(1);
    }

    std::string kickMsg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_ip() + " KICK " + channelName + " " + nickname + " :" + cleanReason + "\r\n";

    channel->echoToAll(kickMsg, "");
    channel->removeUuser(nickname, kickMsg);

    if (channel->isCaptainListEmpty() && !channel->isLonely())
    {
        Client* newOp = channel->firstMember();
        if (newOp)
        {
            channel->promoteToCaptain(newOp);
            std::string modeMsg = ":FT_irc MODE " + channel->getRoomName() + " +o " + newOp->get_nickname() + "\r\n";
            channel->echoToAll(modeMsg, "");
        }
    }

    if (channel->isLonely())
    {
        _channelManager.closeLounge(channelName);
    }
}

void Server::kickClient(Client* client, const std::vector<std::string>& params)
{
    if (!client->is_registered())
    {
        sendReplay(client->get_ffd(), ERR_NOTREGISTERED(std::string("*")));
        return;
    }
    if (params.size() < 2)
    {
        sendReplay(client->get_ffd(), ERR_NEEDMOREPARAMS(std::string("KICK")));
        return;
    }
    std::string channelName = params[0];
    std::string nickname = params[1];
    
    // Nettoyage du nickname et channelName pour éviter les faux négatifs dans la recherche
    while (!nickname.empty() && (nickname[nickname.size() - 1] == '\r' || nickname[nickname.size() - 1] == '\n' || nickname[nickname.size() - 1] == ' '))
        nickname.erase(nickname.size() - 1);
    while (!channelName.empty() && (channelName[channelName.size() - 1] == '\r' || channelName[channelName.size() - 1] == '\n'))
        channelName.erase(channelName.size() - 1);

    std::string reason = "Kicked by operator";
    if (params.size() > 2)
        reason = params[2];

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
    if (!channel->checkForClient(nickname))
    {
        sendReplay(client->get_ffd(), ERR_USERNOTINCHANNEL(nickname, channelName));
        return;
    }
    processKick(client, channel, channelName, nickname, reason);
}