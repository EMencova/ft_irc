/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dcc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vconesa- <vconesa-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 16:14:44 by vconesa-          #+#    #+#             */
/*   Updated: 2025/03/20 11:03:08 by vconesa-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Client.hpp"
#include "../../inc/response.hpp"


uint32_t ipToDecimal(const std::string &ip) {
    struct in_addr addr;
    if (inet_aton(ip.c_str(), &addr) == 0) {
        return 0;
    }
    return ntohl(addr.s_addr);
}

std::string getClientIP(int client_fd) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    if (getsockname(client_fd, (struct sockaddr *)&addr, &addr_len) == -1) {
        return "";
    }
    return inet_ntoa(addr.sin_addr);
}

std::string decimalToIP(uint32_t ip_decimal) {
    struct in_addr ip_addr;
    ip_addr.s_addr = htonl(ip_decimal);
    return inet_ntoa(ip_addr);
}

void Client::handleDCCSend(Client *target, std::string message) {
    std::istringstream iss(message.substr(9)); // substract "DCC SEND "
    std::string filename, file;
    uint32_t ip_decimal;
    int port, file_size;

    std::string sender_nickname = this->getNickname();
    std::string sender_ident = this->getUsername();
    std::string sender_host = this->getHost();


    if (!(iss >> filename >> ip_decimal >> port >> file_size)) {
            this->sendMessage("Invalid DCC SEND format\r\n", this->getFd());
            return;
        }

    std::string ip_string = decimalToIP(ip_decimal);

    const char *home = getenv("HOME");
    file = filename;
    if (home) {
        filename = std::string(home) + "/" + filename;
    }

    std::ifstream infile(filename.c_str(), std::ifstream::binary);
    if (!infile) {
        this->sendMessage("Error: File not found\r\n", this->getFd());
        return;
    }
    
    // get client IP
    std::string client_ip = getClientIP(this->getFd());
    if (client_ip.empty()) {
        this->sendMessage("Error retrieving IP\r\n", this->getFd());
        return;
    }

    uint32_t decimal_ip = ipToDecimal(client_ip);

    // send DCC message to target
    std::ostringstream dcc_message;
    dcc_message << ":" + sender_nickname + "!" + sender_ident + "@" + sender_host + " PRIVMSG " 
                << target->getNickname() << " :\001DCC SEND " << file << " " << decimal_ip << " " << port 
                << " " << file_size << "\001\r\n";
  
    target->sendMessage(dcc_message.str(), target->getFd());
    infile.close();
}
