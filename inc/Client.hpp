/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 08:23:00 by mac               #+#    #+#             */
/*   Updated: 2025/03/02 08:49:34 by mac              ###   ########.fr       */
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
		std::string					_pswrd;
		std::vector<pollfd>			_pollfds;
		std::vector<Channel *>		_channels;

	public:
		Client(int fd, const std::string &pswrd);
		~Client();
		// void						thisClientDisconnect(int fd);
		// void						thisClientConnect();
		// void						thisClientMessage(int fd);


}

#endif
