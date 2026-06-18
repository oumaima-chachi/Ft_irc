#include "../Server.hpp"

bool Server::checkPassErrors(Client *client, const std::vector<std::string> &params)
{
    std::string rps;

    if (client->is_pass_ok())
    {
        rps = ERR_ALREADYREGISTRED(std::string("*"));
        sendReplay(client->get_ffd(), rps);
        return true;
    }

    if (params.empty())
    {
        rps = ERR_NEEDMOREPARAMS(std::string("PASS"));
        sendReplay(client->get_ffd(), rps);
        return true;
    }

    if (params[0] != Pass_)
    {
        rps = ERR_PASSWDMISMATCH(std::string("*"));
        sendReplay(client->get_ffd(), rps);
        return true;
    }
    return false;
}

void Server::verifyPassword(Client *client, const std::vector<std::string> &params)
{
    if (checkPassErrors(client, params))
        return;

    client->set_pass_ok(true);
}