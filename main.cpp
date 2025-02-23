/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eliskam <eliskam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/02/23 21:44:24 by eliskam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./inc/Server.hpp"



int main(int ac, char **av)
{
    int port;
    std::string pswrd = av[2];
    std::istringstream port_input(av[1]);
    
    if(ac != 3)
    {
        std::cerr<<"Bad amount of arguments! Need 2 -> <port> <password>"<<std::endl;
        return (EXIT_FAILURE);
    }
    
    if (!(port_input >> port) || !port_input.eof() || port < 1024 || port > 65535)
    {
		std::cout << "Invalid input for port number!"<<std::endl;
		return (EXIT_FAILURE);
	}

    
}