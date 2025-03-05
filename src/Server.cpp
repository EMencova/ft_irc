/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/05 07:39:28 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include <sstream>


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
	std::cout << "Client FD " << client_fd << " disconnected." << std::endl;
}


void Server::thisClientMessage(int client_fd, Client *sender) {
	sender = _clients[client_fd];
	std::string message = readMessage(client_fd, sender);
	if (message.empty())
		return;

	if (!sender->getRegistered()) {
		if (message.find("PASS ") != 0) {
			sender->sendMessage("Error: You must authenticate using the PASS command.\r\n", sender->getFd());
			thisClientDisconnect(client_fd);
			return;
		}
		return;
	}

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


void Server::authenticateClient(Client *client) {
	int client_fd = client->getFd();

	std::string welcome_message = "Welcome to the server. Please authenticate using the PASS command.\r\n";
	send(client_fd, welcome_message.c_str(), welcome_message.size(), 0);

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

				// Debug received password - to remove later
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
					thisClientDisconnect(client_fd);
					return;
				}
			} else {
				std::string error_message = "Error: You must authenticate using the PASS command.\r\n";
				send(client_fd, error_message.c_str(), error_message.size(), 0);
				thisClientDisconnect(client_fd);
				return;
			}
		}
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
	// Expected format: JOIN <channel> [<key>]
	std::istringstream iss(message);
	std::string command, channel_name, provided_key;
	iss >> command >> channel_name >> provided_key;

	Channel *channel = getChannelByName(channel_name);
	if (channel) {
		if (channel->hasMode('i') && !channel->isInvited(sender)) {
			std::string error_message = "Error: Channel " + channel_name + " is invite-only. You must be invited to join.\r\n";
			sender->sendMessage(error_message, sender->getFd());
			return;
		}
		if (channel->hasMode('k')) {
			if (provided_key.empty() || provided_key != channel->getpassword()) {
				std::string error_message = "Error: Incorrect or missing key for channel " + channel_name + ".\r\n";
				sender->sendMessage(error_message, sender->getFd());
				return;
			}
		}
		addClientToChannel(sender, channel);
		if (channel->isInvited(sender))
			channel->removeInvited(sender);
		std::string join_message = "You have joined the channel: " + channel_name + "\r\n";
		sender->sendMessage(join_message, sender->getFd());
	} else {
		std::string channel_password = provided_key;
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
	if (!message.empty() && message.back() == '\n') {
		message.pop_back();
	}
	if (!message.empty() && message.back() == '\r') {
		message.pop_back();
	}
	size_t first_space = message.find(' ');
	size_t second_space = message.find(' ', first_space + 1);
	std::cout << "Debug: first_space: " << first_space << ", second_space: " << second_space << std::endl;

	if (first_space != std::string::npos && second_space != std::string::npos) {
		std::string username = message.substr(first_space + 1, second_space - (first_space + 1));
		std::string password = message.substr(second_space + 1);
		std::cout << "Debug: Username: '" << username << "', Password: '" << password << "'" << std::endl;
		if (password == _pswrd) {
			client->set_IsOperator(true);
			std::cout << "Debug: Client " << client->getNickname() << " is now an operator." << std::endl;
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

void Server::handleKick(Client *sender, std::string message) {
	// Format: KICK <channel> <user> [<comment>]
	std::istringstream iss(message);
	std::string command, channel_name, target_nickname;
	iss >> command >> channel_name >> target_nickname;
	if (channel_name.empty() || target_nickname.empty()) {
		sender->sendMessage("Error: Invalid KICK command format. Usage: KICK <channel> <user>\r\n", sender->getFd());
		return;
	}
	Channel *channel = getChannelByName(channel_name);
	if (!channel) {
		sender->sendMessage("Error: Channel " + channel_name + " does not exist.\r\n", sender->getFd());
		return;
	}
	if (!channel->isOperator(sender)) {
		sender->sendMessage("Error: You are not a channel operator in " + channel_name + ".\r\n", sender->getFd());
		return;
	}
	std::vector<Client *> clients = channel->getClients();
	Client *target = nullptr;
	for (size_t i = 0; i < clients.size(); i++) {
		if (clients[i]->getNickname() == target_nickname) {
			target = clients[i];
			break;
		}
	}
	if (!target) {
		sender->sendMessage("Error: Client " + target_nickname + " is not in channel " + channel_name + ".\r\n", sender->getFd());
		return;
	}
	channel->removeClient(target);
	std::string kick_message = "You have been kicked from channel " + channel_name + " by " + sender->getNickname() + ".\r\n";
	target->sendMessage(kick_message, target->getFd());
	std::string notify_message = target_nickname + " has been kicked from the channel by " + sender->getNickname() + ".\r\n";
	channel->sendMessageToClients(notify_message, sender);
}

void Server::handleInvite(Client *sender, std::string message) {
	// Format: INVITE <channel> <user>
	std::istringstream iss(message);
	std::string command, channel_name, target_nickname;
	iss >> command >> channel_name >> target_nickname;
	if (channel_name.empty() || target_nickname.empty()) {
		sender->sendMessage("Error: Invalid INVITE command format. Usage: INVITE <channel> <user>\r\n", sender->getFd());
		return;
	}
	Channel *channel = getChannelByName(channel_name);
	if (!channel) {
		sender->sendMessage("Error: Channel " + channel_name + " does not exist.\r\n", sender->getFd());
		return;
	}
	if (!channel->isOperator(sender)) {
		sender->sendMessage("Error: You are not a channel operator in " + channel_name + ".\r\n", sender->getFd());
		return;
	}
	Client *target = nullptr;
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getNickname() == target_nickname) {
			target = it->second;
			break;
		}
	}
	if (!target) {
		sender->sendMessage("Error: Client " + target_nickname + " not found.\r\n", sender->getFd());
		return;
	}
	channel->addInvited(target);
	std::string invite_message = "You have been invited to channel " + channel_name + " by " + sender->getNickname() + ".\r\n";
	target->sendMessage(invite_message, target->getFd());
	sender->sendMessage("Invitation sent.\r\n", sender->getFd());
}

void Server::handleTopic(Client *sender, std::string message) {
	// Format: TOPIC <channel> [<new topic>]
	std::istringstream iss(message);
	std::string command, channel_name;
	iss >> command >> channel_name;
	if (channel_name.empty()) {
		sender->sendMessage("Error: Invalid TOPIC command format. Usage: TOPIC <channel> [<new topic>]\r\n", sender->getFd());
		return;
	}
	Channel *channel = getChannelByName(channel_name);
	if (!channel) {
		sender->sendMessage("Error: Channel " + channel_name + " does not exist.\r\n", sender->getFd());
		return;
	}
	std::string new_topic;
	std::getline(iss, new_topic);
	size_t start = new_topic.find_first_not_of(" ");
	if (start != std::string::npos)
		new_topic = new_topic.substr(start);
	else
		new_topic = "";

	if (new_topic.empty()) {
		std::string current_topic = channel->getTopic();
		if (current_topic.empty())
			sender->sendMessage("No topic is set for channel " + channel_name + ".\r\n", sender->getFd());
		else
			sender->sendMessage("Current topic for " + channel_name + ": " + current_topic + "\r\n", sender->getFd());
	} else {
		if (channel->hasMode('t') && !channel->isOperator(sender)) {
			sender->sendMessage("Error: Only channel operators can change the topic in " + channel_name + ".\r\n", sender->getFd());
			return;
		}
		channel->setTopic(new_topic);
		std::string topic_message = "Channel " + channel_name + " topic changed to: " + new_topic + "\r\n";
		channel->sendMessageToClients(topic_message, sender);
		sender->sendMessage("Topic updated.\r\n", sender->getFd());
	}
}

void Server::handleMode(Client *sender, std::string message) {
	// Format: MODE <channel> <modes> [<mode parameters>]
	std::istringstream iss(message);
	std::string command, channel_name, mode_changes;
	iss >> command >> channel_name >> mode_changes;
	if (channel_name.empty() || mode_changes.empty()) {
		sender->sendMessage("Error: Invalid MODE command format. Usage: MODE <channel> <modes> [<parameters>]\r\n", sender->getFd());
		return;
	}
	Channel *channel = getChannelByName(channel_name);
	if (!channel) {
		sender->sendMessage("Error: Channel " + channel_name + " does not exist.\r\n", sender->getFd());
		return;
	}
	if (!channel->isOperator(sender)) {
		sender->sendMessage("Error: You are not a channel operator in " + channel_name + ".\r\n", sender->getFd());
		return;
	}
	char sign = mode_changes[0];
	if (sign != '+' && sign != '-') {
		sender->sendMessage("Error: Mode changes must start with '+' or '-'.\r\n", sender->getFd());
		return;
	}
	for (size_t i = 1; i < mode_changes.size(); i++) {
		char mode = mode_changes[i];
		if (sign == '+') {
			if (mode == 'i' || mode == 't') {
				channel->addMode(mode);
				std::string mode_msg = "Mode +";
				mode_msg.push_back(mode);
				mode_msg += " set for channel " + channel_name + ".\r\n";
				sender->sendMessage(mode_msg, sender->getFd());
			} else if (mode == 'k') {
				std::string key;
				iss >> key;
				if (key.empty()) {
					sender->sendMessage("Error: Mode +k requires a key parameter.\r\n", sender->getFd());
				} else {
					channel->setPassword(key);
					channel->addMode('k');
					std::string mode_msg = "Mode +k set for channel " + channel_name + " with key.\r\n";
					sender->sendMessage(mode_msg, sender->getFd());
				}
			} else if (mode == 'o') {
				std::string target_nickname;
				iss >> target_nickname;
				if (target_nickname.empty()) {
					sender->sendMessage("Error: Mode +o requires a nickname parameter.\r\n", sender->getFd());
				} else {
					std::vector<Client *> clients = channel->getClients();
					Client *target = nullptr;
					for (size_t j = 0; j < clients.size(); j++) {
						if (clients[j]->getNickname() == target_nickname) {
							target = clients[j];
							break;
						}
					}
					if (!target) {
						sender->sendMessage("Error: Client " + target_nickname + " is not in channel " + channel_name + ".\r\n", sender->getFd());
					} else {
						channel->addOperator(target);
						std::string mode_msg = "Mode +o: " + target_nickname + " is now a channel operator in " + channel_name + ".\r\n";
						sender->sendMessage(mode_msg, sender->getFd());
						target->sendMessage("You have been granted channel operator privileges in " + channel_name + ".\r\n", target->getFd());
					}
				}
			} else {
				sender->sendMessage("Error: Unknown mode flag.\r\n", sender->getFd());
			}
		} else if (sign == '-') {
			if (mode == 'i' || mode == 't') {
				channel->removeMode(mode);
				std::string mode_msg = "Mode -";
				mode_msg.push_back(mode);
				mode_msg += " removed for channel " + channel_name + ".\r\n";
				sender->sendMessage(mode_msg, sender->getFd());
			} else if (mode == 'k') {
				channel->setPassword("");
				channel->removeMode('k');
				std::string mode_msg = "Mode -k removed for channel " + channel_name + ".\r\n";
				sender->sendMessage(mode_msg, sender->getFd());
			} else if (mode == 'o') {
				std::string target_nickname;
				iss >> target_nickname;
				if (target_nickname.empty()) {
					sender->sendMessage("Error: Mode -o requires a nickname parameter.\r\n", sender->getFd());
				} else {
					std::vector<Client *> clients = channel->getClients();
					Client *target = nullptr;
					for (size_t j = 0; j < clients.size(); j++) {
						if (clients[j]->getNickname() == target_nickname) {
							target = clients[j];
							break;
						}
					}
					if (!target) {
						sender->sendMessage("Error: Client " + target_nickname + " is not in channel " + channel_name + ".\r\n", sender->getFd());
					} else {
						channel->removeOperator(target);
						std::string mode_msg = "Mode -o: " + target_nickname + " is no longer a channel operator in " + channel_name + ".\r\n";
						sender->sendMessage(mode_msg, sender->getFd());
						target->sendMessage("Your channel operator privileges have been removed in " + channel_name + ".\r\n", target->getFd());
					}
				}
			} else {
				sender->sendMessage("Error: Unknown mode flag.\r\n", sender->getFd());
			}
		}
	}
}
