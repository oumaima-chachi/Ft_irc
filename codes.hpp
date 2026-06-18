#pragma once
#include "musthave.hpp"

#define PREFIX ":FT_irc "
#define POSTFIX "\r\n" 

// Codes d'authentification et de bienvenue
#define RPL_WELCOME(nickname, username, ipaddress) PREFIX " 001 " + nickname + " :Welcome to our community! Happy to see you here (" + nickname + "!" + username + "@" + ipaddress + ")" + POSTFIX
#define RPL_YOURHOST(nickname) PREFIX " 002 " + nickname + " :Your host is FT_irc, running a custom stable version 1.0" + POSTFIX
#define RPL_CREATED(nickname) PREFIX " 003 " + nickname + " :This awesome server was launched on " + __DATE__ + POSTFIX
#define RPL_MYINFO(nickname) PREFIX " 004 " + nickname + " :FT_irc 1.0 - -" + POSTFIX

// Erreurs de commandes et d'arguments
#define ERR_NEEDMOREPARAMS(command) PREFIX " 461 " + command + " :Oops! It looks like you forgot some arguments for this command. Don't worry, try again!" + POSTFIX
#define ERR_ALREADYREGISTRED(nickname) PREFIX " 462 " + nickname + " :Access denied, buddy! You are already part of our family on this server!" + POSTFIX
#define ERR_PASSWDMISMATCH(nickname) PREFIX " 464 " + nickname + " :Oh no, connection failed... The password you provided isn't the right one, try one more time!" + POSTFIX
#define ERR_UNKNOWNCOMMAND(command) PREFIX " 421 " + command + " :Hmm, I don't know this command... Type /HELP if you're feeling a little lost!" + POSTFIX
#define ERR_NOTREGISTERED(none) PREFIX " 451 " + none + " :Hold on a second! You need to register first before doing that. Let's get you set up! " + POSTFIX

// Gestion des Nicknames
#define ERR_NONICKNAMEGIVEN(nickname) PREFIX " 431 " + nickname + " :Please give us a nickname so we can know how to call you! " + POSTFIX
#define ERR_ERRONEUSNICKNAME(nickname, nick) PREFIX " 432 " + nickname + " " + nick + " :Oh, this nickname has some invalid characters. Let's pick a cleaner one! " + POSTFIX
#define ERR_NICKNAMEINUSE(nickname, nick) PREFIX " 433 " + nickname + " " + nick + " :Sorry, someone else is already using this awesome nickname. Time to be creative! " + POSTFIX

// Gestion des Canaux (Erreurs et Infos)
#define ERR_NOSUCHCHANNEL(nickname, channel) PREFIX " 403 " + nickname + " " + channel + " :This channel doesn't exist yet, but you can always be the first to create it! " + POSTFIX
#define ERR_NOTONCHANNEL(channel) PREFIX " 442 " + channel + " :You can't do that because you aren't a member of this channel yet" + POSTFIX
#define ERR_NOSUCHNICK(nickname) PREFIX " 401 " + nickname + " :We couldn't find any user or channel with that name" + POSTFIX
#define RPL_LISTEND(none) PREFIX " 323 " + none + " :End of channels list" + POSTFIX

// Restrictions d'accès aux Canaux
#define ERR_INVITEONLYCHAN(nick, channel) PREFIX " 473 " + nick + " " + channel + " :This channel is private (+i). You need an invitation to join" + POSTFIX
#define ERR_CHANNELISFULL(nick, channelName) PREFIX " 471 " + nick + " " + channelName + " :Cannot join (+l) - The channel has reached its maximum user limit" + POSTFIX
#define ERR_BADCHANNELKEY(nick, channelName) PREFIX " 475 " + nick + " " + channelName + " :Access denied (+k) - The channel key or password is wrong" + POSTFIX

// Actions et Informations de Canal
#define RPL_TOPIC(topic, nick, channelName) PREFIX " 332 " + nick + " " + channelName + " :" + topic + POSTFIX
#define RPL_NOTOPIC(nickname, channel) PREFIX " 331 " + nickname + " " + channel + " :No topic is currently set for this channel" + POSTFIX
#define RPL_NAMREPLY(nickname, channel, names) PREFIX " 353 " + nickname + " = " + channel + " :" + names + POSTFIX
#define RPL_ENDOFNAMES(nickname, channel) PREFIX " 366 " + nickname + " " + channel + " :End of members list" + POSTFIX
#define RPL_JOIN(nick, username, channelname, ipaddress) ":" + nick + "!" + username + "@" + ipaddress + " JOIN " + channelname + POSTFIX
#define RPL_QUIT(nickname, message) ":" + nickname + " QUIT :Leaving the server: " + message + "\r\n"

// Gestion des Invitations
#define ERR_USERONCHANNEL(nickName, invitedUser, channelName) PREFIX " 443 " + nickName + " " + invitedUser + " " + channelName + " :No need to invite them, this user is already on the channel" + POSTFIX
#define RPL_INVITING(inviting, invited, channel) PREFIX " 341 " + inviting + " " + invited + " " + channel + POSTFIX
#define RPL_INVITED(nick, invited, channel, info) ":" + info + " INVITE " + invited + " :" + channel + POSTFIX

// Gestion des Opérateurs et des Modes
#define ERR_CHANOPRIVSNEEDED(channel) PREFIX " 482 " + channel + " :Permission denied! You need to be a channel operator (@) to do this" + POSTFIX
#define ERR_NOCHANMODES(nickname, channel) PREFIX " 477 " + nickname + " " + channel + " :This specific channel does not support external modes" + POSTFIX
#define ERR_INVALIDMODEPARAM(nickname, channel, param) PREFIX " 500 " + nickname + " " + channel + " " + param + " :The parameter provided for this mode is invalid" + POSTFIX
#define RPL_CHANNELMODEIS(nickname, channel, mode, params) PREFIX " 324 " + nickname + " " + channel + " " + mode + " " + params + POSTFIX
#define ERR_USERNOTINCHANNEL(nickname, channel) PREFIX " 441 " + nickname + " " + channel + " :This user is not active on that channel" + POSTFIX
#define ERR_KEYSET(channel) PREFIX " 467 " + channel + " :The key for this channel has already been configured" + POSTFIX
#define ERR_OPSET(nick) PREFIX " 491 " + nick + " :This user is already a channel operator" + POSTFIX
#define ERR_OP(nick) PREFIX " 494 " + nick + " :Security block: You cannot resign because you are the only operator left" + POSTFIX
#define RPL_CHANGEMODE(chName, mode, arguments) PREFIX " MODE " + chName + " " + mode + " " + arguments + POSTFIX
#define RPL_OPERATOR(nickname) ":" + nickname + " IS NOW AN OPERATOR" + POSTFIX
#define RPL_ISNOTOP(nickname) PREFIX " 492 " + nickname + " :This user does not possess operator privileges" + POSTFIX
#define ERR_INVALIDLIMIT(nickname) PREFIX " 493 " + nickname + " :The new limit cannot be lower than the current number of active members!" + POSTFIX

// BOT Notifications
#define _NOTICE(nickname, message) PREFIX " NOTICE " + nickname + " :" + message + POSTFIX


#define SERV() "\n\
 ▄████▄   ▓█████  ██▀███   ██▒   █▓ ▓█████  ██▀███  \n\
▒██▀ ▀█   ▓█   ▀ ▓██ ▒ ██▒ ▓██░   █▒ ▓█   ▀ ▓██ ▒ ██▒\n\
▒▓█    ▄  ▒███   ▓██ ░▄█ ▒  ▓██  █▒░ ▒███   ▓██ ░▄█ ▒\n\
▒▓▓▄ ▄██▒ ▒▓█  ▄ ▒██▀▀█▄     ▒██ █░░ ▒▓█  ▄ ▒██▀▀█▄  \n\
▒ ▓███▀ ░ ░▒████▒░██▓ ▒██▒    ▒▀█░   ░▒████▒░██▓ ▒██▒\n\
░ ░▒ ▒  ░ ░░ ▒░ ░░ ▒▓ ░▒▓░    ░ ▐░   ░░ ▒░ ░░ ▒▓ ░▒▓░\n\
  ░  ▒     ░ ░  ░  ░▒ ░ ▒░    ░ ░░    ░ ░  ░  ░▒ ░ ▒░\n\
░          ░       ░░   ░       ░░      ░     ░░   ░ \n\
░ ░        ░  ░     ░            ░      ░  ░   ░     \n\
░                               ░                    \n"
