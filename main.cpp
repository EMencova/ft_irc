/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eliskam <eliskam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 13:13:22 by emencova          #+#    #+#             */
/*   Updated: 2025/03/01 20:59:13 by eliskam          ###   ########.fr       */
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
		std::cerr << "Invalid input for port number!"<<std::endl;
		return (EXIT_FAILURE);
	}

    Server ft_irc(port,pswrd);

    //need sig handling
    

    if(!ft_irc.irc_started())
    {
        std::cout<<"Server starting error!"<<std::endl;
        return (EXIT_FAILURE);
    }
    

    
}