/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/05 11:47:51 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"

void Server::privateMessageClient(Client *sender, std::string message) {
	std::string sender_nickname = sender->getNickname();
	size_t space_pos = message.find(' ', 8);
	if (space_pos != std::string::npos) {
		std::string target_nickname = message.substr(8, space_pos - 8);
		std::string private_message = message.substr(space_pos + 1);
		Client *target = nullptr;
		for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
			if (it->second->getNickname() == target_nickname) {
				target = it->second;
				break;
			}
		}
		if (target) {
			std::string formatted_message = "[PM from " + sender_nickname + "]: " + private_message + "\r\n";
			target->sendMessage(formatted_message, target->getFd());
		} else {
			std::string error_message = ERR_NOSUCHNICK("server", target_nickname) + "\r\n";
			sender->sendMessage(error_message, sender->getFd());
		}
	}
}
