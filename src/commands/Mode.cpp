/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/10 14:32:53 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"

void Server::handleMode(Client *sender, std::string message) {
	// Format: MODE <channel> <modes> [<mode parameters>]
	std::istringstream iss(message);
	std::string command, channel_name, mode_changes;
	iss >> command >> channel_name >> mode_changes;
	if (channel_name.empty() || mode_changes.empty()) {
		sender->sendMessage("Error: Invalid MODE command format. Usage: MODE <channel> <modes> [<parameters>]\r\n", sender->getFd());
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
	char sign = mode_changes[0];
	if (sign != '+' && sign != '-') {
		sender->sendMessage("Error: Mode changes must start with '+' or '-'.\r\n", sender->getFd());
		return;
	}
	for (size_t i = 1; i < mode_changes.size(); i++) {
		char mode = mode_changes[i];
		if (sign == '+') {
			if (mode == 'i' || mode == 't') {
				channel->addMode(mode);
				std::string mode_msg = "Mode +";
				mode_msg.push_back(mode);
				mode_msg += " set for channel " + channel_name + ".\r\n";
				sender->sendMessage(mode_msg, sender->getFd());
			} else if (mode == 'k') {
				std::string key;
				iss >> key;
				if (key.empty()) {
					sender->sendMessage("Error: Mode +k requires a key parameter.\r\n", sender->getFd());
				} else {
					channel->setPassword(key);
					channel->addMode('k');
					std::string mode_msg = "Mode +k set for channel " + channel_name + " with key.\r\n";
					sender->sendMessage(mode_msg, sender->getFd());
				}
			} else if (mode == 'o') {
				std::string target_nickname;
				iss >> target_nickname;
				if (target_nickname.empty()) {
					sender->sendMessage("Error: Mode +o requires a nickname parameter.\r\n", sender->getFd());
				} else {
					std::vector<Client *> clients = channel->getClients();
					Client *target = NULL;
					for (size_t j = 0; j < clients.size(); j++) {
						if (clients[j]->getNickname() == target_nickname) {
							target = clients[j];
							break;
						}
					}
					if (!target) {
						sender->sendMessage("Error: Client " + target_nickname +
							" is not in channel " + channel_name + ".\r\n", sender->getFd());
					} else {
						channel->addOperator(target);
						std::string mode_msg = "Mode +o: " + target_nickname +
							" is now a channel operator in " + channel_name + ".\r\n";
						sender->sendMessage(mode_msg, sender->getFd());
						target->sendMessage("You have been granted channel operator privileges in " +
							channel_name + ".\r\n", target->getFd());
						// Broadcast the mode change.
						std::string modeBroadcast = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHost() +
													" MODE " + channel_name + " +o " + target_nickname + "\r\n";
						channel->sendMessageToClients(modeBroadcast, sender);
					}
				}
			} else {
				sender->sendMessage("Error: Unknown mode flag.\r\n", sender->getFd());
			}
		} else if (sign == '-') {
			if (mode == 'i' || mode == 't') {
				channel->removeMode(mode);
				std::string mode_msg = "Mode -";
				mode_msg.push_back(mode);
				mode_msg += " removed for channel " + channel_name + ".\r\n";
				sender->sendMessage(mode_msg, sender->getFd());
			} else if (mode == 'k') {
				channel->setPassword("");
				channel->removeMode('k');
				std::string mode_msg = "Mode -k removed for channel " + channel_name + ".\r\n";
				sender->sendMessage(mode_msg, sender->getFd());
				} else if (mode == 'o') {
				std::string target_nickname;
				iss >> target_nickname;
				if (target_nickname.empty()) {
					sender->sendMessage("Error: Mode -o requires a nickname parameter.\r\n", sender->getFd());
				} else {
					std::vector<Client *> clients = channel->getClients();
					Client *target = NULL;
					for (size_t j = 0; j < clients.size(); j++) {
						if (clients[j]->getNickname() == target_nickname) {
							target = clients[j];
							break;
						}
					}
					if (!target) {
						sender->sendMessage("Error: Client " + target_nickname +
							" is not in channel " + channel_name + ".\r\n", sender->getFd());
					} else {
						channel->removeOperator(target);
						std::string mode_msg = "Mode -o: " + target_nickname +
							" is no longer a channel operator in " + channel_name + ".\r\n";
						sender->sendMessage(mode_msg, sender->getFd());
						target->sendMessage("Your channel operator privileges have been removed in " +
							channel_name + ".\r\n", target->getFd());
						// Broadcast the mode change.
						std::string modeBroadcast = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHost() +
													" MODE " + channel_name + " -o " + target_nickname + "\r\n";
						channel->sendMessageToClients(modeBroadcast, sender);
					}
				}
			} else {
				sender->sendMessage("Error: Unknown mode flag.\r\n", sender->getFd());
			}
		}
	}
}
