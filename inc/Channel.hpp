/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 08:25:24 by mac               #+#    #+#             */
/*   Updated: 2025/03/05 06:40:41 by mac              ###   ########.fr       */
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
		std::string				_name;
		std::string				_password;
		std::vector<Client *>	_clients;
		Client					*_admin;
		std::vector<Client *> _invitedClients; // List of invited clients for +i mode

		//chanop
		bool _inviteOnly; // +i
    	bool _topicRestricted; // +t
    	std::string _channelKey; // +k
    	std::vector<Client *> _operators; // List of channel operators
		std::string _topic; // Channel topic

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

		//chanop
		void setInviteOnly(bool inviteOnly);
		bool isInviteOnly() const;

		// Topic-related methods
		void setTopic(const std::string &topic);
		std::string getTopic() const;

		void setTopicRestricted(bool topicRestricted);
		bool isTopicRestricted() const;

		void setChannelKey(const std::string &key);
		std::string getChannelKey() const;

		void addOperator(Client *client);
		void removeOperator(Client *client);
		bool isOperator(Client *client) const;

		// Invite-related methods
		void addInvitedClient(Client *client);
		bool isClientInvited(Client *client) const;

		void sendMessageToClients(std::string message, Client *client);
};
#endif
