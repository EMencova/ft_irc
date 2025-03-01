
#include "Server.hpp"
#include <ctime>

std::string	getCurrentDateTime()
{
	std::time_t now = std::time(0);
	std::tm	*localTime = std::localtime(&now);
	char		buffer[100];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
	return std::string(buffer);
}

void	printInfoServer(Server* server)
{
	std::cout << "Hostname is : " << (*server).getHostname() << std::endl;
	std::cout << "Port is : " << (*server).getPort() << std::endl;
	std::cout << "Password is : " << (*server).getPassword() << std::endl;
	std::cout << "Start time is : " << (*server).getStartTime() << std::endl;
}