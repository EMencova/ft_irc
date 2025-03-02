/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 09:09:39 by mac               #+#    #+#             */
/*   Updated: 2025/03/02 16:00:40 by mac              ###   ########.fr       */
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
	_clients.push_back(client);
	_admin = client;
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
	_clients.erase(std::find(_clients.begin(), _clients.end(), client));
}

void Channel::addClient(Client *client) {
	_clients.push_back(client);
}

std::vector<Client *> Channel::getClients(){
	return _clients;
}

std::string Channel::getName(){
	return _name;
}

std::string Channel::getpassword() {
	return _password;
}

void Channel::setName(std::string name) {
	_name = name;
}

void Channel::setPassword(std::string password) {
	_password = password;
}

void Channel::sendMessageToClients(std::string message, Client *client){
	for (clients_iterator it = _clients.begin(); it != _clients.end(); it++) {
		if (*it == client)
			continue;
		(*it)->sendMessage(message, (*it)->getFd());
	}
}

void Channel::setAdmin(Client *admin) {
	_admin = admin;
}

Client *Channel::getAdmin() {
	return _admin;
}
