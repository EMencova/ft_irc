/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/01 23:43:51 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

Server::Server(){};

Server::Server(const std::string &port, const std::string &pswrd)
		: _running(1), _host("127.0.0.1"), _port(port), _pswrd(pswrd) {
		_socket = createNewSocket();

}

Server::~Server(){};


int Server::createNewSocket(){
	// server: fds, memset(), bind(), listen(), accept()
	// client:

	int socket_fd;

// 	struct sockaddr_in {
// 	__uint8_t       sin_len;
// 	sa_family_t     sin_family;
// 	in_port_t       sin_port;
// 	struct  in_addr sin_addr;
// 	char            sin_zero[8];
// };
	struct sockaddr_in server_addr;

	if ((socket_fd = socket(AF_INET, SOCK_STREAM , 0)) < 0){
		std::cerr << ("socket_fd failed");
		close (socket_fd);
		return (1);
	}

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(std::stoi(_port));
	// server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(socket_fd, (sockaddr *) &server_addr, sizeof(server_addr)) < 0){
		perror ("bind failed");
		close (socket_fd);
		return (1);
	}

	if ((listen(socket_fd, MAXCLIENT)) < 0) {
		perror ("listen failed");
		close (socket_fd);
		return (1);
	}

	return socket_fd;
}
