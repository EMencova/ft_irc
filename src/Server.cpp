/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/15 07:43:42 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include "../inc/response.hpp"
#include <sstream>
#include <cstring>
#include <cerrno>

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
	server_addr.sin_port = htons((unsigned short)atoi(_port.c_str()));

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
	std::string& clientBuffer = client->getBuffer();
	size_t newlinePos = clientBuffer.find("\n\r");
	clientBuffer.erase(0, newlinePos + 1);

	while (true) {
		bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_read > 0) {
			buffer[bytes_read] = '\0';
			client->appendToBuffer(buffer);
			message = (client->getBuffer());
			if (message.find("\n") != std::string::npos || message.find("\n\r") != std::string::npos) {
				client->getBuffer().clear();
				break;
			}

// 			bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
// 			if (bytes_read > 0) {
// 				buffer[bytes_read] = '\0';
// 				message.append(buffer);
// 				if (message.find("\r\n") != std::string::npos) {
// 					break;
// 				}


		} else if (bytes_read == 0) {
			if (!client->getNickname().empty()) {
				std::ostringstream oss;
				oss << "Client " << client->getNickname() << " disconnected.";
				irc_log(oss.str());
			} else {
				std::ostringstream oss;
				oss << "Client FD " << client_fd << " disconnected.";
				irc_log(oss.str());
			}
			thisClientDisconnect(client_fd);
			return "";
		} else {
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				break;
			else {
				std::ostringstream oss;
				oss << "recv() failed: " << strerror(errno);
				irc_log(oss.str());
				thisClientDisconnect(client_fd);
				return "";
			}
		}
	}

	size_t first = message.find_first_not_of(" \t\r\n");
	if (first != std::string::npos)
		message = message.substr(first);
	else
		message = "";

	size_t last = message.find_last_not_of(" \t\r\n");
	if (last != std::string::npos)
		message = message.substr(0, last + 1);

	// Log the message only if non-empty after trimming.
	if (!message.empty()) {
		std::ostringstream oss;
		if (!client->getNickname().empty()) {
			oss << "Message from " << client->getNickname() << ": " << message;
		} else {
			oss << "Message from FD " << client_fd << ": " << message;
		}
		irc_log(oss.str());
	}
	return message;
}

void Server::thisClientConnect() {
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(_pollfds.begin()->fd, (struct sockaddr *)&client_addr, &client_addr_len);

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

	if (fcntl(client_fd, F_SETFL, client_fds_flags | O_NONBLOCK) < 0) {
		irc_log("set client flags failed");
		close(client_fd);
		return;
	}

	if (_pollfds.size() < MAXCLIENT) {
		pollfd client_fd_poll;
		client_fd_poll.fd = client_fd;
		client_fd_poll.events = POLLIN;
		client_fd_poll.revents = 0;
		_pollfds.push_back(client_fd_poll);

		char hostname[1024];
		if (getnameinfo((struct sockaddr *)&client_addr, client_addr_len, hostname, 1024, NULL, 0, 0) == 0)
			irc_log("New connection from: " + std::string(hostname));
		else
			irc_log("New connection from unknown host");

		// ntohs Convert the client's port from network to host order and then to a string.
		unsigned short portHost = ntohs(client_addr.sin_port);
		std::ostringstream ossPort;
		ossPort << portHost;
		std::string clientPort = ossPort.str();

		Client *new_client = new Client(client_fd, clientPort, std::string(hostname));

		authenticateClient(new_client);

		_clients[client_fd] = new_client;

		// We Set temporary nickname as "user" followed by the file descriptor.
		std::ostringstream ossFd;
		ossFd << "user" << client_fd;
		new_client->setNickname(ossFd.str());
		_clients[client_fd] = new_client;
		std::ostringstream ossLog;
		ossLog << "New client connected: FD " << client_fd << ", Nickname: " << new_client->getNickname();
		irc_log(ossLog.str());
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

	std::ostringstream oss;
	oss << "Client FD " << client_fd << " disconnected.";
	irc_log(oss.str());
}


// /server add -auto -tls_pass 12345 localhost 9984
//****** this CONNECT command must be capital letter***
// /CONNECT localhost 9984 12345
// /MSG real Hello
// /JOIN #ch1
// /MSG User7 helloy
// /MSG #ch1 Message
// /quit (IRC client leaves)

// If you create a join the channel with #ch irssi would be able to
// message everyone in the ch with /MSG #ch1 Message

void Server::thisClientMessage(int client_fd, Client *sender) {
	std::string message = readMessage(client_fd, sender);
	if (message.empty())
		return;
	sender = _clients[client_fd];

	size_t first = message.find_first_not_of(" \t\n\r");
	if (first != std::string::npos)
		message = message.substr(first);
	size_t last = message.find_last_not_of(" \t\n\r");
	if (last != std::string::npos)
		message = message.substr(0, last + 1);

	// Pre-registration handling.
	if (!sender->getRegistered()) {
		if (message.compare(0, 3, "CAP") == 0) {
			sender->sendMessage("CAP * LS :\r\n", client_fd);
			return;
		}
		if (message.find("PASS ") == 0) {
			std::string password = message.substr(5);
			while (!password.empty() && (password[password.size()-1] == '\n' || password[password.size()-1] == '\r')) {
				password.erase(password.size()-1, 1);
			}
			if (password == _pswrd) {
				sender->setRegistered(true);
				std::string welcome_message = RPL_WELCOME("client") + "\r\n";
				sender->sendMessage(welcome_message, client_fd);
				return;
			} else {
				std::string error_message = ERR_PASSWDMISMATCH("server") + "\r\n";
				sender->sendMessage(error_message, client_fd);
				// thisClientDisconnect(client_fd);
				return;
			}
		}
		else if (message.find("NICK ") == 0) {
			setNickname(sender, message);
			return;
		} else if (message.find("USER ") == 0) {
			setUsername(sender, message);
			return;
		} else {
			std::string error_message = ERR_NOTREGISTERED("server") + "\r\n";
			sender->sendMessage(error_message, client_fd);
			return;
		}
	}

	// For registered clients, handle PING commands.
	if (message.find("PING ") == 0) {
		std::string token = message.substr(5);
		while (!token.empty() && (token[token.size()-1] == '\n' || token[token.size()-1] == '\r')) {
			token.erase(token.size()-1, 1);
		}
		std::string pong_response = "PONG :" + token + "\r\n";
		sender->sendMessage(pong_response, client_fd);
		return;
	}

	// Process other commands for registered clients.
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
		// If the client is in a channel, broadcast the message.
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
	return NULL;
}

void Server::addClientToChannel(Client *client, Channel *channel) {
	channel->addClient(client);
	client->setChannel(channel);
}

Client *Server::getClientByFd(int client_fd) {
	return _clients[client_fd];
}

void Server::closeServer() {
	irc_log("\033[0;31mClosing server... with ❤️  from us\033[0m");
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		int client_fd = it->first;
		Client *client = it->second;
		if (client->getChannel()) {
			client->getChannel()->removeClient(client);
		}
		close(client_fd);
		delete client;
	}
	_clients.clear();
	for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
		close(it->fd);
	}
	_pollfds.clear();
	close(_socket);

	// Free all channels.
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		delete *it;
	}
	_channels.clear();
	_running = 0;
}

