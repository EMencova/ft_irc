/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 08:47:29 by mac               #+#    #+#             */
/*   Updated: 2025/03/15 07:18:33 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"
#include "../inc/response.hpp"

Client::Client() { }

Client::Client(int fd, std::string port, std::string host)
: _fd(fd), _host(host), _port(port), _channel(NULL), _registered(false), _is_operator(false)
{ }

Client::~Client() { }

int Client::getFd() {
	return _fd;
}

// Getters

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

bool Client::get_IsOperator() {
	return _is_operator;
}

// Setters

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

void Client::set_IsOperator(bool is_operator) {
	_is_operator = is_operator;
}

void Client::welcomeMessage(std::string host, std::string port) {
	std::string message = "Welcome to the server " + host + " on port " + port + "\r\n";
	send(_fd, message.c_str(), message.size(), 0);
}

void Client::sendMessage(std::string message, int fd) {
	send(fd, message.c_str(), message.size(), 0);
}

void Client::readMessage(int fd) {
	char tempBuffer[1024];
	ssize_t bytes_read = recv(fd, tempBuffer, sizeof(tempBuffer) - 1, 0);
	if (bytes_read <= 0) {
		return;
	}
	tempBuffer[bytes_read] = '\0';
	_partialMessage += tempBuffer;

	size_t pos;
	while ((pos = _partialMessage.find("\r\n")) != std::string::npos) {
		std::string completeMsg = _partialMessage.substr(0, pos);
		irc_log("Received: " + completeMsg);
		_partialMessage.erase(0, pos + 2);
	}
}

void Client::joinChannel(Channel *channel) {
	_channel = channel;
}

void Client::leaveChannel() {
	_channel = NULL;
}

void Client::appendToBuffer(const std::string &data) {
	_partialMessage += data;
}

std::string &Client::getBuffer() {
	return _partialMessage;
}
