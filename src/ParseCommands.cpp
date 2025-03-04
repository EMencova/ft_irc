/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseCommands.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 10:55:28 by mac               #+#    #+#             */
/*   Updated: 2025/03/04 10:57:03 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ParseCommands.hpp"


// class ParseCommands {
// 	private:
// 		Server *_server;
// 		std::vector<std::string> _commands;

// 	public:
// 		ParseCommands();
// 		~ParseCommands();
// 		void callCommand(std::string command);
// };

ParseCommands::ParseCommands(Server *server) : _server(server) {
}

ParseCommands::~ParseCommands() {
}

void ParseCommand::callCommand(std::string command) {
	if (command == "/create") {
		_server->createNewChannel();
	}
	else if (command == "/join") {
		_server->joinChannel();
	}
	else if (command == "/leave") {
		_server->leaveChannel();
	}
	else if (command == "/list") {
		_server->listChannels();
	}
	else if (command == "/msg") {
		_server->sendPrivateMessage();
	}
	else if (command == "/nick") {
		_server->changeNickname();
	}
	else if (command == "/exit") {
		_server->exitServer();
	}
	else {
		std::cout << "Invalid command" << std::endl;
	}
}
