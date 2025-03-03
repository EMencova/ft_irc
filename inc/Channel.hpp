/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 08:25:24 by mac               #+#    #+#             */
/*   Updated: 2025/03/03 07:42:16 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include <string>
#include "Client.hpp"
#include <vector>
#include <algorithm>

class Client;

class Channel {
	private:
		typedef std::vector<Client *>::iterator clients_iterator;
		std::string			_name;
		std::string			_password;
		std::vector<Client *>	_clients;
		Client				*_admin;

	public:
		Channel();
		Channel(std::string &name, std::string &password, Client *client);
		Channel(const Channel &original);
		~Channel();

		Channel &operator=(const Channel &original);

		Client *getAdmin();
		std::string getName();
		std::string	getpassword();
		std::vector<Client *> getClients();

		void setName(std::string name);
		void setPassword(std::string password);
		void setAdmin(Client *admin);


		void addClient(Client *client);
		void removeClient(Client *client);

		void sendMessageToClients(std::string message, Client *client);
};
#endif
