/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Operator.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/10 22:05:05 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"

void Server::makeOperator(Client *client, std::string message) {
	Channel *current_channel = client->getChannel();
	while (!message.empty() && (message[message.size()-1] == '\n' || message[message.size()-1] == '\r')) {
		message.erase(message.size()-1, 1);
	}
	size_t first_space = message.find(' ');
	size_t second_space = message.find(' ', first_space + 1);

	// std::ostringstream oss;
	// oss << "Debug: first_space: " << first_space << ", second_space: " << second_space;
	// irc_log(oss.str());

	if (first_space != std::string::npos && second_space != std::string::npos) {
		std::string username = message.substr(first_space + 1, second_space - (first_space + 1));
		std::string password = message.substr(second_space + 1);

		std::ostringstream oss2;
		oss2 << "Username: '" << username << "', Password: '" << password << "'";
		irc_log(oss2.str());

		if (password == _pswrd) {
			current_channel->addOperator(client);
			client->set_IsOperator(true);
			std::ostringstream oss3;
			oss3 << "Client " << client->getNickname() << " is now an operator.";
			irc_log(oss3.str());
			std::string success_message = "You are now an operator.\r\n";
			client->sendMessage(success_message, client->getFd());
		} else {
			std::string error_message = "Error: Invalid operator password.\r\n";
			client->sendMessage(error_message, client->getFd());
		}
	} else {
		std::string error_message = "Error: Invalid OPER command. Usage: OPER <username> <password>\r\n";
		client->sendMessage(error_message, client->getFd());
	}
}
