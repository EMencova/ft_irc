#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <vector>
#include <string>
#include <Server.hpp>
#include <numeric>


class Command {
	private:
		Server *_server;
		std::vector<std::string> _commands;

	public:
		Command(Server *server);
		~Command();
		void callCommand(std::string command);
};

#endif

// #ifndef COMMAND_HPP

