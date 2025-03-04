#ifndef PARSECOMMANDS_HPP
#define PARSECOMMANDS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <Server.hpp>

class ParseCommands {
	private:
		Server *_server;
		std::vector<std::string> _commands;

	public:
		ParseCommands(Server *server);
		~ParseCommands();
		void callCommand(std::string command);
};

#endif
