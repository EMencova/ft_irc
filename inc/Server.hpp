/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/01 23:15:57 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <csignal>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <map>
#define MAXCLIENT 1000


class Server
{
	private:
		int							_running;
		int							_socket;

		const std::string			_host;
		const std::string			_port;
		const std::string			_pswrd;

		std::vector<pollfd>			_pollfds;
		// std::map<int, Client *>		_clients;
		// std::vector<Channel *>		_channels;
		// CommandHandler*				_commandHandler;

	public:
		Server();
		Server(const std::string &port, const std::string &pswrd);
		// Server(int port_input, const std::string &pswrd);
		// Server(const Server &original);
		// Server &operator=(const Server &original);
		~Server();

		int createNewSocket();

};
