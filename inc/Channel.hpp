/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 08:25:24 by mac               #+#    #+#             */
/*   Updated: 2025/03/02 11:27:07 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <set>
#include <string>
#include "Client.hpp"



class Channel {
	private:
		std::string					_name;
		std::string					_password;
		std::set<Client *>			_clients;

		void removeClient(Client *client);
		void addClient(Client *client);

	public:
		Channel();
		Channel::Channel(std::string &name, std::string &password, Client *client);
		Channel(const Channel &original);
		~Channel();

		Channel &operator=(const Channel &original);
		std::string getName();
		std::string	getpassword();


};
#endif
