/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/05 11:47:21 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"

void Server::joinChannel(Client *sender, std::string message) {
	// Expected format: JOIN <channel> [<key>]
	std::istringstream iss(message);
	std::string command, channel_name, provided_key;
	iss >> command >> channel_name >> provided_key;

	Channel *channel = getChannelByName(channel_name);
	if (channel) {
		if (channel->hasMode('i') && !channel->isInvited(sender)) {
			std::string error_message = "Error: Channel " + channel_name +
				" is invite-only. You must be invited to join.\r\n";
			sender->sendMessage(error_message, sender->getFd());
			return;
		}
		if (channel->hasMode('k')) {
			if (provided_key.empty() || provided_key != channel->getpassword()) {
				std::string error_message = ERR_BADCHANNELKEY("server", channel_name) + "\r\n";
				sender->sendMessage(error_message, sender->getFd());
				return;
			}
		}
		addClientToChannel(sender, channel);
		if (channel->isInvited(sender))
			channel->removeInvited(sender);
		std::string join_message = "You have joined the channel: " + channel_name + "\r\n";
		sender->sendMessage(join_message, sender->getFd());
	} else {
		std::string channel_password = provided_key;
		channel = createNewChannel(channel_name, channel_password, sender);
		std::string create_message = "Channel created: " + channel_name + "\r\n";
		sender->sendMessage(create_message, sender->getFd());
		std::string join_message = "You have joined the channel: " + channel_name + "\r\n";
		sender->sendMessage(join_message, sender->getFd());
	}
}
