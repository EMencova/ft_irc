/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/05 15:34:33 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"


void Server::privateMessageClient(Client *sender, std::string message) {
    std::string sender_nickname = sender->getNickname();
    // Assume the message is of the form "PRIVMSG <target> <message>"
    size_t space_pos = message.find(' ', 8); // Position after "PRIVMSG "
    if (space_pos != std::string::npos) {
        // Extract the target (could be a nickname or channel)
        std::string target = message.substr(8, space_pos - 8);
        // Extract the message (everything after the target)
        std::string private_message = message.substr(space_pos + 1);

        // If target starts with '#' assume it's a channel name.
        if (!target.empty() && target[0] == '#') {
            // Look for the channel by name.
            Channel *channel = getChannelByName(target);
            if (channel) {
                // Format a message indicating it's from sender to the channel.
                std::string formatted_message = "[" + sender_nickname + " -> " + target + "]: " + private_message + "\r\n";
                // Broadcast to all clients in the channel except sender.
                channel->sendMessageToClients(formatted_message, sender);
            } else {
                std::string error_message = ERR_NOSUCHCHANNEL("server", target) + "\r\n";
                sender->sendMessage(error_message, sender->getFd());
            }
        } else {
            // Otherwise, treat as a private message to a user.
            Client *target_client = nullptr;
            for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
                if (it->second->getNickname() == target) {
                    target_client = it->second;
                    break;
                }
            }
            if (target_client) {
                std::string formatted_message = "[PM from " + sender_nickname + "]: " + private_message + "\r\n";
                target_client->sendMessage(formatted_message, target_client->getFd());
            } else {
                std::string error_message = ERR_NOSUCHNICK("server", target) + "\r\n";
                sender->sendMessage(error_message, sender->getFd());
            }
        }
    }
}



// void Server::privateMessageClient(Client *sender, std::string message) {
// 	std::string sender_nickname = sender->getNickname();
// 	size_t space_pos = message.find(' ', 8);
// 	if (space_pos != std::string::npos) {
// 		std::string target_nickname = message.substr(8, space_pos - 8);
// 		std::string private_message = message.substr(space_pos + 1);
// 		Client *target = nullptr;
// 		for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
// 			if (it->second->getNickname() == target_nickname) {
// 				target = it->second;
// 				break;
// 			}
// 		}
// 		if (target) {
// 			std::string formatted_message = "[PM from " + sender_nickname + "]: " + private_message + "\r\n";
// 			target->sendMessage(formatted_message, target->getFd());
// 		} else {
// 			std::string error_message = ERR_NOSUCHNICK("server", target_nickname) + "\r\n";
// 			sender->sendMessage(error_message, sender->getFd());
// 		}
// 	}
// }
