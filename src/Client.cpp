#include "Client.hpp"

Client::Client(){};

Client::Client(int port_input, const std::string &pswrd)
{
    
}

Client::Client(const Client &original)
{
    _socket = original._socket;
    _nickname = original._nickname;
}

Client &Client::operator=(const Client &original)
{
    if (this != &original)
    {
        _socket = original._socket;
        _nickname = original._nickname;
        
    }
    return(*this);
}


Client::~Client(){};

#pragma region GETTERS

std::string Client::getNickname() const
{
    return (_nickname);
}

int Client::getSocket() const
{
    return _socket;
}

bool Client::getConnected() const
{
    return (_connected);
}



#pragma endregion

#pragma region PROCESSES

bool Client::processInput()
{
    std::size_t pos = _buffer.find("\r\n");
	if (pos == std::string::npos)
		return (false);
	_msg = _buffer.substr(0, pos + 2);
	_buffer.erase(0, pos + 2);
	_messageComplete= true;
	return (true);
}