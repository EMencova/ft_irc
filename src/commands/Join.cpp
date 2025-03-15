/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/15 07:22:14 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"

// *****************************
// irc channel mut be preceeded with # or other prefix. So if no #, no channel should be created
// The operator should have @ preceeding their names
// *****************************

// Example form irssi

// 08:21 -!- ola [~mac@nat-wifi-eduroam-5.czu.cz] has joined #ch1
// 08:21 -!- ServerMode/#ch1 [+Cnst] by zirconium.libera.chat
// 08:21 [Users #ch1]
// 08:21 [@ola]
// 08:21 -!- Irssi: #ch1: Total of 1 nicks [1 ops, 0 halfops, 0 voices, 0 normal]
// 08:21 -!- Channel #ch1 created Mon Mar 10 08:21:08 2025
// 08:21 -!- Irssi: Join to #ch1 was synced in 8 secs

static void broadcastJoinMessage(Channel *channel, Client *sender, const std::string &channel_name) {
	// JOIN message with irssi prefix information.
	std::string joinMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHost() +
						  " JOIN " + channel_name + "\r\n" +
						  "[Users " + channel_name + "]\r\n";
	channel->sendMessageToClients(joinMsg, sender);
	sender->sendMessage(joinMsg, sender->getFd());

	if (!channel->getTopic().empty()) {
		std::string topicReply = "332 " + sender->getNickname() + " " + channel_name + " :" +
								 channel->getTopic() + "\r\n";
		sender->sendMessage(topicReply, sender->getFd());
	}

	// names list (353) and end-of-names (366) replies.
	std::string namesList;
	std::vector<Client *> clients = channel->getClients();
	for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it) {
		std::string nick = (*it)->getNickname();
		irc_log("DEBUG: Adding client nickname: " + nick);
		if (channel->isOperator(*it))
			nick = "@" + nick;
		namesList += nick + " ";
	}
	std::string namesReply = "353 " + sender->getNickname() + " = " + channel_name +
							  " :" + namesList + "\r\n";
	std::string endNamesReply = "366 " + sender->getNickname() + " " + channel_name +
								" :End of /NAMES list\r\n";
	sender->sendMessage(namesReply, sender->getFd());
	sender->sendMessage(endNamesReply, sender->getFd());
}


void Server::joinChannel(Client *sender, std::string message) {
	// Expected format: JOIN <channel> [<key>]
	std::istringstream iss(message);
	std::string command, channel_name, provided_key;
	iss >> command >> channel_name >> provided_key;

	// Validate channel name
	if (channel_name.empty()) {
		sender->sendMessage("Error: Channel name is required.\r\n", sender->getFd());
		return;
	}
	if (channel_name.size() > 50) {
		sender->sendMessage("Error: Channel name must be at most 50 characters.\r\n", sender->getFd());
		return;
	}
	char prefix = channel_name[0];
	if (prefix != '#' && prefix != '&' && prefix != '+' && prefix != '!') {
		sender->sendMessage("Error: Channel name must begin with one of the following characters: &, #, +, or !\r\n", sender->getFd());
		return;
	}
	if (channel_name.find(' ') != std::string::npos ||
		channel_name.find(',') != std::string::npos ||
		channel_name.find('\a') != std::string::npos) {
		sender->sendMessage("Error: Channel name must not contain spaces, commas, or control-G characters.\r\n", sender->getFd());
		return;
	}

	//join or create the channel
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
		broadcastJoinMessage(channel, sender, channel_name);
	} else {
		// Create new channel if it doesn't exist
		std::string channel_password = provided_key;
		channel = createNewChannel(channel_name, channel_password, sender);
		std::string create_message = "Channel created: " + channel_name + "\r\n";
		sender->sendMessage(create_message, sender->getFd());
		std::string join_message = "You have joined the channel: " + channel_name + "\r\n";
		sender->sendMessage(join_message, sender->getFd());
		channel->addOperator(sender);
		broadcastJoinMessage(channel, sender, channel_name);
	}
}

