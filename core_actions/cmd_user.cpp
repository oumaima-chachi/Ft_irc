#include "../Server.hpp"

bool Server::validateUser(Client* client, const std::vector<std::string>& params)
{
    if (!client->is_pass_ok())
    {
        sendReplay(client->get_ffd(), ERR_NOTREGISTERED(std::string("*")));
        return false;
    }
    if (client->is_registered())
    {
        sendReplay(client->get_ffd(), ERR_ALREADYREGISTRED(std::string("*")));
        return false;
    }
    if (params.size() < 4)
    {
        sendReplay(client->get_ffd(), ERR_NEEDMOREPARAMS(std::string("USER")));
        return false;
    }
    return true;
}

void Server::registerUser(Client* client, const std::vector<std::string>& params)
{
    if (!validateUser(client, params))
        return;
    client->set_username(params[0]);
    client->set_realname(params[3].substr(1));
    authenticatingclient(client);
}