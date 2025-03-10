/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Operator.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eliskam <eliskam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/05 17:34:24 by eliskam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"

void Server::makeOperator(Client *client, std::string message) {
	while (!message.empty() && (message[message.size() - 1] == '\n' || message[message.size() - 1] == '\r'))
		message.erase(message.size() - 1);
	size_t first_space = message.find(' ');
	size_t second_space = message.find(' ', first_space + 1);
	irc_log("Debug: first_space: " + toString(first_space) + ", second_space: " + toString(second_space));

	if (first_space != std::string::npos && second_space != std::string::npos) {
		std::string username = message.substr(first_space + 1, second_space - (first_space + 1));
		std::string password = message.substr(second_space + 1);
		irc_log("Debug: Username: '" + username + "', Password: '" + password + "'");
		if (password == _pswrd) {
			client->set_IsOperator(true);
			irc_log("Debug: Client " + client->getNickname() + " is now an operator.");
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
