/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 11:27:18 by mac               #+#    #+#             */
/*   Updated: 2025/03/15 07:19:29 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/response.hpp"

void Server::authenticateClient(Client *client) {
	int client_fd = client->getFd();

	std::string welcome_message = "Welcome to the server. Please authenticate using the PASS command.\r\n";
	send(client_fd, welcome_message.c_str(), welcome_message.size(), 0);

	std::string message;
		message = readMessage(client_fd, client);
		if (!message.empty()) {
			if (message.find("PASS ") == 0) {
				std::string password = message.substr(5);
				// Remove any trailing newline or carriage return characters
				while (!password.empty() && (password[password.size()-1] == '\n' || password[password.size()-1] == '\r')) {
					password.erase(password.size()-1, 1);
				}
				// std::ostringstream oss, user_oss, nick_oss;
				std::ostringstream user_oss, nick_oss;
				// oss << "Debug: Received password from FD " << client_fd << ": '" << password << "'";
				user_oss << "user" << client_fd;
				nick_oss << "nick" << client_fd;
				// irc_log(oss.str());

				if (password == _pswrd) {
					std::ostringstream oss2;
					oss2 << "Client FD " << client_fd << " authenticated successfully.";
					irc_log(oss2.str());
					client->setRegistered(true);
					client->setNickname(user_oss.str());
					client->setUsername(nick_oss.str());

					std::ostringstream auth_details;
					auth_details << client->getNickname() << "!" << client->getUsername() << "@localhost";

					std::string success_message = RPL_WELCOME(auth_details.str()) + "\r\n";
					send(client_fd, success_message.c_str(), success_message.size(), 0);
					return ;
				} else {
					std::string error_message = ERR_PASSWDMISMATCH("server") + "\r\n";
					send(client_fd, error_message.c_str(), error_message.size(), 0);
					// thisClientDisconnect(client_fd);
					return;
				}
			} else {
				std::string error_message = ERR_NOTREGISTERED("server") + "\r\n";
				send(client_fd, error_message.c_str(), error_message.size(), 0);
				// thisClientDisconnect(client_fd);
				return;
			}
	}
}
