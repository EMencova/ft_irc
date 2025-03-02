/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 08:47:29 by mac               #+#    #+#             */
/*   Updated: 2025/03/02 15:06:47 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"


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

//getters

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

//setters
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
	std::string message = "Welcome to the server " + host + " on port " + port + "\r\n";
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


