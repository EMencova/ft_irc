/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashobajo <ashobajo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/09 10:04:13 by ashobajo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"

void Server::handleInvite(Client *sender, std::string message) {
	// Format: INVITE <channel> <user>
	std::istringstream iss(message);
	std::string command, channel_name, target_nickname;
	iss >> command >> channel_name >> target_nickname;
	if (channel_name.empty() || target_nickname.empty()) {
		sender->sendMessage("Error: Invalid INVITE command format. Usage: INVITE <channel> <user>\r\n", sender->getFd());
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
	Client *target = NULL;
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getNickname() == target_nickname) {
			target = it->second;
			break;
		}
	}
	if (!target) {
		sender->sendMessage(ERR_NOSUCHNICK("server", target_nickname) + "\r\n", sender->getFd());
		return;
	}
	channel->addInvited(target);
	std::string invite_message = "You have been invited to channel " + channel_name +
		" by " + sender->getNickname() + ".\r\n";
	target->sendMessage(invite_message, target->getFd());
	sender->sendMessage("Invitation sent.\r\n", sender->getFd());
}
