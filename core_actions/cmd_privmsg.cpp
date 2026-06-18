#include "../Server.hpp"

Client* Server::findClientByNickname(const std::string& nickname)
{
    for (std::map<int, Client*>::iterator it = _connectedClients.begin(); it != _connectedClients.end(); ++it) 
    {
        if (it->second->get_nickname() == nickname) 
        {
            return it->second;
        }
    }
    return NULL;
}

void Server::processPrivmsg(Client* client, const std::string& target, const std::string& msgToSend)
{
    #ifdef BONUS
        if (target == getBotNick()) 
        {
            size_t p = msgToSend.rfind(':');
            std::string plain = (p != std::string::npos) ? msgToSend.substr(p + 1) : msgToSend;
            while (!plain.empty() && (plain[plain.size() - 1] == '\r' || plain[plain.size() - 1] == '\n'))
            {
                plain.erase(plain.size() - 1);
            }
            handleBotMessage(client, plain);
            return;
        }
    #endif
    if (target[0] == '#')
    {
        Channel* channel = _channelManager.findLounge(target);
        if (!channel)
        {
            sendReplay(client->get_ffd(), ERR_NOSUCHNICK(target));
            return;
        }
        if (!channel->Is_Member(client))
        {
            sendReplay(client->get_ffd(), ERR_NOTONCHANNEL(target));
            return;
        }
        channel->echoToAll(msgToSend, client->get_nickname());
    }
    else
    {
        Client* recipient = findClientByNickname(target);
        if (!recipient)
        {
            sendReplay(client->get_ffd(), ERR_NOSUCHNICK(client->get_nickname()));
            return;
        }
        send(recipient->get_ffd(), msgToSend.c_str(), msgToSend.size(), 0);
    }
}

void Server::sendMessage(Client* client, const std::vector<std::string>& params)
{
    if (!client->is_registered())
    {
        sendReplay(client->get_ffd(), ERR_NOTREGISTERED(std::string("*")));
        return;
    }
    if (params.size() < 2)
    {
        sendReplay(client->get_ffd(), ERR_NEEDMOREPARAMS(std::string("PRIVMSG")));
        return;
    }
    std::vector<std::string> channels = split(params[0], ',');
    std::string recvmessage = params[1];
    std::string senderInfo = client->get_nickname() + "!" + client->get_username() + "@" + client->get_ip();

    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); it++)
    {
        std::string target = *it;
        std::string msgToSend = ":" + senderInfo + " PRIVMSG " + target + " :" + recvmessage + "\r\n";
        processPrivmsg(client, target, msgToSend);
    }
}