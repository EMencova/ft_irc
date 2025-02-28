/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eliskam <eliskam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/02/28 22:04:07 by eliskam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./inc/Server.hpp"




Server::Server(){};

Server::Server(int port_input, const std::string &password)
  : _Serv_Socket(-1), serverAddr(), client_addr(), _port(port_input), _password(password)
{
    this->serverAddr.sin_family = AF_INET;
	this->serverAddr.sin_addr.s_addr = INADDR_ANY;
	this->serverAddr.sin_port = htons(this->_port);
	_start_time = getCurrentDateTime();
	_hostname = inet_ntoa(this->serverAddr.sin_addr);
}

Server::Server(const Server &original)
{
        _port = original._port;
        _password = original._password;
        _hostname = original._hostname;
        _start_time = original._start_time;
        serverAddr = original.serverAddr;
        client_addr = original.client_addr;
        _clients = original._clients;
        //_channels = original._channels;
}

Server &Server::operator=(const Server &original)
{
    if (this != &original)
    {
        _port = original._port;
        _password = original._password;
        _hostname = original._hostname;
        _start_time = original._start_time;
        serverAddr = original.serverAddr;
        client_addr = original.client_addr;
        _clients = original._clients;
        //_channels = original._channels;
    }
        return(*this);
}

Server::~Server()
{
    for (std::vector<Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
		delete (*it);
	if (this->_Serv_Socket != -1)
		close(this->_Serv_Socket);
}


//GETTERS


bool Server::irc_started()
{
    _Serv_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_Serv_Socket == -1)
		return (false);

	int yes = 1;
	if (setsockopt(_Serv_Socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		return (false);

	if (bind(_Serv_Socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
		return (false);

	if (listen(_Serv_Socket, 10) == -1)
		return (false);

	printInfoServer(this);
	return (true);
}

int Server::getPort() const
{
    return (_port);
}

std::string Server::getHostname() const
{
    return (_hostname);
}
std::string Server::getPassword() const
{
    return (_password);
}

std::string Server::getStartTime() const
{
    return (_start_time);
}

std::string Server::getClientsOn() const
{
	if (_clients.empty())
		return "No clients online.\n";

	std::string response = "Clients online:\n";
	for (size_t i = 0; i < _clients.size(); ++i) {
		response += _clients[i]->getNick() + (i < _clients.size() - 1 ? ", " : "");
	}
	return response;
}

//TODO
/*
std::string Server::getChannelsOn() const
{
    return();
}
void Server::printChannelMembers() const
{
    return ();
}
*/