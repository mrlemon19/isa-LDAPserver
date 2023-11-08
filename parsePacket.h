// parsePacket.h
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#ifndef PARSE_PACKET_H
#define PARSE_PACKET_H

#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <algorithm>
#include <iomanip>

#include "byteStream.h"
#include "searchTree.h"
#include "packetSender.h"

void parsePacket(ByteStream bs, int clientSocket, std::string DBfileName);

#endif // PARSE_PACKET_H
