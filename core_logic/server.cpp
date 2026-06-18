#include "../Server.hpp"

Server::Server(int port, const std::string &password) : Port_(port), Pass_(password),
Server_fd(-1), _serverActive(false) 
{
    socketcreating();
    Addrsetuping();
    bindsocket();
    startlistening();
}
Server::~Server() 
{
    for (std::map<int, Client*>::iterator it = _connectedClients.begin(); it != _connectedClients.end(); ++it) 
    {
        close(it->first);
        delete it->second;
    }
    close(Server_fd);
}
void Server::run() 
{
    _serverActive = true;
    _pollDescriptors.push_back((pollfd){Server_fd, POLLIN, 0});
    
    while (_serverActive && g_server_state) 
    {
        int state = poll(&_pollDescriptors[0], _pollDescriptors.size(), -1);
        if (state < 0 && errno != EINTR)
            throw (std::runtime_error("poll error : " + std::string(strerror(errno))));

        // On utilise un itérateur inversé classique pour pouvoir effacer en toute sécurité
        // sans casser les indices des éléments restants et sans recréer de logique lourde.
        for (int i = static_cast<int>(_pollDescriptors.size()) - 1; i >= 0; i--) 
        {
            if (_pollDescriptors[i].revents & POLLIN) 
            {
                if (_pollDescriptors[i].fd == Server_fd)
                    handlingnewconnection();
                else
                    receiveclientdata(_pollDescriptors[i].fd);
            }
        }
    }
    _serverActive = false;
}

void Server::socketcreating() 
{
    Server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (Server_fd == -1)
        throw (std::runtime_error("socket error : " + std::string(strerror(errno))));
    int option = 1;
    if (setsockopt(Server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
        close(Server_fd), throw (std::runtime_error("setsockopt error : " + std::string(strerror(errno))));
    if (fcntl(Server_fd, F_SETFL, O_NONBLOCK) == -1)
        close(Server_fd), throw (std::runtime_error("fcntl error : " + std::string(strerror(errno))));
}

void Server::Addrsetuping() 
{
    memset(&Server_addr_, 0, sizeof(Server_addr_));
    Server_addr_.sin_family = AF_INET;
    Server_addr_.sin_addr.s_addr = INADDR_ANY;
    Server_addr_.sin_port = htons(Port_);
}

void Server::bindsocket() 
{
    if (bind(Server_fd, (struct sockaddr *)&Server_addr_, sizeof(Server_addr_)) == -1)
        close(Server_fd), throw (std::runtime_error("bind error : " + std::string(strerror(errno))));
}

void Server::startlistening() 
{
    if (listen(Server_fd, SOMAXCONN) == -1)
        close(Server_fd), throw (std::runtime_error("listen error : " + std::string(strerror(errno))));
    std::cout << GRE << SERV() << END << std::endl;
    std::cout << GRE << "\tServer is running on port " << Port_ << std::endl;
    std::cout << YEL << "\tWaiting for connections..." << std::endl;
}

void Server::handlingnewconnection() 
{
    struct sockaddr_in  clientaddr;
    socklen_t           addrlen = sizeof(clientaddr);
    int                 clientFd = accept(Server_fd, (struct sockaddr*)&clientaddr, &addrlen);
    if (clientFd == -1) 
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK) 
        {
            return;
        }
        std::cerr << "accept error : " << strerror(errno) << std::endl;
        return;
    }
    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1) {
        close(clientFd), std::cerr << "fcntl error when add new client : " << strerror(errno) << std::endl;
        return; 
    }
    Client *newclient = new Client(clientFd);
    _connectedClients[clientFd] = newclient;
    _pollDescriptors.push_back((pollfd){clientFd, POLLIN, 0});
    std::cout << BLU << "New client connected with fd: " << clientFd << END << std::endl;
}

void Server::receiveclientdata(int clientFd) 
{
    char buf[1024];
    ssize_t nbytes = recv(clientFd, buf, sizeof(buf) - 1, 0);

    if (nbytes <= 0) {
        if (nbytes == 0) 
        {
            // ✅ Fix de compilation : on passe directement le clientFd à ton unique fonction de déconnexion
            disconnectClient(clientFd);
            return;
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) 
        {
            return;
        } 
        else 
        {
            disconnectClient(clientFd);
            std::cerr << "recv error: " << strerror(errno) << std::endl;
            return;
        }
    }
    buf[nbytes] = '\0';
    Client* client = _connectedClients[clientFd];
    client->add_to_buffer(buf);
    
    std::string buff = client->get_buffer();

    // ✅ Affichage sécurisé si jamais la chaîne reçue est tronquée ou vide
    size_t last_nl = buff.find_last_of('\n');
    if (last_nl != std::string::npos && buff.substr(0, 4) != "PING" && buff.substr(0, 4) != "PONG")
        std::cout << GRA << "--> " <<  buff.substr(0, last_nl) << END << std::endl;

    size_t delimeter;
    while (true)
    {
        if (_connectedClients.find(clientFd) == _connectedClients.end())
            return;
        delimeter = client->get_buffer().find("\r\n");
        if (delimeter != std::string::npos) {
            std::string msg = client->get_buffer().substr(0, delimeter);
            client->clear_buffer(delimeter + 2);
            executecommand(client, msg);
            continue;
        }

        delimeter = client->get_buffer().find('\n');
        if (delimeter != std::string::npos) {
            std::string msg = client->get_buffer().substr(0, delimeter);
            client->clear_buffer(delimeter + 1);
            executecommand(client, msg);
            continue;
        }
        break;
    }
}

void Server::disconnectClient(int clientFd) 
{
    std::map<int, Client*>::iterator it = _connectedClients.find(clientFd);
    if (it == _connectedClients.end()) 
        return;

    Client* client = it->second;
    std::vector<std::string> channelstocleanup;
    for (std::vector<Channel*>::iterator channelit = _channelManager.allLounges.begin(); channelit != _channelManager.allLounges.end(); ++channelit) 
    {
        if ((*channelit)->Is_Member(client)) 
        {
            (*channelit)->waveGoodbye(client);
            if ((*channelit)->isLonely()) 
            {
                channelstocleanup.push_back((*channelit)->getRoomName());
            }
        }
    }
    for (size_t i = 0; i < channelstocleanup.size(); ++i) 
    {
        _channelManager.closeLounge(channelstocleanup[i]);
    }
    for (std::vector<pollfd>::iterator polliterator = _pollDescriptors.begin(); polliterator != _pollDescriptors.end(); ++polliterator) 
    {
        if (polliterator->fd == clientFd) 
        {
            _pollDescriptors.erase(polliterator);
            break;
        }
    }
    delete client;
    _connectedClients.erase(clientFd);
    close(clientFd);
}

void Server::dispatchCommand(Client* cl, const std::string& cmd, const std::vector<std::string>& args) 
{
    if      (cmd == "PASS")    
        verifyPassword(cl, args);
    else if (cmd == "USER")    
        registerUser(cl, args);
    else if (cmd == "NICK")    
        changeNickname(cl, args);
    else if (cmd == "JOIN")    
        joinChannel(cl, args);
    else if (cmd == "PRIVMSG") 
        sendMessage(cl, args);
    else if (cmd == "KICK")    
        kickClient(cl, args);
    else if (cmd == "INVITE")  
        inviteClient(cl, args);
    else if (cmd == "TOPIC")  
         manageTopic(cl, args);
    else if (cmd == "MODE")    
        manageMode(cl, args);
    #ifdef BONUS
        else if (cmd == "DCCFILE") handleDccCommand(cl, args);
    #endif
    else if (cmd == "QUIT")    
        disconnectClient(cl->get_ffd()); // ✅ Fix de compilation : on passe le fd du client directement !
    else if (cmd != "PING" && cmd != "PONG")
        sendReplay(cl->get_ffd(), ERR_UNKNOWNCOMMAND(cmd));
}

void Server::executecommand(Client* cl, const std::string& command) 
{
    std::istringstream  stream(command);
    std::vector<std::string> args;
    std::string         cmd;
 
    stream >> cmd;
    if (!cmd.empty() && cmd[cmd.size() - 1] == '\r')
        cmd.erase(cmd.size() - 1);

    for (std::string token; stream >> token; ) 
    {
        if (token[0] == ':') 
        {
            std::string trailing = (token.size() > 1) ? token.substr(1) : "";
            std::string remainder;
            std::getline(stream, remainder);
            trailing += remainder;
            
            if (!trailing.empty() && trailing[trailing.size() - 1] == '\r')
                trailing.erase(trailing.size() - 1);
                
            args.push_back(trailing);
            break;
        }
        if (!token.empty() && token[token.size() - 1] == '\r')
            token.erase(token.size() - 1);
            
        args.push_back(token);
    }
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    dispatchCommand(cl, cmd, args);
}

void Server::authenticatingclient(Client *client) 
{
    if (client->is_pass_ok() && client->is_nick_ok() && client->is_user_ok() && !client->is_registered()) 
    {
        client->set_registered(true);
        welcomingmsg(client);
    }
}

void Server::welcomingmsg(Client* client) const 
{
    sendReplay(client->get_ffd(), RPL_WELCOME(client->get_nickname(), client->get_username(), client->get_ip()));
    sendReplay(client->get_ffd(), RPL_YOURHOST(client->get_nickname()));
    sendReplay(client->get_ffd(), RPL_CREATED(client->get_nickname()));
    sendReplay(client->get_ffd(), RPL_MYINFO(client->get_nickname()));
}

void Server::sendReplay(int fd, const std::string& response) const 
{
    std::string formatted = response;
    if (formatted.find("\r\n") == std::string::npos)
        formatted += "\r\n";
    send(fd, formatted.c_str(), formatted.size(), 0);
}

Client* Server::searchForUser(std::string nickname)
{
    if (!nickname.empty() && nickname[nickname.size() - 1] == '\r')
        nickname.erase(nickname.size() - 1);
    for (std::map<int, Client*>::iterator it = _connectedClients.begin(); it != _connectedClients.end(); ++it) {
        if (it->second && it->second->get_nickname() == nickname) 
        {
            return it->second;
        }
    }
    return NULL;
}