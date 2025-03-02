/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 08:47:29 by mac               #+#    #+#             */
/*   Updated: 2025/03/02 12:44:03 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"

// class Client {
// 	private:
// 		int							_fd;
// 		std::string					_host;
// 		std::string					_port;
// 		std::string					_username;
// 		std::string					_nickname;
// 		Channel						*_channel;
// 		bool						_registered;

// 	public:
// 		Client ();
// 		Client(int fd, std::string port, std::string host);
// 		~Client();

// 		//getters
// 		int getFd();
// 		std::string getHost();
// 		std::string getPort();
// 		std::string getUsername();
// 		std::string getNickname();
// 		Channel *getChannel();
// 		bool getRegistered();

// 		//setters
// 		void setFd(int fd);
// 		void setHost(std::string host);
// 		void setPort(std::string port);
// 		void setUsername(std::string username);
// 		void setNickname(std::string nickname);
// 		void setChannel(Channel *channel);
// 		void setRegistered(bool registered);

// 		//methods
// 		//write a message, reply a mesaage to an fd, welcome message, join, leave, list, nick, user, who, whois, quit

// 		void welcomeMessage(std::string host, std::string port);
// 		void sendMessage(std::string message, int fd);
// 		void readMessage(int fd);
// 		void joinChannel(Channel *channel);
// 		void leaveChannel();
// };

// #endif


Client::Client() {
}

Client::Client(int fd, std::string port, std::string host) {
	_fd = fd;
	_port = port;
	_host = host;
	_registered = false;
}

Client::~Client() {
}

int Client::getFd() {
	return _fd;
}

std::string Client::getHost() {
	return _host;
}

std::string Client::getPort() {
	return _port;
}

std::string Client::getUsername() {
	return _username;
}

std::string Client::getNickname() {
	return _nickname;
}

Channel *Client::getChannel() {
	return _channel;
}

bool Client::getRegistered() {
	return _registered;
}

void Client::setFd(int fd) {
	_fd = fd;
}

void Client::setHost(std::string host) {
	_host = host;
}

void Client::setPort(std::string port) {
	_port = port;
}

void Client::setUsername(std::string username) {
	_username = username;
}

void Client::setNickname(std::string nickname) {
	_nickname = nickname;
}

void Client::setChannel(Channel *channel) {
	_channel = channel;
}

void Client::setRegistered(bool registered) {
	_registered = registered;
}

void Client::welcomeMessage(std::string host, std::string port) {
	std::string message = "Welcome to the server " + host + " on port " + port + "\n";
	send(_fd, message.c_str(), message.size(), 0);
}

void Client::sendMessage(std::string message, int fd) {
	send(fd, message.c_str(), message.size(), 0);
}

void Client::readMessage(int fd) {
	std::string message;
	char BUFFER[1024];
	bzero(BUFFER, 1024);

	while (!std::strstr(BUFFER, "\r\n")) {
		bzero(BUFFER, 1024);
		recv(fd, BUFFER, 1024, 0);
		message += BUFFER;
	}
	std::cout << message << std::endl;
}

void Client::joinChannel(Channel *channel) {
	_channel = channel;
}

void Client::leaveChannel() {
	_channel = nullptr;
}


