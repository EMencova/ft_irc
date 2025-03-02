/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/02 09:03:45 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

Server::Server(){};

Server::Server(const std::string &port, const std::string &pswrd)
		: _running(1), _host("127.0.0.1"), _port(port), _pswrd(pswrd) {
		_socket = createNewSocket();

}

Server::~Server(){};


int Server::createNewSocket(){
	// server: fds, memset(), bind(), listen(), accept()
	// client:

	int socket_fd;

// 	struct sockaddr_in {
// 	__uint8_t       sin_len;
// 	sa_family_t     sin_family;
// 	in_port_t       sin_port;
// 	struct  in_addr sin_addr;
// 	char            sin_zero[8];
// };
	struct sockaddr_in server_addr;

	if ((socket_fd = socket(AF_INET, SOCK_STREAM , 0)) < 0){
		std::cerr << ("socket_fd failed");
		close (socket_fd);
		return (1);
	}

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(std::stoi(_port));
	// server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(socket_fd, (sockaddr *) &server_addr, sizeof(server_addr)) < 0){
		perror ("bind failed");
		close (socket_fd);
		return (1);
	}

	if ((listen(socket_fd, MAXCLIENT)) < 0) {
		perror ("listen failed");
		close (socket_fd);
		return (1);
	}

	return socket_fd;
}

void Server::startServer(){
	pollfd server_fd = {_socket, POLLIN, 0};
	_pollfds.push_back(server_fd);

	std::cout << "server listening on: " << _port << std::endl;

	while (_running)
	{
		// block indefinitely until an event occurs poll()
		if (poll(_pollfds.data(), _pollfds.size(), -1) < 0){
			perror("poll count");
			break;
		}

		// Check if the listening socket is ready (new connection)
		for (pollfds_iterator p_it = _pollfds.begin(); p_it != _pollfds.end(); p_it++){
			if (p_it->revents == 0)
				continue;

			if ((p_it->revents & POLLHUP) == POLLHUP)
				thisClientDisconnect(p_it->fd);

			if ((p_it->revents & POLLIN) == POLLIN)
				if (p_it->fd == _socket) {
					thisClientConnect();
					break;
				}

			if (p_it->revents == POLLOUT) {
				thisClientMessage(p_it->fd);
			}
			else {
				std::cerr << "Unknown event" << std::endl;
			}
		}
	}
}

void Server::thisClientConnect(){
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(_socket, (sockaddr *) &client_addr, &client_addr_len);

	if (client_fd < 0){
		perror("accept failed");
		return;
	}

	pollfd client_fd_poll = {client_fd, POLLIN, 0};
	_pollfds.push_back(client_fd_poll);

	char hostname[1024];
	if ((getnameinfo((sockaddr *) &client_addr, client_addr_len, hostname, 1024, NULL, 0, 0)) == 0)
		std::cout << "New connection from: " << hostname << std::endl;
	else
		std::cerr << "New connection from unknown host" << std::endl;

	Client *new_client = new Client(client_fd, _pswrd);
	_clients[client_fd] = new_client;
}

void Server::thisClientDisconnect(int client_fd){
	Client *client = _clients[client_fd];
	//client should leave all channels
	// for (Channel *channel : client->getChannels())
	// 	channel->removeClient(client);

	_clients.erase(client_fd);

	//remove the client from the pollfds
	for (pollfds_iterator p_it = _pollfds.begin(); p_it != _pollfds.end(); p_it++) {
		if (p_it->fd != client_fd)
			continue;
		_pollfds.erase(p_it);
		close(client_fd);
		break;
	}

	delete client;
}

