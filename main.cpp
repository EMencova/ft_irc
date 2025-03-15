/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/15 07:36:22 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./inc/Server.hpp"
#include <signal.h>
#include <cstdlib>
#include <iostream>
#include <sstream>

// Global pointer to our server instance.
Server *g_server = NULL;

// Signal handler for SIGINT.
void sigint_handler(int signum) {
	(void)signum; // suppress unused parameter warning
	if (g_server) {
		g_server->closeServer();
	}
	exit(0);
}

int main(int argc, char **argv)
{
	int port;
	if (argc != 3) {
		std::cerr << "Bad amount of arguments! Need 2 -> <port> <password>" << std::endl;
		return EXIT_FAILURE;
	}

	std::string pswrd = argv[2];
	std::istringstream port_input(argv[1]);
	if (!(port_input >> port) || !port_input.eof() || port < 1024 || port > 65535) {
		std::cout << "Invalid input for port number!" << std::endl;
		return EXIT_FAILURE;
	}

	try {
		// Create server instance.
		Server server(argv[1], argv[2]);
		// Set the global pointer.
		g_server = &server;
		// Install the SIGINT handler.
		signal(SIGINT, sigint_handler);

		server.startServer();
		return 0;
	} catch (const std::exception &ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
}
