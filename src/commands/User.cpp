/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/10 05:45:17 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"

void Server::setUsername(Client *client, std::string message) {
	std::string new_username = message.substr(5);
	while (!new_username.empty() &&
			(new_username[new_username.size() - 1] == '\n' ||
			new_username[new_username.size() - 1] == '\r')) {
		new_username.erase(new_username.size() - 1, 1);
	}
	bool username_exists = false;
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getUsername() == new_username) {
			username_exists = true;
			break;
		}
	}
	if (username_exists) {
		std::string error_message = "Error: Username '" + new_username +
			"' already exists. Please choose a different username.\r\n";
		client->sendMessage(error_message, client->getFd());
	} else {
		client->setUsername(new_username);
		std::string success_message = "Username set to: " + client->getUsername() + "\r\n";
		client->sendMessage(success_message, client->getFd());
	}
}
