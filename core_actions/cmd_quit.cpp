#include "../Server.hpp"

void Server::processQuitChannels(Client* client, const std::string& quitMessage)
{
    for (std::vector<Channel*>::iterator it = _channelManager.allLounges.begin();
         it != _channelManager.allLounges.end(); ++it)
    {
        if ((*it)->Is_Member(client))
        {
            (*it)->echoToAll(RPL_QUIT(client->get_nickname(), quitMessage),
                             client->get_nickname());

            if ((*it)->isCaptain(client))
                (*it)->appointFirstCaptain();
        }
    }
}

void Server::disconnectClient(Client* client, const std::vector<std::string>& params)
{
    std::string quitMessage = "";
    if (!params.empty())
    {
        quitMessage = params[0];
    }
    processQuitChannels(client, quitMessage);
    std::cout << RED << "Client with fd : " << client->get_ffd() << " disconnected" << END << std::endl;
    disconnectClient(client->get_ffd());
}
