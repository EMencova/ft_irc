/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/14 17:08:18 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./inc/Server.hpp"

int main(int argc, char **argv)
{
	int port;
	std::string pswrd = argv[2];
	std::istringstream port_input(argv[1]);

	try {
		if(argc != 3) {
			std::cerr<<"Bad amount of arguments! Need 2 -> <port> <password>"<<std::endl;
			return (EXIT_FAILURE);
		}

		else if (!(port_input >> port) || !port_input.eof() || port < 1024 || port > 65535) {
			std::cout << "Invalid input for port number!"<<std::endl;
			return (EXIT_FAILURE);
		}
		else {
			Server server(argv[1], argv[2]);
			server.startServer();
			return 0;
		}

	} catch (const std::exception &ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
}
