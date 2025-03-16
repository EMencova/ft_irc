/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashobajo <ashobajo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/09 10:04:33 by ashobajo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"

void Server::handleKick(Client *sender, std::string message) {
	// Format: KICK <channel> <user> [<comment>]
	std::istringstream iss(message);
	std::string command, channel_name, target_nickname;
	iss >> command >> channel_name >> target_nickname;
	if (channel_name.empty() || target_nickname.empty()) {
		sender->sendMessage("Error: Invalid KICK command format. Usage: KICK <channel> <user>\r\n", sender->getFd());
		return;
	}
	Channel *channel = getChannelByName(channel_name);
	if (!channel) {
		sender->sendMessage(ERR_NOSUCHCHANNEL("server", channel_name) + "\r\n", sender->getFd());
		return;
	}
	if (!channel->isOperator(sender)) {
		sender->sendMessage(ERR_CHANOPRIVSNEEDED("server", channel_name) + "\r\n", sender->getFd());
		return;
	}
	std::vector<Client *> clients = channel->getClients();
	Client *target = NULL;
	for (size_t i = 0; i < clients.size(); i++) {
		if (clients[i]->getNickname() == target_nickname) {
			target = clients[i];
			break;
		}
	}
	if (!target) {
		sender->sendMessage("Error: Client " + target_nickname + " is not in channel " + channel_name + ".\r\n", sender->getFd());
		return;
	}
	channel->removeClient(target);
	std::string kick_message = "You have been kicked from channel " + channel_name +
		" by " + sender->getNickname() + ".\r\n";
	target->sendMessage(kick_message, target->getFd());
	std::string notify_message = target_nickname + " has been kicked from the channel by " + sender->getNickname() + ".\r\n";
	channel->sendMessageToClients(notify_message, sender);
}
