#pragma once

#include "Server.hpp"

class Client
{

    private:


    public:
        Client();
        Client(int port_input, const std::string &pswrd);
        Client(const Client &original);
        Client &operator=(const Client &original);
        ~Client();
        std::string getNick();

};
