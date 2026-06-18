#include "../Server.hpp"

volatile sig_atomic_t g_server_state = 1;

void handlingshutdown(int signum) 
{
    (void)signum;
    g_server_state = 0;
}

int validPort(std::string isport) 
{
    size_t i = 0;
    if (isport[0] == '+')
        i++;
    for (; i < isport.size(); i++) {
        if (!std::isdigit(isport[i]))
            return -1;
    }

    int port;
    std::stringstream stream(isport);
    stream >> port;

    if (stream.fail() || (port < 1024 || port > 65535))
        return -1;
    return port;
}

int checkPassword(std::string password) 
{
    if (password.empty())
        return -1;
    for (size_t i = 0; i < password.size(); i++) 
    {
        if (std::isspace(password[i]))
            return -1;
    }
    return 0;
}

void run_server(char** av) 
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, handlingshutdown);
    signal(SIGTERM, handlingshutdown);
    
    int port = validPort(static_cast<std::string>(av[1]));
    if (port == -1) 
        throw std::runtime_error(RED "Error: port number is invalid  => " + static_cast<std::string>(av[1]));
        
    if (checkPassword(static_cast<std::string>(av[2])) == -1) 
        throw std::runtime_error("Error: invalid password configuration");
        
    Server server(port, av[2]);
    server.run();
}

int main(int ac, char** av) 
{
    if (ac != 3) 
    {
        std::cerr << "Usage: " << av[0] << " Error: bad arguments" << std::endl;
        return 1;
    }
    
    try 
    {
        run_server(av);
    } 
    catch (const std::exception& e) 
    {
        std::cerr << RED << e.what() << END << std::endl;
        return 1;
    }
    
    return 0;
}