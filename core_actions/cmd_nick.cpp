#include "../Server.hpp"

bool Server::validateNick(Client* client, const std::vector<std::string>& params)
{
    if (!client->is_pass_ok())
    {
        sendReplay(client->get_ffd(), ERR_NOTREGISTERED(std::string("*")));
        return false;
    }
    if (params.size() != 1)
    {
        sendReplay(client->get_ffd(), ERR_NEEDMOREPARAMS(std::string("NICK")));
        return false;
    }
    if (params.empty())
    {
        sendReplay(client->get_ffd(), ERR_NONICKNAMEGIVEN(client->get_nickname()));
        return false;
    }
    return true;
}

bool Server::NickValid(const std::string& nickname) 
{
    if (nickname.empty() || nickname.length() > 9)
        return false;
    const std::string special = "-[]\\`^{}";
    for (size_t i = 0; i < nickname.length(); ++i) 
    {
        char c = nickname[i];
        if (i == 0 && !isalpha(c) && special.find(c) == std::string::npos)
            return false;
        if (!isalnum(c) && special.find(c) == std::string::npos)
            return false;
    }
    return true;
}

void Server::changeNickname(Client* client, const std::vector<std::string>& params)
{
    if (!validateNick(client, params))
        return;

    std::string nickname = params[0];
    std::transform(nickname.begin(), nickname.end(), nickname.begin(), ::tolower);

    if (client->get_nickname() == nickname)
    {
        if (client->is_registered())
            return;
    }
    if (!NickValid(nickname))
    {
        sendReplay(client->get_ffd(), ERR_ERRONEUSNICKNAME(client->get_nickname(), nickname));
        return;
    }
    if (findClientByNickname(nickname))
    {
        sendReplay(client->get_ffd(), ERR_NICKNAMEINUSE(client->get_nickname(), nickname));
        return;
    }

    std::string oldNick = client->get_nickname();
    client->set_nickname(nickname);

    std::string msg = ":" + oldNick + " NICK :" + nickname + "\r\n";

    for (std::vector<Channel*>::iterator it = _channelManager.allLounges.begin(); it != _channelManager.allLounges.end(); ++it)
    {
        Channel* channel = *it;
        if (channel->Is_Member(client))
            channel->echoToAll(msg, oldNick);
    }
    authenticatingclient(client);
}
