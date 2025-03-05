/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/05 06:34:36 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <csignal>
#include <sstream>
#include <algorithm>
#include "Client.hpp"
#include "Channel.hpp"

#define MAXCLIENT 1000

class Server {
private:
	typedef std::vector<pollfd>::iterator pollfds_iterator;
	typedef std::vector<Channel *>::iterator channels_iterator;

	int                         _running;
	int                         _socket;

	const std::string           _host;
	const std::string           _port;
	const std::string           _pswrd;

	std::vector<pollfd>         _pollfds;
	std::map<int, Client *>     _clients;
	std::vector<Channel *>      _channels;

public:
	Server();
	Server(const std::string &port, const std::string &pswrd);
	~Server();

	int createNewSocket();
	void startServer();
	void thisClientConnect();
	void thisClientDisconnect(int client_fd);
	void thisClientMessage(int client_fd, Client *sender);

	void addClientToChannel(Client *client, Channel *channel);
	Channel *createNewChannel(std::string &channel_name, std::string &channel_password, Client *client);
	Channel *getChannelByName(std::string &channel_name);
	Client *getClientByFd(int client_fd);
	std::string readMessage(int client_fd, Client *client);
	void authenticateClient(Client *client);

	void privateMessageClient(Client *sender, std::string message);
	void joinChannel(Client *client, std::string message);
	void setNickname(Client *client, std::string message);
	void setUsername(Client *client, std::string message);
	void makeOperator(Client *client, std::string message);

	// New methods for channel commands
	void kickClient(Client *sender, const std::string &message);
	void inviteClient(Client *sender, const std::string &message);
	void setTopic(Client *sender, const std::string &message);
	void setChannelMode(Client *sender, const std::string &message);
};

#endif
