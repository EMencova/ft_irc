/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 09:09:39 by mac               #+#    #+#             */
/*   Updated: 2025/03/05 06:39:11 by mac              ###   ########.fr       */
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

void Channel::addClient(Client *client) {
	_clients.push_back(client);
	client->setChannel(this);
	std::cout << "Client " << client->getNickname() << " joined channel " << _name << std::endl;
}

void Channel::removeClient(Client *client) {
	std::vector<Client *>::iterator it = std::find(_clients.begin(), _clients.end(), client);
	if (it != _clients.end()) {
		_clients.erase(it);
		client->setChannel(nullptr);
		std::cout << "Client " << client->getNickname() << " left channel " << _name << std::endl;
	}
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

//confirm
void Channel::sendMessageToClients(const std::string message, Client *sender) {
	for (clients_iterator it = _clients.begin(); it != _clients.end(); it++) {
		if (*it != sender) {
			(*it)->sendMessage(message, (*it)->getFd());
		}
	}
}

void Channel::setAdmin(Client *admin) {
	_admin = admin;
}

Client *Channel::getAdmin() {
	return _admin;
}

// Mode-related methods
void Channel::setInviteOnly(bool inviteOnly) {
    _inviteOnly = inviteOnly;
}

bool Channel::isInviteOnly() const {
    return _inviteOnly;
}

void Channel::setTopicRestricted(bool topicRestricted) {
    _topicRestricted = topicRestricted;
}

bool Channel::isTopicRestricted() const {
    return _topicRestricted;
}

void Channel::setTopic(const std::string &topic) {
    _topic = topic;
}

std::string Channel::getTopic() const {
    return _topic;
}

void Channel::setChannelKey(const std::string &key) {
    _channelKey = key;
}

std::string Channel::getChannelKey() const {
    return _channelKey;
}

void Channel::addOperator(Client *client) {
    if (!isOperator(client)) {
        _operators.push_back(client);
    }
}

void Channel::removeOperator(Client *client) {
    _operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
}

bool Channel::isOperator(Client *client) const {
    return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}

// Invite-related methods
void Channel::addInvitedClient(Client *client) {
    if (!isClientInvited(client)) {
        _invitedClients.push_back(client);
    }
}

bool Channel::isClientInvited(Client *client) const {
    return std::find(_invitedClients.begin(), _invitedClients.end(), client) != _invitedClients.end();
}
