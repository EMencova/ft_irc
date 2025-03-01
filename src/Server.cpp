/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eliskam <eliskam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/01 20:51:24 by eliskam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./inc/Server.hpp"

#pragma region CONSTRUCTORS


Server::Server(){};

Server::Server(int port_input, const std::string &password)
  :  _serv_Socket(-1), serverAddr(), client_addr(), _port(port_input), _password(password)
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
        _channels = original._channels;
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
        _channels = original._channels;
    }
        return(*this);
}

Server::~Server()
{
    for (std::vector<Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
		delete (*it);
	if (this-> _serv_Socket != -1)
		close(this-> _serv_Socket);
}

#pragma endregion

#pragma region GETTERS

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

Client *Server::getClient(const std::string &nick) const
{
    for (std::vector<Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if ((*it)->getNickname() == nick)
            return (*it);
    }
    return (NULL);
}


#pragma endregion



#pragma region GETTING ONLINE

bool Server::irc_started()
{
     _serv_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if ( _serv_Socket == -1)
		return (false);

	int yes = 1;
	if (setsockopt( _serv_Socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		return (false);

	if (bind( _serv_Socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
		return (false);

	if (listen( _serv_Socket, 10) == -1)
		return (false);

	printInfoServer(this);
	return (true);
}

std::string Server::getClientsOnline() const
{
	if (_clients.empty())
		return "There aren't any clients online.\n";

	std::string resp = "Clients online:\n";
	for (size_t i = 0; i < _clients.size(); ++i)
		resp += _clients[i]->getNickname() + (i < _clients.size() - 1 ? ", " : "");
	return (resp);
}

void Server::showChannels() const
{
    if (_channels.empty())
    {
        std::cout << "There aren't any active channels." << std::endl;
        return;
    }
    for (std::map<std::string, Channel>::const_iterator i = _channels.begin(); i != _channels.end(); ++i)
    {
        std::cout << i->first << ": ";
        std::map<std::string, Client> members = i->second.getMembers();
        for (std::map<std::string, Client>::const_iterator j = members.begin(); j != members.end(); ++j)
        {
            std::cout << j->second.getNickname();
            if (j != --members.end())
                std::cout << ", ";
        }
        std::cout << std::endl;
    }
}

std::string Server::getChannelsOnline() const
{
    if (_channels.empty())
        return ("There aren't any active channels.");
    std::string resp = "Active channels: ";
    for (std::map<std::string, Channel>::const_iterator i = _channels.begin(); i != _channels.end(); ++i)
        resp += i->first + (std::next(i) == _channels.end() ? "" : ", ");
    return (resp);
}

bool Server::clientCheck(const std::string &nickname) const
{
    for (std::vector<Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getNickname() == nickname)
            return (true);
    }
    return (false);
}

#pragma endregion

#pragma region OTHER FUNCTIONS


void Server::removeClient(fd_set &masterSet, int &clientFd)
{
    close(clientFd);
    FD_CLR(clientFd, &masterSet);

    for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if ((*it)->getSocket() == clientFd)
        {
            (*it)->commandQuit(_clients, "it has crashed..");
            delete *it;
            _clients.erase(it);
            return;
        }
    }
}

void Server::ExistingClient(fd_set &master_set, int client_fd, char buffer[1024])
{
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if ((*it)->getSocket() != client_fd)
			continue;

		(*it)->_buffer += buffer;

		while (true)
        {
				(*it)->processInput();
				if (!(*it)->_messageComplete)
					break;
				(*it)->_messageComplete = false;
			if (!(*it)->getConnected())
            {
				close(client_fd);
				FD_CLR(client_fd, &master_set);
				delete *it;
				_clients.erase(it);
				return;
			}
		}
		return;
	}		
}


void Server::addClient(fd_set &master_set, int &max_sd)
{
    int client_socket;
    socklen_t addr_len = sizeof(client_addr);

    client_socket = accept( _serv_Socket, (struct sockaddr *)&client_addr, &addr_len);
    if (client_socket == -1)
    {
        std::cerr<< "Error accepting client!"<<std::endl;
        return;
    }
}




