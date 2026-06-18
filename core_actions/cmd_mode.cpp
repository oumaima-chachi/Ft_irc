#include "../Server.hpp"

void Server::handleModeI(Client* client, Channel* channel, const std::string& channelName, char sign, const std::vector<std::string>& params)
{
    if (params.size() > 2)
    {
        sendReplay(client->get_ffd(), ERR_INVALIDMODEPARAM(client->get_nickname(), channelName, params[2]));
        return;
    }
    if (sign == '+')
    {
        if (channel->getKnockOnly())
        {
            sendReplay(client->get_ffd(), ERR_KEYSET(channelName));
            return;
        }
        channel->setKnockOnly(1);
        channel->addBadge('i');
    }
    else
    {
        if (!channel->getKnockOnly())
        {
            sendReplay(client->get_ffd(), ERR_KEYSET(channelName));
            return;
        }
        channel->setKnockOnly(0);
        channel->removeBadge('i');
    }
}

void Server::handleModeT(Client* client, Channel* channel, const std::string& channelName, char sign, const std::vector<std::string>& params)
{
    if (params.size() > 2)
    {
        sendReplay(client->get_ffd(), ERR_INVALIDMODEPARAM(client->get_nickname(), channelName, params[2]));
        return;
    }
    if (sign == '+')
        channel->lockTitle(true);
    else
        channel->lockTitle(false);
}

void Server::handleModeK(Client* client, Channel* channel, const std::string& channelName, char sign, const std::vector<std::string>& params)
{
    if (sign == '+')
    {
        if (params.size() < 3)
        {
            sendReplay(client->get_ffd(), ERR_NEEDMOREPARAMS(std::string("MODE")));
            return;
        }
        if (params.size() > 3)
        {
            sendReplay(client->get_ffd(), ERR_INVALIDMODEPARAM(client->get_nickname(), channelName, params[2]));
            return;
        }
        channel->set_pass_ok(params[2]);
        channel->addBadge('k');
    }
    else
    {
        if (params.size() > 2)
        {
            sendReplay(client->get_ffd(), ERR_INVALIDMODEPARAM(client->get_nickname(), channelName, params[2]));
            return;
        }
        channel->set_pass_ok("");
        channel->removeBadge('k');
    }
}

void Server::handleModeO(Client* client, Channel* channel, const std::string& channelName, char sign, const std::vector<std::string>& params)
{
    if (params.size() < 3)
    {
        sendReplay(client->get_ffd(), ERR_NEEDMOREPARAMS(std::string("MODE")));
        return;
    }
    if (params.size() > 3)
    {
        sendReplay(client->get_ffd(), ERR_INVALIDMODEPARAM(client->get_nickname(), channelName, params[2]));
        return;
    }
    Client* mem = searchForUser(params[2]);
    if (!mem)
    {
        return;
    }
    if (sign == '+')
    {
        if (!channel->Is_Member(mem))
        {
            sendReplay(client->get_ffd(), ERR_USERNOTINCHANNEL(client->get_nickname(), channelName));
            return;
        }
        if (channel->isCaptain(mem))
        {
            sendReplay(client->get_ffd(), ERR_OPSET(client->get_nickname()));
            return;
        }
        channel->addCaptain(mem->get_nickname());
    }
    else
    {
        if (!channel->isCaptain(mem))
        {
            sendReplay(client->get_ffd(), RPL_ISNOTOP(mem->get_nickname()));
            return;
        }
        if (channel->countCaptains() == 1)
        {
            sendReplay(client->get_ffd(), ERR_OP(mem->get_nickname()));
            return;
        }
        channel->demoteCaptain(mem->get_nickname());
    }
}

void Server::handleModeL(Client* client, Channel* channel, const std::string& channelName, char sign, const std::vector<std::string>& params)
{
    if (sign == '+')
    {
        if (params.size() < 3 || params[2].empty())
        {
            sendReplay(client->get_ffd(), ERR_NEEDMOREPARAMS(std::string("MODE")));
            return;
        }
        if (params.size() > 3)
        {
            sendReplay(client->get_ffd(), ERR_INVALIDMODEPARAM(client->get_nickname(), channelName, params[2]));
            return;
        }
        int lim = 0;
        std::stringstream iss(params[2]);
        iss >> lim;
        for (size_t i = 0; i < params[2].size(); i++)
        {
            if (params[2][i] < '0' || params[2][i] > '9')
            {
                sendReplay(client->get_ffd(), ERR_INVALIDMODEPARAM(client->get_nickname(), channelName, params[2]));
                return;
            }
        }
        if (lim < static_cast<int>(channel->countBuddies()))
        {
            sendReplay(client->get_ffd(), ERR_INVALIDLIMIT(client->get_nickname()));
            return;
        }
        channel->setMaxBuddies(lim);
        channel->addBadge('l');
    }
    else
    {
        if (params.size() > 2)
        {
            sendReplay(client->get_ffd(), ERR_INVALIDMODEPARAM(client->get_nickname(), channelName, params[2]));
            return;
        }
        channel->setMaxBuddies(0);
        channel->removeBadge('l');
    }
}

void Server::manageMode(Client* client, const std::vector<std::string>& params)
{
    if (!client->is_registered())
    {
        sendReplay(client->get_ffd(), ERR_NOTREGISTERED(std::string("*")));
        return;
    }
    if (params.empty())
    {
        sendReplay(client->get_ffd(), ERR_NEEDMOREPARAMS(std::string("MODE")));
        return;
    }
    std::string channelName = params[0];
    Channel* channel = _channelManager.findLounge(channelName);
    if (!channel)
    {
        sendReplay(client->get_ffd(), ERR_NOSUCHCHANNEL(client->get_nickname(), channelName));
        return;
    }
    if (params.size() == 1)
    {
        std::string modes = "";
        for (size_t i = 0; i < channel->getBadges().size(); i++)
            modes += channel->getBadges()[i];
        std::string modeMsg;
        if (modes.empty())
            modeMsg = ": 324 " + client->get_nickname() + " " + channelName + " (no modes set)\r\n";
        else
            modeMsg = ": 324 " + client->get_nickname() + " " + channelName + " +" + modes + "\r\n";
        sendReplay(client->get_ffd(), modeMsg);
        return;
    }
    if (!channel->isCaptain(client))
    {
        sendReplay(client->get_ffd(), ERR_CHANOPRIVSNEEDED(channelName));
        return;
    }
    if (params[1].size() == 2 && (params[1][0] == '-' || params[1][0] == '+'))
    {
        char sign = params[1][0];
        if (params[1][1] == 'i')
            handleModeI(client, channel, channelName, sign, params);
        else if (params[1][1] == 't')
            handleModeT(client, channel, channelName, sign, params);
        else if (params[1][1] == 'k')
            handleModeK(client, channel, channelName, sign, params);
        else if (params[1][1] == 'o')
            handleModeO(client, channel, channelName, sign, params);
        else if (params[1][1] == 'l')
            handleModeL(client, channel, channelName, sign, params);
        else
        {
            sendReplay(client->get_ffd(), ERR_NOCHANMODES(client->get_nickname(), channelName));
            return;
        }
    }
    else if (params.size() > 2 && !params[2].empty() && params[2] != "+sn")
    {
        sendReplay(client->get_ffd(), ERR_NOCHANMODES(client->get_nickname(), channelName));
        return;
    }
    if (params.size() > 2)
        channel->echoSystemMessage(RPL_CHANGEMODE(channelName, params[1], params[2]));
    else
        channel->echoSystemMessage(RPL_CHANGEMODE(channelName, params[1], ""));
}
