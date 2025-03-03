/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/03 08:40:08 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

Server::Server(){};

Server::Server(const std::string &port, const std::string &pswrd)
		: _running(1), _host("127.0.0.1"), _port(port), _pswrd(pswrd) {
		_socket = createNewSocket();

}

Server::~Server(){};

int Server::createNewSocket() {
	int socket_fd;
	struct sockaddr_in server_addr;

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cerr << "socket_fd failed" << std::endl;
		close(socket_fd);
		return -1;
	}

	int server_flags = fcntl(socket_fd, F_GETFL, 0);
	if (server_flags < 0) {
		perror("get server flags failed");
		close(socket_fd);
		return -1;
	}

	if ((fcntl(socket_fd, F_SETFL, server_flags | O_NONBLOCK)) < 0) {
		perror("set server flags failed");
		close(socket_fd);
		return -1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(std::stoi(_port));

	if (bind(socket_fd, (sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		perror("bind failed");
		close(socket_fd);
		return -1;
	}

	if ((listen(socket_fd, MAXCLIENT)) < 0) {
		perror("listen failed");
		close(socket_fd);
		return -1;
	}

	return socket_fd;
}


void Server::startServer() {
	_pollfds.clear();
	pollfd server_fd = {_socket, POLLIN, 0};
	_pollfds.push_back(server_fd);

	std::cout << "Server listening on port: " << _port << std::endl;

	while (_running) {
		// Block indefinitely until an event occurs
		int poll_count = poll(_pollfds.data(), _pollfds.size(), -1);
		if (poll_count < 0) {
			perror("poll failed");
			break;
		}

		// Iterate through all FDs in the polling list
		for (size_t i = 0; i < _pollfds.size(); i++) {
			if (_pollfds[i].revents & POLLIN) {
				if (_pollfds[i].fd == _socket) {
					// New client connection
					thisClientConnect();
				} else {
					// Client message
					thisClientMessage(_pollfds[i].fd);
				}
			}

			if (_pollfds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
				// Client disconnected or error occurred
				thisClientDisconnect(_pollfds[i].fd);
			}
		}
	}
}

std::string Server::readMessage(int client_fd) {
	std::string message;
	char buffer[1024];
	ssize_t bytes_read;

	while (true) {
		bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

		if (bytes_read > 0) {
			buffer[bytes_read] = '\0';
			message.append(buffer);
			if (message.find("\r\n") != std::string::npos) {
				break;
			}
		} else if (bytes_read == 0) {
			std::cout << "Client FD " << client_fd << " disconnected." << std::endl;
			thisClientDisconnect(client_fd);
			return "";
		} else {
			if (errno == EWOULDBLOCK || errno == EAGAIN) {
				break;
			} else {
				perror("recv failed");
				thisClientDisconnect(client_fd);
				return "";
			}
		}
	}

	if (!message.empty()) {
		std::cout << "Message from FD " << client_fd << ": " << message << "\r\n";
	}

	return message;
}

void Server::thisClientConnect() {
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(_pollfds.begin()->fd, (sockaddr *) &client_addr, &client_addr_len);

	if (client_fd < 0) {
		if (errno != EWOULDBLOCK && errno != EAGAIN) {
			perror("accept failed");
		}
		return;
	}

	int client_fds_flags = fcntl(client_fd, F_GETFL, 0);
	if (client_fds_flags < 0) {
		perror("get client flags failed");
		close(client_fd);
		return;
	}

	if ((fcntl(client_fd, F_SETFL, client_fds_flags | O_NONBLOCK)) < 0) {
		perror("set client flags failed");
		close(client_fd);
		return;
	}

	if (_pollfds.size() < MAXCLIENT) {
		pollfd client_fd_poll = {client_fd, POLLIN, 0};
		_pollfds.push_back(client_fd_poll);

		char hostname[1024];
		if ((getnameinfo((sockaddr *) &client_addr, client_addr_len, hostname, 1024, NULL, 0, 0)) == 0) {
			std::cout << "New connection from: " << hostname << std::endl;
		} else {
			std::cerr << "New connection from unknown host" << std::endl;
		}

		Client *new_client = new Client(client_fd, std::to_string(client_addr.sin_port), std::string(hostname));
		_clients[client_fd] = new_client;

		new_client->setNickname("User" + std::to_string(client_fd));
		_clients[client_fd] = new_client;

		std::cout << "New client connected: FD " << client_fd << ", Nickname: " << new_client->getNickname() << std::endl;

		// std::cout << "New client connected: FD " << client_fd << std::endl;
	} else {
		std::cout << "Max clients reached, rejecting new connection." << std::endl;
		close(client_fd);
	}
}


void Server::thisClientDisconnect(int client_fd) {
	Client *client = _clients[client_fd];

	if (client->getChannel()) {
		client->getChannel()->removeClient(client);
	}

	for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
		if (it->fd == client_fd) {
			_pollfds.erase(it);
			break;
		}
	}

	close(client_fd);
	_clients.erase(client_fd);
	delete client;

	std::cout << "Client FD " << client_fd << " disconnected." << std::endl;
}

void Server::thisClientMessage(int client_fd) {
	std::string message = readMessage(client_fd);

	if (!message.empty()) {
		Client *sender = _clients[client_fd];
		std::string sender_nickname = sender->getNickname();

		// Check if the message is a private message
		if (message.find("/msg ") == 0) {
			// Parse the target nickname and message
			size_t space_pos = message.find(' ', 5);
			if (space_pos != std::string::npos) {
				std::string target_nickname = message.substr(5, space_pos - 5);
				std::string private_message = message.substr(space_pos + 1);

				// Find the target client
				Client *target = nullptr;
				for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
					if (it->second->getNickname() == target_nickname) {
						target = it->second;
						break;
					}
				}

				if (target) {
					// Send the private message to the target client
					std::string formatted_message = "[PM from " + sender_nickname + "]: " + private_message + "\r\n";
					target->sendMessage(formatted_message, target->getFd());
				} else {
					// Notify the sender that the target client was not found
					std::string error_message = "Error: Client '" + target_nickname + "' not found.\r\n";
					sender->sendMessage(error_message, sender->getFd());
				}
			}
		} else {
			// Broadcast the message to all clients in the same channel
			if (sender->getChannel()) {
				std::string formatted_message = "[" + sender_nickname + "]: " + message + "\r\n";
				sender->getChannel()->sendMessageToClients(formatted_message, sender);
			}
		}
	}
}


// void Server::thisClientMessage(int client_fd) {
// 	std::string message = readMessage(client_fd);
// 	if (!message.empty()) {
// 		Client *client = _clients[client_fd];
// 		if (client->getChannel()) {
// 			client->getChannel()->sendMessageToClients(message, client);
// 		}
// 	}
// }

Channel *Server::createNewChannel(std::string &channel_name, std::string &channel_password, Client *client){
	Channel *new_channel = new Channel(channel_name, channel_password, client);
	_channels.push_back(new_channel);
	client->setChannel(new_channel);
	return new_channel;
}

Channel *Server::getChannelByName(std::string &channel_name){
	for (channels_iterator ch_it = _channels.begin(); ch_it != _channels.end(); ch_it++)
		if ((*ch_it)->getName() == channel_name)
			return *ch_it;
	return nullptr;
}

Client *Server::getClientByFd(int client_fd){
	return _clients[client_fd];
}
