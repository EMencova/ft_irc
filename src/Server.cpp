/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/05 11:37:54 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include "../inc/response.hpp"

Server::Server() { }

Server::Server(const std::string &port, const std::string &pswrd)
	: _running(1), _host("127.0.0.1"), _port(port), _pswrd(pswrd)
{
	_socket = createNewSocket();
}

Server::~Server() { }

int Server::createNewSocket() {
	int socket_fd;
	struct sockaddr_in server_addr;

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		irc_log("socket() failed");
		close(socket_fd);
		return -1;
	}

	int server_flags = fcntl(socket_fd, F_GETFL, 0);
	if (server_flags < 0) {
		irc_log("get server flags failed");
		close(socket_fd);
		return -1;
	}

	if ((fcntl(socket_fd, F_SETFL, server_flags | O_NONBLOCK)) < 0) {
		irc_log("set server flags failed");
		close(socket_fd);
		return -1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(std::stoi(_port));

	if (bind(socket_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		irc_log("bind() failed");
		close(socket_fd);
		return -1;
	}

	if ((listen(socket_fd, MAXCLIENT)) < 0) {
		irc_log("listen() failed");
		close(socket_fd);
		return -1;
	}

	irc_log("Server listening on port: " + _port);
	return socket_fd;
}

void Server::startServer() {
	_pollfds.clear();
	pollfd server_fd = { _socket, POLLIN, 0 };
	_pollfds.push_back(server_fd);

	irc_log("Server started on port: " + _port);

	while (_running) {
		int poll_count = poll(_pollfds.data(), _pollfds.size(), -1);
		if (poll_count < 0) {
			irc_log("poll() failed");
			break;
		}

		for (size_t i = 0; i < _pollfds.size(); i++) {
			if (_pollfds[i].revents & POLLIN) {
				if (_pollfds[i].fd == _socket) {
					// New client connection
					thisClientConnect();
				} else {
					// Client message
					thisClientMessage(_pollfds[i].fd, _clients[_pollfds[i].fd]);
				}
			}

			if (_pollfds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
				thisClientDisconnect(_pollfds[i].fd);
			}
		}
	}
}

std::string Server::readMessage(int client_fd, Client *client) {
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
			if (!client->getNickname().empty())
				irc_log("Client " + client->getNickname() + " disconnected.");
			else
				irc_log("Client FD " + std::to_string(client_fd) + " disconnected.");
			thisClientDisconnect(client_fd);
			return "";
		} else {
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				break;
			else {
				irc_log("recv() failed");
				thisClientDisconnect(client_fd);
				return "";
			}
		}
	}
	if (!message.empty()) {
		if (!client->getNickname().empty())
			irc_log("Message from " + client->getNickname() + ": " + message);
		else
			irc_log("Message from FD " + std::to_string(client_fd) + ": " + message);
	}
	return message;
}

void Server::thisClientConnect() {
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(_pollfds.begin()->fd, (sockaddr *)&client_addr, &client_addr_len);

	if (client_fd < 0) {
		if (errno != EWOULDBLOCK && errno != EAGAIN)
			irc_log("accept() failed");
		return;
	}

	int client_fds_flags = fcntl(client_fd, F_GETFL, 0);
	if (client_fds_flags < 0) {
		irc_log("get client flags failed");
		close(client_fd);
		return;
	}

	if ((fcntl(client_fd, F_SETFL, client_fds_flags | O_NONBLOCK)) < 0) {
		irc_log("set client flags failed");
		close(client_fd);
		return;
	}

	if (_pollfds.size() < MAXCLIENT) {
		pollfd client_fd_poll = { client_fd, POLLIN, 0 };
		_pollfds.push_back(client_fd_poll);

		char hostname[1024];
		if ((getnameinfo((sockaddr *)&client_addr, client_addr_len, hostname, 1024, NULL, 0, 0)) == 0)
			irc_log("New connection from: " + std::string(hostname));
		else
			irc_log("New connection from unknown host");

		Client *new_client = new Client(client_fd, std::to_string(client_addr.sin_port), std::string(hostname));

		authenticateClient(new_client);

		_clients[client_fd] = new_client;

		new_client->setNickname("User" + std::to_string(client_fd));
		_clients[client_fd] = new_client;

		irc_log("New client connected: FD " + std::to_string(client_fd) +
		        ", Nickname: " + new_client->getNickname());
	} else {
		irc_log("Max clients reached, rejecting new connection.");
		close(client_fd);
	}
}

void Server::thisClientDisconnect(int client_fd) {
	if (_clients.find(client_fd) == _clients.end())
		return;
	Client *client = _clients[client_fd];

	if (client->getChannel())
		client->getChannel()->removeClient(client);
	for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
		if (it->fd == client_fd) {
			_pollfds.erase(it);
			break;
		}
	}

	close(client_fd);
	_clients.erase(client_fd);
	delete client;
	irc_log("Client FD " + std::to_string(client_fd) + " disconnected.");
}

void Server::thisClientMessage(int client_fd, Client *sender) {
	std::string message = readMessage(client_fd, sender);
	if (message.empty())
		return;
	sender = _clients[client_fd];
	if (!sender->getRegistered()) {
		if (message.find("PASS ") == 0) {
			std::string password = message.substr(5);
			while (!password.empty() && (password.back() == '\n' || password.back() == '\r'))
				password.pop_back();

			if (password == _pswrd) {
				sender->setRegistered(true);
				std::string welcome_message = RPL_WELCOME("client") + "\r\n";
				sender->sendMessage(welcome_message, client_fd);
				return;
			} else {
				std::string error_message = ERR_PASSWDMISMATCH("server") + "\r\n";
				sender->sendMessage(error_message, client_fd);
				thisClientDisconnect(client_fd);
				return;
			}
		} else {
			std::string error_message = ERR_NOTREGISTERED("server") + "\r\n";
			sender->sendMessage(error_message, client_fd);
			thisClientDisconnect(client_fd);
			return;
		}
	}

	// Handle other commands only if client is registered
	if (message.find("PRIVMSG ") == 0) {
		privateMessageClient(sender, message);
	} else if (message.find("JOIN ") == 0) {
		joinChannel(sender, message);
	} else if (message.find("NICK ") == 0) {
		setNickname(sender, message);
	} else if (message.find("USER ") == 0) {
		setUsername(sender, message);
	} else if (message.find("OPER ") == 0) {
		makeOperator(sender, message);
	} else if (message.find("KICK ") == 0) {
		handleKick(sender, message);
	} else if (message.find("INVITE ") == 0) {
		handleInvite(sender, message);
	} else if (message.find("TOPIC ") == 0) {
		handleTopic(sender, message);
	} else if (message.find("MODE ") == 0) {
		handleMode(sender, message);
	} else {
		if (sender->getChannel()) {
			std::string formatted_message = "[" + sender->getNickname() + "]: " + message + "\r\n";
			sender->getChannel()->sendMessageToClients(formatted_message, sender);
		}
	}
}

Channel *Server::createNewChannel(std::string &channel_name, std::string &channel_password, Client *client) {
	Channel *new_channel = new Channel(channel_name, channel_password, client);
	_channels.push_back(new_channel);
	client->setChannel(new_channel);
	new_channel->setAdmin(client);
	return new_channel;
}

Channel *Server::getChannelByName(std::string &channel_name) {
	for (channels_iterator ch_it = _channels.begin(); ch_it != _channels.end(); ch_it++)
		if ((*ch_it)->getName() == channel_name)
			return *ch_it;
	return nullptr;
}

void Server::addClientToChannel(Client *client, Channel *channel) {
	channel->addClient(client);
	client->setChannel(channel);
}

Client *Server::getClientByFd(int client_fd) {
	return _clients[client_fd];
}
