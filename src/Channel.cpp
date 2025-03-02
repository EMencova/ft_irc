/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 09:09:39 by mac               #+#    #+#             */
/*   Updated: 2025/03/02 11:22:00 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Channel.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>

Channel::Channel() {
}

Channel::Channel(std::string &name, std::string &password, Client *client) {
	_name = name;
	_password = password;
	_clients.insert(client);
}

Channel::~Channel() {
}


Channel::Channel(const Channel &original) {
	*this = original;
}

Channel &Channel::operator=(const Channel &original) {
	if (this == &original)
	return *this;
	_name = original._name;
	_password = original._password;
	_clients = original._clients;
	return *this;
}

void Channel::removeClient(Client *client) {
	_clients.erase(client);
}

void Channel::addClient(Client *client) {
	_clients.insert(client);
}

std::set<Client *> Channel::getClients() {
	return _clients;
}

std::string Channel::getName() {
	return _name;
}

std::string Channel::getPassword() {
	return _password;
}

void Channel::setName(std::string &name) {
	_name = name;
}

void Channel::setPassword(std::string &password) {
	_password = password;
}

void Channel::sendMessageToClients(Client *client, std::string &message) {
	for (Client *c : _clients) {
		if (c == client)
			continue;
		send(c->getFd(), message.c_str(), message.size(), 0);
	}
}

void Channel::sendMessageToClients(std::string &message) {
	for (Client *c : _clients) {
		send(c->getFd(), message.c_str(), message.size(), 0);
	}
}

