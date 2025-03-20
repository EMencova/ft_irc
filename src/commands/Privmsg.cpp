/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vconesa- <vconesa-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/20 10:59:06 by vconesa-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"


	// Assume the message is of the form "PRIVMSG <target> <message>"
			// Extract the target (could be a nickname or channel)
				// Extract the message (everything after the target)
				// If target starts with '#' assume it's a channel name.
					// Look for the channel by name.
							// Broadcast to all clients in the channel except sender.
				// Otherwise, treat as a private message to a user.
				// Format a message indicating it's from sender to the channel.


// :<sender_nick>!<sender_ident>@<sender_host> PRIVMSG <target> :<message>\r\n

void Server::privateMessageClient(Client *sender, std::string message) {
	std::string sender_nickname = sender->getNickname();
	std::string sender_ident = sender->getUsername();
	std::string sender_host = sender->getHost();

	size_t space_pos = message.find(' ', 8); // Skip "PRIVMSG "
	if (space_pos != std::string::npos) {
		std::string target = message.substr(8, space_pos - 8);
		std::string private_message = message.substr(space_pos + 1);

		std::string clean_message = private_message;
		if (clean_message[0] == ':') {
			clean_message = clean_message.substr(1);
		}
		
		clean_message = clean_message.substr(clean_message.find_first_not_of(" "));

		// remove invisible control characters (example: 0x01 SOH)
		while (!clean_message.empty() && (clean_message[0] < 32)) {
			clean_message.erase(0, 1);
		}

		clean_message.erase(clean_message.find_last_not_of("\r\n") + 1);

		if (clean_message.find("DCC SEND") == 0) {
			Client *target_client = getClientByNickname(target);
			if (!target_client) {
				sender->sendMessage(ERR_NOSUCHNICK("server", target) + "\r\n", sender->getFd());
				return;
			}
			sender->handleDCCSend(target_client, clean_message);
			return;
		}	
		//IRC-formatted message
		std::string formatted_message = ":" + sender_nickname + "!" + sender_ident + "@" + sender_host +
			" PRIVMSG " + target + " :" + private_message + "\r\n";

		if (!target.empty() && target[0] == '#') {
			Channel *channel = getChannelByName(target);
			if (channel) {
				if (sender->getChannel() != channel) {
					std::string error_message = ERR_NOTONCHANNEL("server", target) + "\r\n";
					sender->sendMessage(error_message, sender->getFd());
					return;
				}
				channel->sendMessageToClients(formatted_message, sender);
			} else {
				std::string error_message = ERR_NOSUCHCHANNEL("server", target) + "\r\n";
				sender->sendMessage(error_message, sender->getFd());
			}
		}
		else {
			Client *target_client = NULL;
			for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
				if (it->second->getNickname() == target) {
					target_client = it->second;
					break;
				}
			}
			if (target_client) {
				target_client->sendMessage(formatted_message, target_client->getFd());
			} else {
				std::string error_message = ERR_NOSUCHNICK("server", target) + "\r\n";
				sender->sendMessage(error_message, sender->getFd());
			}
		}
	}
}

// to see private messages in irssi client
// **/query <nick>*
// in the main screen, you can also send the message as /msg <nick> <your_message>
