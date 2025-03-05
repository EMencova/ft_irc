/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/05 05:54:28 by mac              ###   ########.fr       */
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
		int poll_count = poll(_pollfds.data(), _pollfds.size(), -1);
		if (poll_count < 0) {
			perror("poll failed");
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
			if (!client->getNickname().empty()) {
				std::cout << "Client " << client->getNickname() << " disconnected." << std::endl;
			} else {
				std::cout << "Client FD " << client_fd << " disconnected." << std::endl;
			}
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
		if (!client->getNickname().empty()) {
			std::cout << "Message from " << client->getNickname() << ": " << message << "\r\n";
		} else {
			std::cout << "Message from FD " << client_fd << ": " << message << "\r\n";
		}
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

		authenticateClient(new_client);

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

void Server::thisClientMessage(int client_fd, Client *sender) {

	sender = _clients[client_fd];
	std::string message = readMessage(client_fd, sender);

	if (message.empty()) {
		return;
	}
	std::string sender_nickname = sender->getNickname();

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
	} else {
		if (sender->getChannel()) {
			std::string formatted_message = "[" + sender_nickname + "]: " + message + "\r\n";
			sender->getChannel()->sendMessageToClients(formatted_message, sender);
		}
	}
}

void Server::authenticateClient(Client *client) {
	int client_fd = client->getFd();

	// Send a welcome message to the client
	std::string welcome_message = "Welcome to the server. Please authenticate using the PASS command.\r\n";
	send(client_fd, welcome_message.c_str(), welcome_message.size(), 0);

	// Wait for the client to send the PASS command
	std::string message;
	while (true) {
		message = readMessage(client_fd, client);

		if (!message.empty()) {
			if (message.find("PASS ") == 0) {
				std::string password = message.substr(5);
				if (!password.empty() && password.back() == '\n') {
					password.pop_back();
				}
				if (!password.empty() && password.back() == '\r') {
					password.pop_back();
				}

				// Debug: Print the received password
				std::cout << "Debug: Received password from FD " << client_fd << ": '" << password << "'" << std::endl;

				if (password == _pswrd) {
					std::cout << "Client FD " << client_fd << " authenticated successfully." << std::endl;
					client->setRegistered(true);

					std::string success_message = "Authentication successful. You are now connected to the server.\r\n";
					send(client_fd, success_message.c_str(), success_message.size(), 0);
					break;
				} else {

					std::string error_message = "Error: Wrong password. Disconnecting...\r\n";
					send(client_fd, error_message.c_str(), error_message.size(), 0);
					thisClientDisconnect(client_fd); //change this later to prompt for retry
					return;
				}
			} else {
				std::string error_message = "Error: You must authenticate using the PASS command.\r\n";
				send(client_fd, error_message.c_str(), error_message.size(), 0);
				thisClientDisconnect(client_fd);
				return;
			}
		}
		// (optional)
		usleep(100000);
	}
}

Channel *Server::createNewChannel(std::string &channel_name, std::string &channel_password, Client *client){
	Channel *new_channel = new Channel(channel_name, channel_password, client);
	_channels.push_back(new_channel);
	client->setChannel(new_channel);
	new_channel->setAdmin(client);
	return new_channel;
}

Channel *Server::getChannelByName(std::string &channel_name){
	for (channels_iterator ch_it = _channels.begin(); ch_it != _channels.end(); ch_it++)
		if ((*ch_it)->getName() == channel_name)
			return *ch_it;
	return nullptr;
}

void Server::addClientToChannel(Client *client, Channel *channel) {
	channel->addClient(client);
	client->setChannel(channel);
}

Client *Server::getClientByFd(int client_fd){
	return _clients[client_fd];
}

void Server::privateMessageClient(Client *sender, std::string message) {
	std::string sender_nickname = sender->getNickname();
	size_t space_pos = message.find(' ', 8);
	if (space_pos != std::string::npos) {
		std::string target_nickname = message.substr(8, space_pos - 8);
		std::string private_message = message.substr(space_pos + 1);
		Client *target = nullptr;
		for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
			if (it->second->getNickname() == target_nickname) {
				target = it->second;
				break;
			}
		}
		if (target) {
			std::string formatted_message = "[PM from " + sender_nickname + "]: " + private_message + "\r\n";
			target->sendMessage(formatted_message, target->getFd());
		} else {
			std::string error_message = "Error: Client '" + target_nickname + "' not found.\r\n";
			sender->sendMessage(error_message, sender->getFd());
		}
	}
}

void Server::joinChannel(Client *sender, std::string message) {
	std::string channel_name = message.substr(6);
	Channel *channel = getChannelByName(channel_name);
	if (channel) {
		addClientToChannel(sender, channel);
		std::string join_message = "You have joined the channel: " + channel_name + "\r\n";
		sender->sendMessage(join_message, sender->getFd());
	} else {
		// Create a new channel if it doesn't exist
		std::string channel_password = ""; // modify this to handle passwords
		channel = createNewChannel(channel_name, channel_password, sender);
		std::string create_message = "Channel created: " + channel_name + "\r\n";
		sender->sendMessage(create_message, sender->getFd());
		std::string join_message = "You have joined the channel: " + channel_name + "\r\n";
		sender->sendMessage(join_message, sender->getFd());
	}
}

void Server::setNickname(Client *client, std::string message) {
	std::string new_nickname = message.substr(5);
	if (!new_nickname.empty() && new_nickname.back() == '\n') {
		new_nickname.pop_back();
	}
	if (!new_nickname.empty() && new_nickname.back() == '\r') {
		new_nickname.pop_back();
	}
	bool nickname_exists = false;
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getNickname() == new_nickname) {
			nickname_exists = true;
			break;
		}
	}
	if (nickname_exists) {
		std::string error_message = "Error: Nickname '" + new_nickname + "' already exists. Please choose a different nickname.\r\n";
		client->sendMessage(error_message, client->getFd());
	} else {
		client->setNickname(new_nickname);
		std::string success_message = "Nickname changed to: " + client->getNickname() + "\r\n";
		client->sendMessage(success_message, client->getFd());
	}
}

void Server::setUsername(Client *client, std::string message) {
	std::string new_username = message.substr(5);
	if (!new_username.empty() && new_username.back() == '\n') {
		new_username.pop_back();
	}
	if (!new_username.empty() && new_username.back() == '\r') {
		new_username.pop_back();
	}
	bool username_exists = false;
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getUsername() == new_username) {
			username_exists = true;
			break;
		}
	}
	if (username_exists) {
		std::string error_message = "Error: Username '" + new_username + "' already exists. Please choose a different username.\r\n";
		client->sendMessage(error_message, client->getFd());
	} else {
		client->setUsername(new_username);
		std::string success_message = "Username set to: " + client->getUsername() + "\r\n";
		client->sendMessage(success_message, client->getFd());
	}
}

void Server::makeOperator(Client *client, std::string message) {
	// Debug: Print the raw OPER command
	std::cout << "Debug: Raw OPER command: '" << message << "'" << std::endl;

	// Remove trailing newline and carriage return characters
	if (!message.empty() && message.back() == '\n') {
		message.pop_back();
	}
	if (!message.empty() && message.back() == '\r') {
		message.pop_back();
	}

	// Debug: Print the cleaned OPER command
	std::cout << "Debug: Cleaned OPER command: '" << message << "'" << std::endl;

	// Find the positions of the spaces
	size_t first_space = message.find(' '); // Space after "OPER"
	size_t second_space = message.find(' ', first_space + 1); // Space between username and password

	// Debug: Print the positions of the spaces
	std::cout << "Debug: first_space: " << first_space << ", second_space: " << second_space << std::endl;

	if (first_space != std::string::npos && second_space != std::string::npos) {
		// Extract the username and password
		std::string username = message.substr(first_space + 1, second_space - (first_space + 1));
		std::string password = message.substr(second_space + 1);

		// Debug: Print the extracted username and password
		std::cout << "Debug: Username: '" << username << "', Password: '" << password << "'" << std::endl;

		// Check if the password matches the server's operator password
		if (password == _pswrd) {
			// Set the client as an operator
			client->set_IsOperator(true);

			// Debug: Verify that the operator status is set
			std::cout << "Debug: Client " << client->getNickname() << " is now an operator." << std::endl;

			// Send a success message to the client
			std::string success_message = "You are now an operator.\r\n";
			client->sendMessage(success_message, client->getFd());
		} else {
			// Send an error message to the client
			std::string error_message = "Error: Invalid operator password.\r\n";
			client->sendMessage(error_message, client->getFd());
		}
	} else {
		// Handle invalid OPER command format
		std::string error_message = "Error: Invalid OPER command. Usage: OPER <username> <password>\r\n";
		client->sendMessage(error_message, client->getFd());
	}
}
