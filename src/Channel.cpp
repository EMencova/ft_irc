/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 09:09:39 by mac               #+#    #+#             */
/*   Updated: 2025/03/10 16:02:23 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Channel.hpp"
#include "../inc/response.hpp"

Channel::Channel() {
}

Channel::Channel(std::string &name, std::string &password, Client *client) {
	_name = name;
	_password = password;
	_clients.push_back(client);
	_admin = client;
	_operators.push_back(client);
	if (!password.empty())
		_modes.insert('k');
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
	_admin = original._admin;
	_modes = original._modes;
	_topic = original._topic;
	_operators = original._operators;
	_invitedClients = original._invitedClients;
	return *this;
}

void Channel::addClient(Client *client) {
	_clients.push_back(client);
	client->setChannel(this);
	irc_log("Client " + client->getNickname() + " joined channel " + _name);
}

void Channel::removeClient(Client *client) {
	std::vector<Client *>::iterator it = std::find(_clients.begin(), _clients.end(), client);
	if (it != _clients.end()) {
		_clients.erase(it);
		client->setChannel(NULL);
		irc_log("Client " + client->getNickname() + " left channel " + _name);
	}
	removeOperator(client);
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
	if (!password.empty())
		_modes.insert('k');
	else
		_modes.erase('k');
}

// :<sender_nick>!<sender_ident>@<sender_host> PRIVMSG <channel> :<message>\r\n

void Channel::sendMessageToClients(const std::string message, Client *sender) {
	// Get sender's information
	std::string sender_nick = sender->getNickname();
	std::string sender_ident = sender->getUsername();
	std::string sender_host = sender->getHost();

	if (isOperator(sender)) {
		sender_nick = "@" + sender_nick;
	}

	// IRC-formatted message:
	// :<sender_nick>!<sender_ident>@<sender_host> PRIVMSG <channel> :<message>\r\n
	std::string formatted_message = ":" + sender_nick + "!" + sender_ident + "@" + sender_host +
		" PRIVMSG " + this->getName() + " :" + message + "\r\n";

	for (clients_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (*it != sender) {
			(*it)->sendMessage(formatted_message, (*it)->getFd());
		}
	}
}

void Channel::setAdmin(Client *admin) {
	_admin = admin;
}

Client *Channel::getAdmin() {
	return _admin;
}

// Channel modes

void Channel::addMode(char mode) {
	_modes.insert(mode);
}

void Channel::removeMode(char mode) {
	_modes.erase(mode);
}

bool Channel::hasMode(char mode) const {
	return _modes.find(mode) != _modes.end();
}

std::string Channel::getModes() const {
	std::string modes;
	for (std::set<char>::iterator it = _modes.begin(); it != _modes.end(); ++it)
		modes.push_back(*it);
	return modes;
}

void Channel::setTopic(const std::string &topic) {
	_topic = topic;
}

std::string Channel::getTopic() const {
	return _topic;
}

void Channel::addOperator(Client *client) {
	if (!isOperator(client))
		_operators.push_back(client);
}

void Channel::removeOperator(Client *client) {
	std::vector<Client*>::iterator it = std::find(_operators.begin(), _operators.end(), client);
	if (it != _operators.end())
		_operators.erase(it);
}

bool Channel::isOperator(Client *client) const {
	if (client == _admin)
		return true;
	for (std::vector<Client*>::const_iterator it = _operators.begin(); it != _operators.end(); ++it) {
		if (*it == client)
			return true;
	}
	return false;
}

void Channel::addInvited(Client *client) {
	if (!isInvited(client))
		_invitedClients.push_back(client);
}

bool Channel::isInvited(Client *client) const {
	for (std::vector<Client*>::const_iterator it = _invitedClients.begin(); it != _invitedClients.end(); ++it) {
		if (*it == client)
			return true;
	}
	return false;
}

void Channel::removeInvited(Client *client) {
	std::vector<Client*>::iterator it = std::find(_invitedClients.begin(), _invitedClients.end(), client);
	if (it != _invitedClients.end())
		_invitedClients.erase(it);
}
