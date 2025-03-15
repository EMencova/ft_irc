/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 08:23:00 by mac               #+#    #+#             */
/*   Updated: 2025/03/15 07:18:47 by mac              ###   ########.fr       */
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
#include <algorithm>
#include <unordered_map>

#include "Channel.hpp"

class Channel;
class Client {
	private:
		int							_fd;
		std::string					_host;
		std::string					_port;
		std::string					_username;
		std::string					_nickname;
		Channel						*_channel;
		bool						_registered;
		bool						_is_operator;
		std::string					_partialMessage; //CGM

	public:
		Client ();
		Client(int fd, std::string port, std::string host);
		~Client();

		int getFd();
		std::string getHost();
		std::string getPort();
		std::string getUsername();
		std::string getNickname();
		Channel *getChannel();
		bool getRegistered();
		bool get_IsOperator();
		std::string &getBuffer();

		void setFd(int fd);
		void setHost(std::string host);
		void setPort(std::string port);
		void setUsername(std::string username);
		void setNickname(std::string nickname);
		void setChannel(Channel *channel);
		void setRegistered(bool registered);
		void set_IsOperator(bool is_operator);

		void welcomeMessage(std::string host, std::string port);
		void sendMessage(std::string message, int fd);
		void readMessage(int fd);
		void joinChannel(Channel *channel);
		void leaveChannel();
		void appendToBuffer(const std::string &data);
};

#endif
