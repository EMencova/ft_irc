/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eliskam <eliskam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/01 20:53:06 by eliskam          ###   ########.fr       */
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
#include "Channel.hpp"

class Server
{
    private:
        int                             _serv_Socket;
	    struct sockaddr_in		        serverAddr,client_addr;
	    std::map<std::string, Channel> _channels;
	    std::vector<Client*>		   _clients;
	    std::string					   _hostname;
	    std::string					   _start_time;
	    int							   _port;
	    std::string					   _password;

        Client *getClient(std::string const &nick) const;
        bool	clientCheck(std::string const &nick) const;
        
        void removeClient(fd_set &master_set, int &client_fd);
	    void ExistingClient(fd_set &master_set, int client_fd, char buffer[1024]);
	    void addClient(fd_set &master_set, int &max_sd); 
	    void processClientMessage(std::string buffer, std::vector<Client *>::iterator client_it);
	    
        //TO DO 
        void msgInit(Client *client, std::string const &buffer);
	    bool msgOfDay(Client &client);



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
	    std::string getClientsOnline() const;
	    std::string getChannelsOnline() const;
        void		showChannels() const;
    
    
};

std::string	getCurrentDateTime();
void	printInfoServer(Server* server);