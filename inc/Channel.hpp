/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 08:25:24 by mac               #+#    #+#             */
/*   Updated: 2025/03/05 11:21:22 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "Client.hpp"
#include <algorithm>
#include <sys/poll.h>
#include <sys/socket.h>

class Client;

class Channel {
	private:
		typedef std::vector<Client *>::iterator clients_iterator;
		std::string				_name;
		std::string				_password;
		std::vector<Client *>	_clients;
		Client					*_admin;
		std::set<char>			_modes;
		std::string				_topic;
		std::vector<Client *>	_operators;
		std::vector<Client *>	_invitedClients;

	public:
		Channel();
		Channel(std::string &name, std::string &password, Client *client);
		Channel(const Channel &original);
		~Channel();

		Channel &operator=(const Channel &original);

		Client *getAdmin();
		std::string getName();
		std::string getpassword();
		std::vector<Client *> getClients();

		void setName(std::string name);
		void setPassword(std::string password);
		void setAdmin(Client *admin);

		void addClient(Client *client);
		void removeClient(Client *client);

		void sendMessageToClients(std::string message, Client *client);

		//channel modes and commands
		void addMode(char mode);
		void removeMode(char mode);
		bool hasMode(char mode) const;
		std::string getModes() const;

		void setTopic(const std::string &topic);
		std::string getTopic() const;

		void addOperator(Client *client);
		void removeOperator(Client *client);
		bool isOperator(Client *client) const;

		void addInvited(Client *client);
		bool isInvited(Client *client) const;
		void removeInvited(Client *client);
};

#endif
