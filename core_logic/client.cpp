#include "../Client.hpp"

Client::Client(int fd) : _fd(fd), _is_pass_ok(false), _is_nick_ok(false), _is_user_ok(false), _is_registered(false) 
{
	struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char clientIp[INET_ADDRSTRLEN];
	
    if (getpeername(fd, (struct sockaddr *)&client_addr, &client_len) == -1)
		throw std::runtime_error("getpeername failed");
    if (inet_ntop(AF_INET, &client_addr.sin_addr, clientIp, sizeof(clientIp)) == NULL)
		throw std::runtime_error("inet_ntop failed");
    _ip = std::string(clientIp);
}

Client::~Client() {}

void Client::add_to_buffer(const std::string& data) { BuFfer += data;}
std::string& Client::get_buffer() { return BuFfer; }
void Client::clear_buffer(size_t pos) { BuFfer.erase(0, pos);}

std::string Client::get_info() const 
{
    return _nickname + "!" + _username + "@" + _ip;
}

int Client::get_ffd() const {return _fd;}

bool Client::is_registered() const {return _is_registered;}

bool Client::is_pass_ok() const {return _is_pass_ok;}

bool Client::is_nick_ok() const {return _is_nick_ok;}

bool Client::is_user_ok() const {return _is_user_ok;}

std::string Client::get_ip() const {return  _ip;}

std::string Client::get_realname() const {return _realname;}

std::string Client::get_nickname() const {return _nickname;}

std::string Client::get_username() const {return _username;}

void Client::set_nickname(const std::string& nickName) 
{
	_nickname = nickName, _is_nick_ok = true;
}
void Client::set_pass_ok(bool status) 
{
	_is_pass_ok = status;
}
void Client::set_registered(bool status) 
{
	_is_registered = status;
}
void Client::set_username(const std::string& userName) 
{
	_username = userName, _is_user_ok = true;
}
void Client::set_realname(const std::string& realName) 
{
	_realname = realName;
}