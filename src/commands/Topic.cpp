/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/10 14:36:16 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"

void Server::handleTopic(Client *sender, std::string message) {
	// Format: TOPIC <channel> [<new topic>]
	std::istringstream iss(message);
	std::string command, channel_name;
	iss >> command >> channel_name;
	if (channel_name.empty()) {
		sender->sendMessage("Error: Invalid TOPIC command format. Usage: TOPIC <channel> [<new topic>]\r\n", sender->getFd());
		return;
	}
	Channel *channel = getChannelByName(channel_name);
	if (!channel) {
		sender->sendMessage(ERR_NOSUCHCHANNEL("server", channel_name) + "\r\n", sender->getFd());
		return;
	}
	std::string new_topic;
	std::getline(iss, new_topic);
	size_t start = new_topic.find_first_not_of(" ");
	if (start != std::string::npos)
		new_topic = new_topic.substr(start);
	else
		new_topic = "";

	if (new_topic.empty()) {
		std::string current_topic = channel->getTopic();
		if (current_topic.empty())
			sender->sendMessage("No topic is set for channel " + channel_name + ".\r\n", sender->getFd());
		else
			sender->sendMessage("Current topic for " + channel_name + ": " + current_topic + "\r\n", sender->getFd());
	} else {
		if (channel->hasMode('t') && !channel->isOperator(sender)) {
			sender->sendMessage(ERR_CHANOPRIVSNEEDED("server", channel_name) + "\r\n", sender->getFd());
			return;
		}
		channel->setTopic(new_topic);
		//using standard IRC syntax.
		std::string topicBroadcast = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHost() +
									 " TOPIC " + channel_name + " :" + new_topic + "\r\n";
		channel->sendMessageToClients(topicBroadcast, sender);
		sender->sendMessage("Topic updated.\r\n", sender->getFd());
	}
}
