#include "Server.hpp"

//TO  DO

void Server::processClientMessage(std::string buffer, std::vector<Client *>::iterator client_it)
{
    buffer = buffer.substr(0, buffer.find("\r\n"));

    Client* client = *client_it;
    
    if (!client->getModSent())
        this->initialMessageTreatment(client, buffer);

    if (client->getconnectionReady() && !client->getModSent())
        if (this->messageOfTheDay(*client)) return;

    struct message input = parseInput(buffer);

    if (input.cmd == "NICK") {
        if (!client->getValidNick())
            client->setNick(buffer, this->clients);
        else
            client->cmdNick(input.params[0], this->clients);
    } else if (input.cmd == "PING")
        cmdPing(input.params, *client);
    else if (input.cmd == "KICK")
        cmdKick(input, *client);
    else if (input.cmd == "MODE")
        cmdMode(input.params, *client);
    else if (input.cmd == "INVITE")
        cmdInvite(input.params, *client);
    else if (input.cmd == "JOIN")
        cmdJoin(input, *client);
    else if (input.cmd == "PRIVMSG")
        cmdPrivmsg(input.params, *client);
    else if (input.cmd == "NOTICE")
        cmdNotice(input.params, *client);
    else if (input.cmd == "WHO")
        cmdWho(input.params, *client);
    else if (input.cmd == "PART")
        cmdPart(input, *client);
    else if (input.cmd == "BOT" || input.cmd == "bot")
        cmdBot(input.params, *client);
    else if (input.cmd == "FTIRC" || input.cmd == "ftirc")
        debugServer(*this, input.params);
    else if (input.cmd == "TOPIC")
        cmdTopic(input.params, *client);
    else if (input.cmd == "QUIT")
        cmdQuit(input, *client);
}


void Client::commandQuit(std::vector<Client *> &clients, std::string const &reason)
{
    
}
