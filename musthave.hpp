#pragma once

#include <iostream>
#include <sstream>
#include <vector> 
#include <map>
#include <algorithm>
#include <csignal> 

#include <cstring>
#include <cstdlib>
#include <cerrno>

#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <poll.h> 
#include <fcntl.h> 
#include <unistd.h> 

#include "codes.hpp"


#define RED "\e[38;5;203m"
#define GRE "\e[38;5;121m"
#define YEL "\e[38;5;222m"
#define BLU "\e[38;5;117m"
#define MAG "\e[38;5;212m"
#define CYA "\e[38;5;159m"
#define WHI "\e[38;5;253m"
#define GRA "\e[38;5;243m"
#define END "\e[0m"
#define BLA "\e[38;5;235m"
#define PUR "\e[38;5;141m"
#define ORA "\e[38;5;215m"

extern volatile sig_atomic_t g_server_state;