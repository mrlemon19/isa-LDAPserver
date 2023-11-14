// packetSender.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#include "packetSender.h"

packetSender::packetSender(int clientSocket) {
  this->clientSocket = clientSocket;
}

void packetSender::sendPacketLDAP(std::vector<unsigned char> response,
                                  int messageID) {

  response.push_back(static_cast<char>(messageID));
  response.push_back(0x01);
  response.push_back(0x02);
  response.push_back(static_cast<char>(response.size()));
  response.push_back(0x30);

  std::reverse(response.begin(), response.end());

  // sends response

  send(this->clientSocket, response.data(), response.size(), 0);
}
