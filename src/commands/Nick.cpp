/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eliskam <eliskam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/05 17:31:45 by eliskam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"

void Server::setNickname(Client *client, std::string message) {
	std::string new_nickname = message.substr(5);
	if (!new_nickname.empty() && new_nickname[new_nickname.size() - 1] == '\n') {
		new_nickname.erase(new_nickname.size() - 1);
	}
	if (!new_nickname.empty() && new_nickname[new_nickname.size() - 1] == '\r') {
		new_nickname.erase(new_nickname.size() - 1);
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
