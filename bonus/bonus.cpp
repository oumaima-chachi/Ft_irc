#include "../Server.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>

struct FileOffer 
{
    std::string senderNick;
    std::string fileName;
    std::string fileData;
    size_t      fileSize;
};

static std::map<std::string, FileOffer> s_offers;

static std::string offerKey(const std::string& receiver, const std::string& sender) 
{
    std::string r = receiver;
    std::string s = sender;
    std::transform(r.begin(), r.end(), r.begin(), ::toupper);
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return r + "|" + s;
}

std::string Server::getBotNick() const 
{
    return "IRCBot";
}

void Server::notice(int fd, const std::string& nick, const std::string& msg) 
{
    sendReplay(fd, ":server NOTICE " + nick + " :" + msg + "\r\n");
}

void Server::handleBotMessage(Client* sender, const std::string& msg) 
{
    std::string reply;

    if (msg == "!help") 
    {
        reply = "Commands: !help | !joke | !time | !hello";
    } else if (msg == "!hello" || msg == "!hi") 
    {
        reply = "Hello, " + sender->get_nickname() + "! Welcome to the server.";
    } else if (msg == "!time") 
    {
        time_t now = time(NULL);
        std::string t = ctime(&now);
        if (!t.empty() && t[t.size() - 1] == '\n')
            t.erase(t.size() - 1);
        reply = "Server time: " + t;
    } else if (msg == "!joke") 
    {
        const std::string jokes[3] = 
        {
            "Why do programmers prefer dark mode? Light attracts bugs!",
            "A SQL query walks into a bar and asks two tables: 'Can I join you?'",
            "There are 10 types of people: those who understand binary and those who don't."
        };
        
        static bool seeded = false;
        if (!seeded) 
        {
            srand(static_cast<unsigned int>(time(NULL)));
            seeded = true;
        }
        reply = jokes[rand() % 3];
    } else 
    {
        reply = "Unknown command. Type !help for the list.";
    }

    std::string out = ":" + getBotNick() + "!bot@localhost PRIVMSG " +
                      sender->get_nickname() + " :" + reply + "\r\n";
    sendReplay(sender->get_ffd(), out);
}

static std::string b64Encode(const std::string& data) 
{
    static const std::string C = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    int val = 0, valb = -6;
    for (size_t i = 0; i < data.size(); i++) {
        unsigned char c = data[i];
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(C[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
        out.push_back(C[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4)
        out.push_back('=');
    return out;
}

void Server::handleDccCommand(Client* client, const std::vector<std::string>& params)
{
    if (!client->is_registered()) 
    {
        sendReplay(client->get_ffd(), ERR_NOTREGISTERED(std::string("*")));
        return;
    }
    if (params.empty()) {
        notice(client->get_ffd(), client->get_nickname(), "Usage: DCCFILE SEND <nick> <file>  |  DCCFILE GET <nick>");
        return;
    }

    std::string sub = params[0];
    std::transform(sub.begin(), sub.end(), sub.begin(), ::toupper);

    std::vector<std::string> cleanParams = params;
    for (size_t i = 0; i < cleanParams.size(); ++i) 
    {
        cleanParams[i].erase(std::remove(cleanParams[i].begin(), cleanParams[i].end(), '\r'), cleanParams[i].end());
        cleanParams[i].erase(std::remove(cleanParams[i].begin(), cleanParams[i].end(), '\n'), cleanParams[i].end());
    }
    if (sub == "SEND") 
    {
        if (cleanParams.size() < 3) 
        {
            notice(client->get_ffd(), client->get_nickname(), "Usage: DCCFILE SEND <nick> <filepath>");
            return;
        }
        const std::string& targetNick = cleanParams[1];
        const std::string& filePath   = cleanParams[2];

        Client* target = findClientByNickname(targetNick);
        if (!target) 
        { 
            sendReplay(client->get_ffd(), ERR_NOSUCHNICK(targetNick)); 
            return; 
        }

        std::string path = filePath;
        size_t start = path.find_first_not_of(" \t");
        size_t end   = path.find_last_not_of(" \t");
        if (start == std::string::npos) 
        {
            notice(client->get_ffd(), client->get_nickname(), "Error: empty file path.");
            return;
        }
        path = path.substr(start, end - start + 1);

        if (path.find("..") != std::string::npos) 
        {
            notice(client->get_ffd(), client->get_nickname(), "Error: invalid path.");
            return;
        }

        notice(client->get_ffd(), client->get_nickname(), "Opening: [" + path + "]");
        std::ifstream f(path.c_str(), std::ios::binary);
        if (!f.is_open()) 
        {
            notice(client->get_ffd(), client->get_nickname(), "Error: cannot open file: " + path);
            return;
        }

        std::ostringstream ss;
        ss << f.rdbuf();
        std::string raw = ss.str();
        
        if (raw.size() > 8 * 1024 * 1024) 
        {
            notice(client->get_ffd(), client->get_nickname(), "Error: file too large (max 8MB).");
            return;
        }

        std::string fileName = path;
        size_t sl = path.find_last_of("/\\");
        if (sl != std::string::npos) {
            fileName = path.substr(sl + 1);
        }

        FileOffer offer;
        offer.senderNick = client->get_nickname();
        offer.fileName   = fileName;
        offer.fileData   = raw;
        offer.fileSize   = raw.size();

        s_offers[offerKey(target->get_nickname(), client->get_nickname())] = offer;

        std::ostringstream szStr;
        szStr << raw.size();
        notice(client->get_ffd(), client->get_nickname(), "File offer sent to " + target->get_nickname() + ".");
        notice(target->get_ffd(), target->get_nickname(),
            client->get_nickname() + " wants to send you '" + fileName +
            "' (" + szStr.str() + " bytes). Type: DCCFILE GET " + client->get_nickname());
    }

    else if (sub == "GET") 
    {
        if (cleanParams.size() < 2) 
        {
            notice(client->get_ffd(), client->get_nickname(), "Usage: DCCFILE GET <sender_nick>");
            return;
        }
        const std::string& senderNick = cleanParams[1];

        std::string key = offerKey(client->get_nickname(), senderNick);

        if (s_offers.find(key) == s_offers.end()) 
        {
            notice(client->get_ffd(), client->get_nickname(), "No pending file from " + senderNick + ".");
            return;
        }
        
        FileOffer& offer = s_offers[key];
        std::string encoded = b64Encode(offer.fileData);
        std::ostringstream szStr;
        szStr << offer.fileSize;

        std::string dataMsg = ":" + offer.senderNick + "!dcc@localhost PRIVMSG " +
                              client->get_nickname() + " :\x01" "DCC FILE " +
                              offer.fileName + " " + szStr.str() + " " + encoded + "\x01\r\n";
        
        send(client->get_ffd(), dataMsg.c_str(), dataMsg.size(), 0);

        Client* senderClient = findClientByNickname(senderNick);
        if (senderClient) 
        {
            notice(senderClient->get_ffd(), senderClient->get_nickname(),
                client->get_nickname() + " accepted '" + offer.fileName + "'.");
        }

        s_offers.erase(key);
    }
    else 
    {
        notice(client->get_ffd(), client->get_nickname(), "Unknown subcommand. Use SEND or GET.");
    }
}