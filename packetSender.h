// packetSender.h
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#ifndef PACKET_SENDER_H
#define PACKET_SENDER_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sys/socket.h>

class packetSender
{
    public:
        packetSender(int clientSocket);
        void sendPacketLDAP(std::vector<unsigned char> response, int messageID);

        int clientSocket;
};

#endif // PACKET_SENDER_H
