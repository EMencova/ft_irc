/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 10:55:10 by mac               #+#    #+#             */
/*   Updated: 2025/03/10 14:43:15 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_RESPONSE_HPP
#define IRC_RESPONSE_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <time.h>

/* Error Responses */
#define ERR_NOTREGISTERED(source)						("451 " + std::string(source) + " :You have not registered")
#define ERR_ALREADYREGISTERED(source)					("462 " + std::string(source) + " :You may not register")
#define ERR_PASSWDMISMATCH(source)						("464 " + std::string(source) + " :Password is incorrect")

#define ERR_NONICKNAMEGIVEN(source)						("431 " + std::string(source) + " :Nickname not given")
#define ERR_NICKNAMEINUSE(source)						("433 " + std::string(source) + " " + std::string(source)  + " :Nickname is already in use")

#define ERR_UNKNOWNCOMMAND(source, command)				("421 " + std::string(source) + " " + std::string(command) + " :Unknown command")
#define ERR_NEEDMOREPARAMS(source, command)				("461 " + std::string(source) + " " + std::string(command) + " :Not enough parameters")
#define ERR_TOOMANYCHANNELS(source, channel)			("405 " + std::string(source) + " " + std::string(channel) + " :You have joined too many channels")
#define ERR_NOTONCHANNEL(source, channel)				("442 " + std::string(source) + " " + std::string(channel) + " :You're not on that channel")
#define ERR_NOSUCHCHANNEL(source, channel)				("403 " + std::string(source) + " " + std::string(channel) + " :No such channel")
#define ERR_BADCHANNELKEY(source, channel)				("475 " + std::string(source) + " " + std::string(channel) + " :Cannot join channel (+k)")
#define ERR_CHANNELISFULL(source, channel)				("471 " + std::string(source) + " " + std::string(channel) + " :Cannot join channel (+l)")
#define ERR_CANNOTSENDTOCHAN(source, channel)			("404 " + std::string(source) + " " + std::string(channel) + " :Cannot send to channel")
#define ERR_CHANOPRIVSNEEDED(source, channel)			("482 " + std::string(source) + " " + std::string(channel) + " :You're not channel operator")
#define ERR_NOSUCHNICK(source, nickname)				("401 " + std::string(source) + " " + std::string(nickname) + " :No such nick/channel")
#define ERR_USERNOTINCHANNEL(source, nickname, channel) ("441 " + std::string(source) + " " + std::string(nickname) + " " + std::string(channel) + " :They aren't on that channel")

/* Numeric Responses */
#define RPL_WELCOME(source)								("001 " + std::string(source) + " :Welcome " + std::string(source) + " to the ft_irc network")
#define RPL_NAMREPLY(source, channel, users)			("353 " + std::string(source) + " = " + std::string(channel) + " :" + std::string(users))
#define RPL_ENDOFNAMES(source, channel)					("366 " + std::string(source) + " " + std::string(channel) + " :End of /NAMES list.")

/* Command Responses */
#define RPL_JOIN(source, channel)						(":" + std::string(source) + " JOIN :" + std::string(channel))
#define RPL_PART(source, channel)						(":" + std::string(source) + " PART :" + std::string(channel))
#define RPL_PING(source, token)							(":" + std::string(source) + " PONG :" + std::string(token))
#define RPL_PRIVMSG(source, target, message)			(":" + std::string(source) + " PRIVMSG " + std::string(target) + " :" + std::string(message))
#define RPL_NOTICE(source, target, message)				(":" + std::string(source) + " NOTICE " + std::string(target) + " :" + std::string(message))
#define RPL_QUIT(source, message)						(":" + std::string(source) + " QUIT :Quit: " + std::string(message))
#define RPL_KICK(source, channel, target, reason)		(":" + std::string(source) + " KICK " + std::string(channel) + " " + std::string(target) + " :" + std::string(reason))
#define RPL_MODE(source, channel, modes, args)			(":" + std::string(source) + " MODE " + std::string(channel) + " " + std::string(modes) + " " + std::string(args))


static inline void irc_log(const std::string& message)
{
	time_t		rawtime;
	struct tm	*timeinfo;
	char		buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	std::string timeStr(buffer);
	std::cout << "\033[0;34m[" << timeStr << "]\033[0m " << message << std::endl;
}

#endif
