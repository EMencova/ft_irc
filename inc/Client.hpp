/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 08:23:00 by mac               #+#    #+#             */
/*   Updated: 2025/03/02 12:42:08 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include "Channel.hpp"
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "Channel.hpp"

class Client {
	private:
		int							_fd;
		std::string					_host;
		std::string					_port;
		std::string					_username;
		std::string					_nickname;
		Channel						*_channel;
		bool						_registered;

	public:
		Client ();
		Client(int fd, std::string port, std::string host);
		~Client();

		//getters
		int getFd();
		std::string getHost();
		std::string getPort();
		std::string getUsername();
		std::string getNickname();
		Channel *getChannel();
		bool getRegistered();

		//setters
		void setFd(int fd);
		void setHost(std::string host);
		void setPort(std::string port);
		void setUsername(std::string username);
		void setNickname(std::string nickname);
		void setChannel(Channel *channel);
		void setRegistered(bool registered);

		//methods
		//write a message, reply a mesaage to an fd, welcome message, join, leave, list, nick, user, who, whois, quit

		void welcomeMessage(std::string host, std::string port);
		void sendMessage(std::string message, int fd);
		void readMessage(int fd);
		void joinChannel(Channel *channel);
		void leaveChannel();
};

#endif
