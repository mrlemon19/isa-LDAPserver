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

#include "byteStream.h"

void parsePacket(ByteStream bs, int clientSocket);

#endif // PARSE_PACKET_H
