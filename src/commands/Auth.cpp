/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eliskam <eliskam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/05 17:34:00 by eliskam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"

void Server::authenticateClient(Client *client) {
	int client_fd = client->getFd();

	std::string welcome_message = "Welcome to the server. Please authenticate using the PASS command.\r\n";
	send(client_fd, welcome_message.c_str(), welcome_message.size(), 0);

	std::string message;
	while (true) {
		message = readMessage(client_fd, client);
		if (!message.empty()) {
			if (message.find("PASS ") == 0) {
				std::string password = message.substr(5);
				while (!password.empty() && (password[password.size() - 1] == '\n' || password[password.size() - 1] == '\r'))
					password.erase(password.size() - 1);
				irc_log("Debug: Received password from FD " + toString(client_fd) +
				        ": '" + password + "'");

				if (password == _pswrd) {
					irc_log("Client FD " + toString(client_fd) + " authenticated successfully.");
					client->setRegistered(true);

					std::string success_message = RPL_WELCOME("server") + "\r\n";
					send(client_fd, success_message.c_str(), success_message.size(), 0);
					break;
				} else {
					std::string error_message = ERR_PASSWDMISMATCH("server") + "\r\n";
					send(client_fd, error_message.c_str(), error_message.size(), 0);
					thisClientDisconnect(client_fd);
					return;
				}
			} else {
				std::string error_message = ERR_NOTREGISTERED("server") + "\r\n";
				send(client_fd, error_message.c_str(), error_message.size(), 0);
				thisClientDisconnect(client_fd);
				return;
			}
		}
		usleep(100000);
	}
}
