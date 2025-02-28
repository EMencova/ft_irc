/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eliskam <eliskam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/02/28 22:03:51 by eliskam          ###   ########.fr       */
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
#include <map>
#include "Client.hpp"

class Server
{
    private:
        int                             _Serv_Socket;
	    struct sockaddr_in		    serverAddr,client_addr;
	   // std::map<std::string, Channel> _channels;
	    std::vector<Client*>		   _clients;
	    std::string					   _hostname;
	    std::string					   _start_time;
	    int							   _port;
	    std::string					   _password;

    public:
        Server();
        Server(int port_input, const std::string &password);
        Server(const Server &original);
        Server &operator=(const Server &original);
        ~Server();

	    bool		irc_started();
	    int			getPort() const;
	    std::string getHostname() const;
	    std::string getPassword() const;
	    std::string getStartTime() const;
	    std::string getClientsOn() const;
	    std::string getChannelsOn() const;

        //TODO
	    void		printChannelMembers() const;
        void		monitorConnections();
    
    
};

std::string	getCurrentDateTime();
void	printInfoServer(Server* server);