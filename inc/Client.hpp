#pragma once

#include "Server.hpp"

class Client
{

    private:
    std::string _nickname;
    int         _socket;
    bool        _connected;


    public:
        Client();
        Client(int port_input, const std::string &pswrd);
        Client(const Client &original);
        Client &operator=(const Client &original);
        ~Client();

        std::string _buffer;
        bool _messageComplete;
        std::string _msg;


        std::string getNickname() const;
        int     getSocket() const;
        bool    getConnected() const;
        bool    processInput();


        void commandQuit(std::vector<Client *> &clients, std::string const &reason);

};
